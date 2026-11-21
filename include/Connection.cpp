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

    std::size_t Connection::read_line()
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
            return -1; /* use -1 to define end of file */
        }

        const std::size_t length
            { trim_crlf(client_info_.buffer.data(),
                static_cast<std::size_t>(num_bytes)) };

        if (length == 0)
        {
            std::cout << "end of headers\n";
        }

        return length;
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

    /* TODO: Add bool option for remove whitespaces, separate from delimiter */
    static std::vector<std::string_view> get_buffer_elements(
        std::string_view buffer, const char delimiter, const std::size_t bytes)
    {
        std::size_t i { 0 };
        std::vector<std::string_view> result;

        while (i < bytes)
        {
            /* skip the leading white spaces */
            while (i < bytes && buffer[i] == delimiter)
            {
                ++i;
            }

            /* cache start of the string_view */
            const auto start { i };

            /* get the index up until whitespace */
            while (i < bytes && buffer[i] != delimiter)
            {
                ++i;
            }

            /* this removes blank being pushed at end */
            if (start < i)
            {
                result.push_back(buffer.substr(start, i - start));
            }
        }

        return result;
    }

    http::RequestLine Connection::get_request_line()
    {
        /* read the request line into the buffer */
        auto line_bytes { read_line() };

        /* use a string_view to represent the buffer */
        std::string_view buffer_sv(client_info_.buffer.data(),
            client_info_.buffer.size());

        const auto elements { get_buffer_elements(buffer_sv, ' ', line_bytes) };

        for (auto e : elements)
        {
            std::cout << e << std::endl;
        }

        return {};
    }

    std::unique_ptr<http::Message> Connection::get_request()
    {
        http::RequestLine request_line { get_request_line() };

        return std::make_unique<http::Message>(http::Message());
    }
}
