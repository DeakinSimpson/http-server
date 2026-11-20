//
// Created by Deakin Simpson on 20/11/2026.
//

#include "Connection.hpp"

#include "Socket.hpp"

namespace http_server
{
    void Connection::accept(const Socket& socket)
    {
        client_info_.length = sizeof(client_info_.address);

        /* accept the connection */
        client_info_.file_descriptor = system_error_handle(
            ::accept, "accept failed",
            socket.get_file_descriptor(),
            reinterpret_cast<sockaddr*>(&client_info_.address),
            &client_info_.length);

        /* get the clients info */
        getnameinfo_safe(reinterpret_cast<sockaddr*>(&client_info_.address),
            client_info_.length, client_info_.hostname, MAX_LINE,
            client_info_.port, MAX_LINE, 0);
    }

    void Connection::close() const
    {
        close_safe(client_info_.file_descriptor);
    }

    void Connection::print_client_socket() const
    {
        std::cout << "Accepted Connection from (" << client_info_.hostname;
        std::cout << ", " << client_info_.port << ")" << '\n';
    }
}
