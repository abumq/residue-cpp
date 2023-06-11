//
//  residue.cc
//  Residue C++
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present @abumq (Majid Q.)
//
//  See https://github.com/abumq/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#include <ctime>
#include <cstdlib>
#include <memory>
#include <chrono>
#include <functional>
#include "ripe/Ripe.h"
#include "nlohmann-json/json.h"
#include "include/residue.h"
#include "internal-logger.h"
#include "dispatcher.h"
#include "network-client.h"
#include "log.h"

INITIALIZE_EASYLOGGINGPP

const std::size_t Residue::DEFAULT_KEY_SIZE = 2048;
const std::string Residue::LOCALHOST = "127.0.0.1";
const int Residue::DEFAULT_PORT = 8777;
const unsigned int Residue::TOUCH_THRESHOLD = 60;

using json = nlohmann::json;

#define RESIDUE_LOCK_LOG(id) InternalLogger(InternalLogger::crazy) << id << "_lock()";\
    InternalLogger scoped_InternalLogger(InternalLogger::crazy); \
    scoped_InternalLogger << id << "_unlock()";

volatile int Residue::s_internalLoggingLevel = InternalLogger::none;

static std::unique_ptr<NetworkClient> s_connectionClient;
static std::unique_ptr<NetworkClient> s_loggingClient;

void residueCrashHandler(int sig) noexcept
{
    InternalLogger(InternalLogger::error) << "Crashed!";
    if (!Residue::instance().crashHandlerLoggerId().empty()) {
        CLOG(ERROR, Residue::instance().crashHandlerLoggerId().c_str()) << "Crashed!";
        el::Helpers::logCrashReason(sig, true, el::Level::Fatal,
                                    Residue::instance().crashHandlerLoggerId().c_str());
    }
    Residue::disconnect();
    el::Helpers::crashAbort(sig);
}

Residue::Residue() noexcept :
#ifdef RESIDUE_PROFILING
    queue_ms(0), dispatch_ms(0), c_total(0),
#endif
    m_host(""),
    m_port(0),
    m_connected(false),
    m_connecting(false),
    m_disconnected(false),
    m_bulkDispatch(false),
    m_bulkSize(1),
    m_maxBulkSize(1),
    m_keySize(DEFAULT_KEY_SIZE),
    m_age(0),
    m_dateCreated(0),
    m_loggingPort(0),
    m_serverFlags(0x0),
    m_utc(false),
    m_timeOffset(0),
    m_dispatchDelay(1),
    m_dispatcher(nullptr),
    m_running(false),
    m_autoBulkParams(true),
    m_knownClient(false)
{
    InternalLogger(InternalLogger::debug) << "Initialized residue!";
}

Residue::~Residue() noexcept
{
    Residue::disconnect();
}

void Residue::healthCheck() noexcept
{
    if (!isClientValid()) {
        try {
            InternalLogger(InternalLogger::warning) << "health check - reset";
            reset();
        } catch (const std::exception& e) {
            // We ignore these errors as we have
            // log messages from the reset()
        }
    } else if (shouldTouch()) {
        InternalLogger(InternalLogger::warning) << "health check - touch";
        touch();
    }
}

void Residue::connect_(const std::string& host, int port, bool estabilishFullConnection)
{
    if (m_connected) {
        InternalLogger(InternalLogger::error) << "Already connected. Please disconnect first";
        return;
    }
    InternalLogger(InternalLogger::info) << "Connecting...";

    // This method is only called by user so we want to reset disconnected state
    m_disconnected = false;

    m_host = host;
    m_port = port;

    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);

    s_connectionClient = std::unique_ptr<NetworkClient>(
                new NetworkClient(m_host, std::to_string(m_port)));

    try {
        s_connectionClient->connect();
        if (!s_connectionClient->connected()) {
            addError("Failed to connect. (Port: " + std::to_string(m_port) + ") " + s_connectionClient->lastError());
            throw ResidueException(m_errors.at(m_errors.size() - 1));
        }
    } catch (const ResidueException& e) {
        throw e;
    }

    if (estabilishFullConnection) {
        if (!m_knownClient) {
            // Generate new key pair
    #ifdef RESIDUE_PROFILING
            TIMED_SCOPE(timed, "gen_rsa");
    #endif // ELPP_DEBUG_LOG
            Ripe::KeyPair rsaKey = Ripe::generateRSAKeyPair(m_keySize);
            m_rsaPrivateKey = rsaKey.privateKey;
            m_rsaPublicKey = rsaKey.publicKey;
        }
        InternalLogger(InternalLogger::info) << "Estabilishing full connection...";
        reset();
    }

}

void Residue::onConnect() noexcept
{
    InternalLogger(InternalLogger::info) << "Successfully connected";

    // Register callbacks (and unregister default callback)
    el::Helpers::uninstallLogDispatchCallback<el::base::DefaultLogDispatchCallback>("DefaultLogDispatchCallback");
    el::Helpers::installLogDispatchCallback<ResidueDispatcher>("ResidueDispatcher");
    ResidueDispatcher* dispatcher = el::Helpers::logDispatchCallback<ResidueDispatcher>("ResidueDispatcher");
    dispatcher->setEnabled(true);
    dispatcher->setResidue(this);

    if (!m_running) {
        // we do not want to cancel existing thread so we use m_running as reference point
        m_running = true;
        //constant connect and disconnect may cause memory leak as of
        // v2.1.0+
        // following delete causes failure in some situations
        if (m_dispatcher != nullptr) {
            InternalLogger(InternalLogger::debug) << "Delete dispatcher...";
            delete m_dispatcher;
            m_dispatcher = nullptr; // just in case if next line fails due to insufficient memory
            InternalLogger(InternalLogger::debug) << "Dispatcher deleted!";
        }
        InternalLogger(InternalLogger::debug) << "Init dispatcher...";
        m_dispatcher = new std::thread([&]() {
            while (m_running) {
                dispatch();
                if ((!m_connected || m_disconnected) && m_requests.empty()) {
                    InternalLogger(InternalLogger::debug) << "Force disconnect";
                    disconnect_();
                    m_running = false;
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(m_dispatchDelay));
            }
            InternalLogger(InternalLogger::debug) << "Finishing residue...";
        });
    } else {
        InternalLogger(InternalLogger::debug) << "Dispatcher already initialized...";
    }
}

void Residue::reset()
{
    InternalLogger(InternalLogger::debug) << "Resetting connection...";
    m_connecting = true;
    m_connected = false;
    RESIDUE_LOCK_LOG("reset");
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    m_errors.clear();
    int type = 1; // 1 = CONNECT

    json j;
    j["type"] = type;
    j["_t"] = getTimestamp();
    if (!m_knownClient) {
        j["rsa_public_key"] = Ripe::base64Encode(m_rsaPublicKey);
    } else {
        j["client_id"] = m_clientId;
    }

    InternalLogger(InternalLogger::debug) << "Connecting...(1)";

    std::string data = j.dump();
    if (!m_serverPublicKey.empty()) {
        data = Ripe::base64Encode(Ripe::encryptRSA(data, m_serverPublicKey));
    }
    data += Ripe::PACKET_DELIMITER;
    try {
        s_connectionClient->send(std::move(data), true,
                                 [&](std::string&& connectResponse, bool hasError, std::string&& errorText) -> void {
            m_connecting = false;
            if (hasError || connectResponse.empty()) {
                InternalLogger(InternalLogger::error) << "Failed to connect!";
                addError("Failed to connect to the residue server. " + errorText);
                throw ResidueException(m_errors.at(m_errors.size() - 1));
            } else {
                InternalLogger(InternalLogger::debug) << "Connecting...(2)";
                std::string decryptedResponse;
                if (connectResponse.at(0) == '{') {
                    decryptedResponse = connectResponse;
                } else {
                    try {
                        decryptedResponse = Ripe::decryptRSA(connectResponse, m_rsaPrivateKey,
                                                             true, false, m_rsaPrivateKeySecret);
                    } catch (const std::exception& e) {
                        InternalLogger(InternalLogger::error) << "Failed to read response: " << e.what();
                        addError("Client private key does not match with the corresponding public key on the server.");
                        throw ResidueException(m_errors.at(m_errors.size() - 1));
                    }
                }

                int result = decryptedResponse.empty() ? -1 : 0;
                if (result != -1) {

                    InternalLogger(InternalLogger::debug) << "Connecting...(2) > Parsing...";

                    try {
                        j = json::parse(decryptedResponse);
                        if (j.count("status") == 1 && j["status"].get<int>() == 0) {
                            m_key = j["key"].get<std::string>();

                            // This two-step setting is necessary otherwise setting it directly to m_clientId gives
                            // memory error:
                            //
                            // malloc: *** error for object 0x10e5bd1c0: pointer being freed was not allocated
                            // *** set a breakpoint in malloc_error_break to debug
                            //
                            std::string clientId = j["client_id"].get<std::string>();
                            m_clientId = clientId;
                            m_connecting = true;
                            InternalLogger(InternalLogger::debug) << "Connecting...(3)";
                            json j2;
                            type = 2; // 2 = ACKNOWLEDGE
                            j2["type"] = type;
                            j2["_t"] = getTimestamp();
                            j2["client_id"] = m_clientId;
                            std::string jsonRequest = j2.dump();
                            std::string ackRequest = Ripe::prepareData(jsonRequest.c_str(), m_key, m_clientId.c_str());
                            try {
                                s_connectionClient->send(ackRequest.c_str(), true, [&](std::string&& ackResponse,
                                                         bool hasError, std::string&& errorText) -> void {
                                    InternalLogger(InternalLogger::debug) << "Saving connection parameters...";
                                    m_connecting = false;
                                    if (hasError) {
                                        InternalLogger(InternalLogger::error) << "Failed to connect. Network error";
                                        addError("Failed to connect to the residue server. Network error. " + errorText);
                                        throw ResidueException(m_errors.at(m_errors.size() - 1));
                                    } else {
                                        if (ackResponse.find("{") != std::string::npos) {
                                            // Error
                                            if (ackResponse.at(0) != '{') {
                                                // remove head
                                                ackResponse.erase(0, ackResponse.find_first_of(":") + 1);
                                            }
                                            try {
                                                j = json::parse(ackResponse);
                                                InternalLogger(InternalLogger::error) << j["error_text"].get<std::string>();
                                                addError(j["error_text"].get<std::string>());
                                                throw ResidueException(m_errors.at(m_errors.size() - 1));
                                            } catch (const std::exception& e) {
                                                InternalLogger(InternalLogger::error)
                                                        << "Error occurred but could not parse response: "
                                                        << ackResponse << " (" << e.what() << ")";
                                            }

                                        } else {
                                            std::string iv;
                                            try {
                                                InternalLogger(InternalLogger::error) << "Reading connection";
                                                std::string decryptedAckResponse = Ripe::decryptAES(ackResponse, m_key, iv, true);
                                                loadConnectionFromJson_(decryptedAckResponse);
                                                onConnect();
                                            } catch (const std::exception& e) {
                                                InternalLogger(InternalLogger::error) << "Failed to connect (4): " << e.what();
                                                throw ResidueException(e.what());
                                            }
                                        }
                                    }
                                });
                            } catch (const ResidueException e) {
                                throw e;
                            }
                        } else {
                           InternalLogger(InternalLogger::error) << "Failed to connect (2): "
                                                                 << j["error_text"].get<std::string>();
                           addError("Failed to connect. " + j["error_text"].get<std::string>());
                           throw ResidueException(m_errors.at(m_errors.size() - 1));
                        }
                    } catch (const std::exception& e) {
                        InternalLogger(InternalLogger::error) << "Failed to connect (2) [parse error]: " << e.what();
                        throw ResidueException(e.what());
                    }

                } else {
                    InternalLogger(InternalLogger::debug) << "Failed to read response!";
                    InternalLogger(InternalLogger::error) << "Failed to connect (2)";
                    if (m_knownClient) {
                        InternalLogger(InternalLogger::error)
                                << "RSA private key provided does not match the public key on the server.";
                        addError("RSA private key provided does not match the public key on the server.");
                        throw ResidueException(m_errors.at(m_errors.size() - 1));
                    }
                }
            }
        });
    } catch (const ResidueException& e) {
        throw e;
    }
}

void Residue::addToQueue(RawRequest&& request) noexcept
{
#ifdef RESIDUE_PROFILING
    c_total++;
    unsigned long queue_dura;
    RESIDUE_PROFILE_START(t_queue);
#endif
    RESIDUE_LOCK_LOG("addToQueue");
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_requests.push_front(std::move(request));

#ifdef RESIDUE_PROFILING
    RESIDUE_PROFILE_END(t_queue, queue_dura);
    InternalLogger(InternalLogger::debug) << queue_dura << "ms taken to add to queue";
    queue_ms += queue_dura;
#endif
}

void Residue::dispatch()
{
#ifdef RESIDUE_PROFILING
    int c = 0;
    int c_in_while = 0;
    unsigned long while_loop;
    unsigned long for_loop;
    unsigned long prepare_data;
    unsigned long send_recv;
    unsigned long total_duration_dispatch;
#endif

    RESIDUE_PROFILE_START(t_dispatch);
    while (!m_requests.empty()) {

        while (m_connecting) {
            InternalLogger(InternalLogger::debug) << "Still connecting...";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        try {
            if (!s_loggingClient->connected()) {
                InternalLogger(InternalLogger::info) << "Trying to connect...";
                m_connected = false;
                reconnect();
            }
        } catch (const ResidueException& e) {
            InternalLogger(InternalLogger::error) << "Failed to connect: " << e.what() << ". Retrying in 500ms";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        RESIDUE_PROFILE_START(t_while);
        std::string jsonData;

        if (m_bulkDispatch) {
            jsonData.append("[");
        }
        std::size_t totalRequests = static_cast<std::size_t>(m_bulkDispatch ? m_bulkSize : 1);
        totalRequests = std::min(totalRequests, m_requests.size());

        // Health check once per bulk
        healthCheck();

        RESIDUE_PROFILE_CHECKPOINT(t_dispatch, total_duration_dispatch, 1);

        RESIDUE_PROFILE_START(t_for);
        for (std::size_t i = 0; i < totalRequests; ++i) {

            RawRequest request;

            {
                RESIDUE_LOCK_LOG("dispatch");
                std::lock_guard<std::recursive_mutex> lock(m_mutex);
                request = m_requests.back();
                m_requests.pop_back();
            }
            jsonData.append(requestToJson(std::move(request)));
            if (i < totalRequests - 1) {
                jsonData.append(",");
            }
            RESIDUE_PROFILE_CHECKPOINT(t_dispatch, total_duration_dispatch, 2);

#ifdef RESIDUE_PROFILING
            c_dispatched++;
            c++;
#endif
        }

        RESIDUE_PROFILE_END(t_for, for_loop);

        if (m_bulkDispatch) {
            jsonData.append("]");
        }
        RESIDUE_PROFILE_CHECKPOINT(t_dispatch, total_duration_dispatch, 3);

        RESIDUE_PROFILE_START(t_prepare_data);

        if (hasFlag(Flag::COMPRESSION)) {
            try {
                jsonData = Ripe::base64Encode(Ripe::compressString(jsonData));
            } catch (const std::exception& e) {
                InternalLogger(InternalLogger::error) << "Failed to compress the data: " << e.what();
            }
        }

        std::string data = Ripe::prepareData(jsonData.c_str(), m_key, m_clientId.c_str());
        RESIDUE_PROFILE_END(t_prepare_data, prepare_data);

        RESIDUE_PROFILE_START(t_send);

        // we keep it to true as we may end program before it's actually dispatched
        s_loggingClient->send(std::move(data), true);

        RESIDUE_PROFILE_END(t_send, send_recv);

#ifdef RESIDUE_PROFILING
        RESIDUE_PROFILE_END(t_while, while_loop);
        c_in_while++;
        RESIDUE_PROFILE_CHECKPOINT(t_dispatch, total_duration_dispatch, 4);
        InternalLogger(InternalLogger::debug) << "While: " << while_loop << " ms For: "
                                              << for_loop << " ms Prepare: " << prepare_data
                                              << " ms send_recv " << send_recv << "ms \n";
#endif
    }

#ifdef RESIDUE_PROFILING
    RESIDUE_PROFILE_END(t_dispatch, total_duration_dispatch);
    InternalLogger(InternalLogger::debug) << total_duration_dispatch
                                          << " ms taken for dispatching " << c << " items with "
                                          << c_in_while << " each loop\n";

    dispatch_ms += total_duration_dispatch;
#endif
}

std::string Residue::requestToJson(RawRequest&& request)
{
    json j;
    if (!m_applicationId.empty()) {
        j["app"] = m_applicationId;
    }

    j["_t"] = getTimestamp();
    j["thread"] = request.threadId;
    j["datetime"] = request.date;
    j["logger"] = request.loggerId;
    j["msg"] = request.msg;
    if (!request.file.empty()) {
        j["file"] = request.file;
    }
    if (request.line > 0L) {
        j["line"] = request.line;
    }
    if (!request.func.empty()) {
        j["func"] = request.func;
    }
    j["level"] = request.level;

    if (request.vlevel > 0) {
        j["vlevel"] = request.vlevel;
    }
    return j.dump();
}

bool Residue::shouldTouch() const noexcept
{
    if (m_age == 0) {
        // Always alive so don't need to touch the client
        return false;
    }
    unsigned long now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
    return m_age - (now - m_dateCreated) < Residue::TOUCH_THRESHOLD;
}

void Residue::touch() noexcept
{
    InternalLogger(InternalLogger::debug) << "Touching...";
    m_connecting = true;
    json j;
    int type = 3; // 3 = TOUCH
    j["type"] = type;
    j["_t"] = getTimestamp();
    j["client_id"] = m_clientId;
    std::string jsonRequest = j.dump();
    std::string request;
    try {
        request = Ripe::prepareData(jsonRequest.c_str(), m_key, m_clientId.c_str());
    } catch (const std::exception& e) {
        InternalLogger(InternalLogger::error) << "Failed to prepare request (TOUCH): " << e.what();
    }

    s_connectionClient->send(std::move(request), true, [&](std::string&& response, bool hasError,
                             std::string&& errorText) -> void {
        m_connecting = false;
        if (hasError) {
            InternalLogger(InternalLogger::error) << "Failed to touch. Network error. " + errorText;
        } else {
            if (response.find("{") != std::string::npos) {
                // Error
                if (response.at(0) != '{') {
                    // remove head
                    response.erase(0, response.find_first_of(":") + 1);
                }
                try {
                    j = json::parse(response);
                    InternalLogger(InternalLogger::error) << j["error_text"].get<std::string>();
                    addError(j["error_text"].get<std::string>());
                } catch (const std::exception& e) {
                    InternalLogger(InternalLogger::error) << "Error occurred but could not parse response: "
                                                          << response << " (" << e.what() << ")";
                }

            } else {
                std::string iv;
                try {
                    std::string decryptedResponse = Ripe::decryptAES(response, m_key, iv, true);
                    j = json::parse(decryptedResponse);
                    RESIDUE_LOCK_LOG("sendTouch");
                    std::lock_guard<std::recursive_mutex> lock(m_mutex);
                    m_age = j["age"].get<int>();
                    m_dateCreated = j["date_created"].get<unsigned long>();
                } catch (const std::exception& e) {
                    InternalLogger(InternalLogger::error) << "Failed to read TOUCH response: " << e.what();
                }
            }
        }
    });
}

void Residue::disconnect_() noexcept
{
    InternalLogger(InternalLogger::debug) << "Disconnecting...";
    m_connected = false;
    el::Helpers::uninstallLogDispatchCallback<ResidueDispatcher>("ResidueDispatcher");
    //el::Helpers::installLogDispatchCallback<el::base::DefaultLogDispatchCallback>("DefaultLogDispatchCallback");
}

void Residue::disconnect() noexcept
{
    if (Residue::connected()) {
        Residue::instance().m_connected = false;
        Residue::instance().m_disconnected = true;
        Residue::wait();
        Residue::instance().m_running = false;
        InternalLogger(InternalLogger::debug) << "Wait finished!";
    }
}

bool Residue::isClientValid() const noexcept
{
    if (m_age == 0) {
        return true;
    }
    unsigned long now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
    return now - m_dateCreated < m_age;
}

void Residue::loadConfiguration(const std::string& jsonFilename)
{
    std::ifstream fs(jsonFilename.c_str(), std::ios::in);
    if (!fs.is_open()) {
        throw ResidueException("File [" + jsonFilename + "] is not readable");
    }
    std::string confJson = std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
    fs.close();
    loadConfigurationFromJson(confJson);
}

void Residue::loadConfigurationFromJson_(const std::string& confJson)
{
    Residue* r = &Residue::instance();
    json j;
    try {
        j = json::parse(confJson);
    } catch (const std::exception& e) {
        throw ResidueException("Unable to parse configuration file: " + std::string(e.what()));
    }
    if (j.count("url") == 0) {
        throw ResidueException("URL not specified in configuration");
    }
    std::string url = j["url"].get<std::string>();
    std::size_t pos = url.find_first_of(':');
    if (pos == std::string::npos) {
        throw ResidueException("URL should be in format of <host>:<port>");
    }
    r->m_host = url.substr(0, pos);
    std::string port = url.substr(pos + 1);
    r->m_port = atoi(port.c_str());
    if (r->m_port == 0) {
        throw ResidueException("Invalid port");
    }
    if (j.count("client_id") > 0 && j.count("client_private_key") > 0) {
        std::string privateKeyFile = j["client_private_key"].get<std::string>();
        if (j.count("client_key_secret") > 0) {
            std::string secretKey = j["client_key_secret"].get<std::string>();
            if (!secretKey.empty()) {
                r->m_rsaPrivateKeySecret = Ripe::hexToString(secretKey);
            }
        }
        resolveResidueHomeEnvVar(privateKeyFile);
        std::ifstream fsPriv(privateKeyFile.c_str(), std::ios::in);
        if (fsPriv.is_open()) {
            setKnownClient(j["client_id"].get<std::string>(),
                    std::string(std::istreambuf_iterator<char>(fsPriv), std::istreambuf_iterator<char>()));
            fsPriv.close();
        } else {
            throw ResidueException("File [" + privateKeyFile + "] not readable");
        }
    }
    if (j.count("server_public_key") > 0) {
        std::string publicKeyFile = j["server_public_key"].get<std::string>();
        resolveResidueHomeEnvVar(publicKeyFile);
        std::ifstream fsPub(publicKeyFile.c_str(), std::ios::in);
        if (fsPub.is_open()) {
            r->m_serverPublicKey = std::string(std::istreambuf_iterator<char>(fsPub),
                                               std::istreambuf_iterator<char>());
            fsPub.close();
        } else {
            throw ResidueException("File [" + publicKeyFile + "] not readable");
        }
    }
    if (j.count("application_id") > 0) {
        r->m_applicationId = j["application_id"].get<std::string>();
    }
    if (j.count("utc_time") > 0) {
        r->m_utc = j["utc_time"].get<bool>();
    }
    if (j.count("time_offset") > 0) {
        r->m_timeOffset = j["time_offset"].get<int>();
    }
    if (j.count("rsa_key_size") > 0) {
        setKeySize(j["rsa_key_size"].get<std::size_t>());
    }
    if (j.count("dispatch_delay") > 0) {
        r->m_dispatchDelay = j["dispatch_delay"].get<int>();
    }
    if (j.count("main_thread_id") > 0) {
        setThreadName(j["main_thread_id"].get<std::string>());
    }
    if (j.count("internal_logging_level") > 0) {
        Residue::s_internalLoggingLevel = j["internal_logging_level"].get<int>();
    }
}

void Residue::saveConnection_(const std::string& outputFile)
{
    if (!m_connected) {
        throw ResidueException("Not connected yet");
        return;
    }
    if (m_connection.empty()) {
        return;
    }
    std::ofstream fs(outputFile.c_str(), std::ios::out);
    if (!fs.is_open()) {
        throw ResidueException("File [" + outputFile + "] is not writable");
    }
    fs.write(m_connection.c_str(), m_connection.size());
    if (fs.fail()) {
        throw ResidueException("File [" + outputFile + "] is not writable");
    }
    fs.close();
}

void Residue::loadConnection_(const std::string& connectionFile)
{
    std::ifstream fs(connectionFile.c_str(), std::ios::in);
    if (!fs.is_open()) {
        throw ResidueException("File [" + connectionFile + "] is not readable");
    }
    std::string conn = std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
    fs.close();
    loadConnectionFromJson(conn);
}

void Residue::loadConnectionFromJson(const std::string& json)
{
    if (Residue::instance().m_host.empty()) {
        throw ResidueException("Failed to connect (load connection). No host found.");
    }
    Residue::instance().connect_(Residue::instance().m_host, Residue::instance().m_port, false);
    Residue::instance().loadConnectionFromJson_(json);
    Residue::instance().healthCheck();
    Residue::instance().onConnect();
}

void Residue::loadConnectionFromJson_(const std::string& connectionJson)
{
    try {
        json j;
        j = json::parse(connectionJson);
        m_key = j["key"].get<std::string>();
        m_age = j["age"].get<unsigned int>();
        m_dateCreated = j["date_created"].get<unsigned long>();
        m_loggingPort = j["logging_port"].get<int>();
        m_maxBulkSize = j["max_bulk_size"].get<unsigned int>();
        m_serverFlags = j["flags"].get<unsigned int>();
        m_serverVersion = j["server_info"]["version"].get<std::string>();

        // Logging server
        InternalLogger(InternalLogger::debug) << "Logging server socket init";
        s_loggingClient = std::unique_ptr<NetworkClient>(new NetworkClient(m_host, std::to_string(m_loggingPort)));
        try {
            s_loggingClient->connect();
            if (!s_loggingClient->connected()) {
                addError("Failed to connect. (Port: " + std::to_string(m_loggingPort) + ") " + s_loggingClient->lastError());
                throw ResidueException(m_errors.at(m_errors.size() - 1));
            }
        } catch (const ResidueException& e) {
            addError("Failed to connect. (Port: " + std::to_string(m_loggingPort) + ") " + e.what());
            throw e;
        }

        if (m_autoBulkParams && hasFlag(Flag::ALLOW_BULK_LOG_REQUEST)) {
            m_bulkDispatch = true;
            m_bulkSize = std::min(m_maxBulkSize, 40U);
        }

        if (hasFlag(Flag::ALLOW_BULK_LOG_REQUEST) && m_bulkDispatch && m_bulkSize > m_maxBulkSize) {
            // bulk dispatch is manually on
            InternalLogger(InternalLogger::info) << "Resetting bulk size to " << m_maxBulkSize;
            m_bulkSize = m_maxBulkSize;
        } else if (!hasFlag(Flag::ALLOW_BULK_LOG_REQUEST) && m_bulkDispatch) {
            InternalLogger(InternalLogger::info) << "Bulk log requests not allowed by this server";
            m_bulkDispatch = false;
        }

        m_connected = true;
        m_connection = connectionJson;
    } catch (const std::exception& e) {
        InternalLogger(InternalLogger::error) << "Failed to connect (load connection): " << e.what();
        throw ResidueException(e.what());
    }
}

std::string& Residue::resolveResidueHomeEnvVar(std::string& str)
{
    auto pos = str.find("$RESIDUE_HOME");
    if (pos != std::string::npos) {
        std::string val = m_homepath.empty() ?
                    el::base::utils::OS::getEnvironmentVariable("RESIDUE_HOME", "", "echo $RESIDUE_HOME") :
                    m_homepath;
        if (val.empty()) {
            InternalLogger(InternalLogger::error) << "Environment variable RESIDUE_HOME not set";
        }
        str.replace(pos, std::string("$RESIDUE_HOME").size(), val);
    }
    return str;
}

void Residue::addError(const std::string& errorText) noexcept
{
    if (m_errors.size() > 10) {
        m_errors.erase(m_errors.begin(), m_errors.begin() + m_errors.size() - 10);
    }
    m_errors.push_back(errorText);
}

std::string Residue::version() noexcept
{
    return RESIDUE_SOVERSION;
}

std::string Residue::info() noexcept
{
    std::stringstream ss;
    ss << "Residue C++ client library v" << version()
       << " (based on Easylogging++ v" << el::VersionInfo::version()
       << ")";
    return ss.str();
}

void Residue::setBulkSize(unsigned int bulkSize)
{
    if (connected() && bulkSize > maxBulkSize()) {
        throw ResidueException("Invalid bulk dispatch size. Maximum allowed: " + std::to_string(maxBulkSize()));
    }
    Residue::instance().m_bulkSize = bulkSize;
}

void Residue::setKeySize(std::size_t keySize)
{
    if (keySize != 2048 && keySize != 2048 * 2 && keySize != 2048 * 4) {
        throw ResidueException("Invalid key size. Please select 2048, 4096 or 8192");
    }
    Residue::instance().m_keySize = keySize;
}

void Residue::setKnownClient(const std::string& clientId, const std::string& privateKeyPem)
{
    Residue::instance().m_clientId = clientId;
    Residue::instance().m_rsaPrivateKey = privateKeyPem;
    if (!clientId.empty() && privateKeyPem.empty()) {
        throw ResidueException("Please provide private key for known client.");
    }
    Residue::instance().m_knownClient = !clientId.empty() && !privateKeyPem.empty();
}

void Residue::enableBulkDispatch()
{
    if (connected() && !hasFlag(Flag::ALLOW_BULK_LOG_REQUEST)) {
        throw ResidueException("Bulk log requests not allowed by this server");
    }
    Residue::instance().m_bulkDispatch = true;
}
