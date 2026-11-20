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

    http::RequestLine Connection::get_request_line()
    {
        /* read the request line into the buffer */
        read_line();

        /* use a string_view to represent the buffer */
        std::string_view buffer_sv(client_info_.buffer.data(),
            client_info_.buffer.size());

        std::size_t i { 0 };
        while (i < buffer_sv.size())
        {
            /* skip the leading white spaces */
            while (i < buffer_sv.size() && std::isspace(buffer_sv[i]))
            {
                ++i;
            }

            /* get the start of the string view */
            std::size_t start { i };

            /* move until whitespace */
            while (i < buffer_sv.size() && !std::isspace(buffer_sv[i]))
            {
                ++i;
            }

            std::cout << "method: " << buffer_sv.substr(start, i - start) << '\n';
            break;
        }

        return {};
    }

    std::unique_ptr<http::Message> Connection::get_request()
    {
        http::RequestLine request_line { get_request_line() };

        return std::make_unique<http::Message>(http::Message());
    }
}
