//
//  network-client.h
//  Residue C++
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present Zuhd Web Services
//  Copyright (C) 2017-present @abumusamq
//
//  https://muflihun.com/
//  https://zuhd.org
//  https://github.com/zuhd-org/residue-cpp/
//
//  See https://github.com/zuhd-org/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#ifndef RESIDUE_NETWORK_CLIENT_H
#define RESIDUE_NETWORK_CLIENT_H

#include <string>
#include "asio.hpp"

///
/// \brief Residue client connector class.
/// This uses ASIO for networking and Ripe for encryption
///
class NetworkClient
{
public:
    ///
    /// Response handle from server. std::string = responseBody, bool = hasError, , std::string&& = errorText
    ///
    using ResponseHandler = std::function<void(std::string&&, bool, std::string&&)>;

    ///
    /// \brief NetworkClient constructor
    /// \param host Residue server host
    /// \param port Residue server connect_port (rvalue)
    ///
    NetworkClient(const std::string& host, std::string&& port) noexcept;

    virtual ~NetworkClient() noexcept = default;

    ///
    /// \brief Sends payload to the server
    /// \param payload Payload to be sent
    /// \param expectResponse If true, it waits for response from the server. Use this
    /// only when we know the application is going to be alive by the time async_wait
    /// can finish. The only time we can be potentially sure about this situation is when
    /// running this in a gui application
    /// \param retryOnTimeout Retry by connect()ing and the sending again
    /// \param resHandler When response received this callback is called
    /// \see ResponseHandler
    ///
    void send(std::string&& payload, bool expectResponse = false,
              const ResponseHandler& resHandler = [](std::string&&, bool, std::string&&) -> void {}, bool retryOnTimeout = true, bool ignoreQueue = false);

    ///
    /// \brief Whether connected to the server (using this host and port) or not
    /// \return True if connected, otherwise false
    ///
    inline bool connected() const noexcept { return m_socket.is_open() && m_connected; }

    ///
    /// \brief Returns last error recorded
    ///
    inline const std::string& lastError() const noexcept { return m_lastError; }

    ///
    /// \brief Connects to the server using initial host and port
    ///
    void connect();
private:
    std::string m_host;
    std::string m_port;
    asio::io_service m_ioService;
    asio::ip::tcp::socket m_socket;
    bool m_connected;
    std::string m_lastError;

    ///
    /// \brief Reads incomming bytes from the server
    /// \param resHandler When response is read, this callback is called
    /// \see ResponseHandler
    ///
    void read(const ResponseHandler& resHandler = [](std::string&&, bool, std::string&&) -> void {}, const std::string& payload = "");
};

#endif /* RESIDUE_NETWORK_CLIENT_H */
