//
//  network-client.cc
//  Residue C++
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present @abumq (Majid Q.)
//
//  See https://github.com/abumq/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#include <system_error>
#include "ripe/Ripe.h"
#include "include/residue.h"
#include "network-client.h"
#include "internal-logger.h"

using asio::ip::tcp;

NetworkClient::NetworkClient(const std::string& host, std::string&& port) noexcept :
    m_host(host),
    m_port(std::move(port)),
    m_socket(m_ioService),
    m_connected(false)
{
}

void NetworkClient::connect()
{
    tcp::resolver resolver(m_ioService);
    tcp::resolver::iterator endpoint = resolver.resolve(tcp::resolver::query(m_host, m_port));
    // Do not use async_connect as we want client application to wait
    try {
        asio::connect(m_socket, endpoint);
        m_connected = true;
    } catch (const std::exception& e) {
        m_lastError = "Failed to connect: " + std::string(e.what());
        InternalLogger(InternalLogger::error) << m_lastError;
        throw ResidueException(e.what());
    }
}

void NetworkClient::send(std::string&& payload, bool expectResponse, const ResponseHandler& resHandler, bool retryOnTimeout, bool ignoreQueue)
{
    InternalLogger(InternalLogger::crazy) << "Send: " << payload;
    if (!m_socket.is_open()) {
        InternalLogger(InternalLogger::error) << "Socket closed [" << m_port << "]";
        return;
    }
    if (!m_connected) {
        InternalLogger(InternalLogger::error) << "Client not connected [" << m_port << "]";
        return;
    }
    InternalLogger(InternalLogger::crazy) << "Writing [" << m_port << "]..." << (expectResponse ? "" : " (async)");
    if (expectResponse) {
        try {
            asio::write(m_socket, asio::buffer(payload, payload.length()));
            InternalLogger(InternalLogger::crazy) << "Waiting for response...";
            read(resHandler, payload);
            InternalLogger(InternalLogger::crazy) << "Done";
        } catch (const std::exception& e) {
            m_lastError = "Failed to send: " + std::string(e.what());
            InternalLogger(InternalLogger::error) << m_lastError;
            if (retryOnTimeout && m_lastError.find("Operation timed out") != std::string::npos) {
                m_socket.close();
                m_connected = false;
                connect();
                send(std::move(payload), expectResponse, resHandler, false);
            } else {
                throw ResidueException(e.what());
            }
        }
    } else {
        asio::async_write(m_socket, asio::buffer(payload, payload.length()),
                                 [&](const std::error_code&, std::size_t) {
        });
    }
}

void NetworkClient::read(const ResponseHandler& resHandler, const std::string& payload)
{
    std::error_code ec;
    asio::streambuf buf;
    try {
        std::size_t numOfBytes = asio::read_until(m_socket, buf, Ripe::PACKET_DELIMITER, ec);
        if (!ec) {
            std::istream is(&buf);
            std::string buffer((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
            buffer.erase(numOfBytes - Ripe::PACKET_DELIMITER_SIZE);
            resHandler(std::move(buffer), false, "");
        } else {
            if ((ec == asio::error::eof) || (ec == asio::error::connection_reset)) {
                InternalLogger(InternalLogger::error) << "Failed to send, requeueing...";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                m_connected = false;
            } else {
                InternalLogger(InternalLogger::error) << ec.message();
                resHandler("", true, ec.message());
            }
        }
    } catch (const std::exception& e) {
        InternalLogger(InternalLogger::error) << e.what();
        resHandler("", true, std::string(e.what()));
    }
}
