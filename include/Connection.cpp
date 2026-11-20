//
// Created by Deakin Simpson on 20/11/2026.
//

#include "Connection.hpp"

#include "Socket.hpp"

namespace http_server
{
    Connection::Connection()
    {
    }

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

        /* initialize io for reading */
        io_init(client_info_.buffer_data, client_info_.file_descriptor);
    }

    void Connection::read_line()
    {
        ssize_t num_bytes { io_readline(client_info_.buffer_data,
            client_info_.buffer.data(), MAX_LINE) };

        if (num_bytes < 0)
        {
            throw std::runtime_error("read error");
        }
        if (num_bytes == 0)
        {
            std::cout << "client connection close\n";
            return;
        }

        std::size_t length
            { trim_crlf(client_info_.buffer.data(),
                static_cast<std::size_t>(num_bytes)) };

        if (length == 0)
        {
            std::cout << "end of headers\n";
        }
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

    void Connection::print_buffer() const
    {
        std::cout << client_info_.buffer.data() << std::endl;
    }
}
