//
// Created by Deakin Simpson on 20/11/2026.
//

#include "../include/Connection.hpp"

#include "../include/Socket.hpp"

namespace http_server
{
    Connection::Connection()
    {
    }

    /**
     * accepts incoming connections from the supplied socket and stores the file
     * descriptor. performs getnameinfo_safe to get the host and the port.
     * prepares the io (socket) for reading via io_init.
     *
     * @param socket the socket that will accept the connection
     */
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

    /**
     * reads line sockets buffer and stores it in client_info_.buffer.
     *
     * if there is an error reading the sockets buffer, throw std::runtime_error
     *
     * if the line is the EOF, return -1, as this is std::size_t this will cause
     * underflow, but as the buffers max size is 32kb, we can safely assume that
     * the numeric limits will never be reached.
     *
     * then the function will trim the crlf \r\n from the end of the line
     *
     * @return the number of bytes in the line after trimming \r\n
     */
    std::optional<std::size_t> Connection::read_line()
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
            return std::nullopt; /* use -1 to define end of file */
        }

        const std::size_t length
            { trim_crlf(client_info_.buffer.data(),
                static_cast<std::size_t>(num_bytes)) };

        return length;
    }

    /**
     * closes the Connections file descriptor
     */
    void Connection::close() const
    {
        close_safe(client_info_.file_descriptor);
    }

    /**
     * prints the clients socket data, this is retrieved when connection is
     * accepted in Connection::accept.
     */
    void Connection::print_client_socket() const
    {
        std::cout << "Accepted Connection from (" << client_info_.hostname;
        std::cout << ", " << client_info_.port << ")" << '\n';
    }

    /**
     * prints the Connections buffer (this is not the buffer of the socket)
     */
    void Connection::print_buffer() const
    {
        std::cout << client_info_.buffer.data() << std::endl;
    }

    /* TODO: Add bool option for remove whitespaces, separate from delimiter */

    /**
     * this function reads buffer, then splits the string_view into sub
     * string_view's that are deliminated by delimiter and returns them as a
     * vector.
     *
     * @param buffer the buffer the elements will be read from
     * @param delimiter what char will be used to split the elements
     * @param bytes number of bytes until end of line
     * @param no_white_space how many word from 0 index that CANNOT have white spaces
     * @return a vector of elements retrieved from the line
     */
    static std::vector<std::string_view> get_buffer_elements(
        std::string_view buffer, const std::size_t bytes)
    {
        std::size_t i { 0 };
        std::vector<std::string_view> result;

        while (i < bytes)
        {
            /* skip the leading white spaces */
            while (i < bytes && std::isspace(buffer[i]))
            {
                ++i;
            }

            /* cache start of the string_view */
            const auto start { i };

            /* get the index up until whitespace */
            while (i < bytes && !std::isspace(buffer[i]))
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

    /**
     * Converts a string_view of the Method, from the buffer, and converts it
     * to the corresponding enum.
     *
     * @param method_sv string_view of the input method
     * @return http::Method associated with method_sv
     */
    static constexpr std::optional<http::Method> parse_method(
        const std::string_view method_sv)
    {
        if (method_sv == "GET")     return http::Method::GET;
        if (method_sv == "HEAD")    return http::Method::HEAD;
        if (method_sv == "POST")    return http::Method::POST;
        if (method_sv == "PUT")     return http::Method::PUT;
        if (method_sv == "DELETE")  return http::Method::DELETE;
        if (method_sv == "CONNECT") return http::Method::CONNECT;
        if (method_sv == "OPTIONS") return http::Method::OPTIONS;
        if (method_sv == "TRACE")   return http::Method::TRACE;

        return std::nullopt;
    }

    /**
     * reads line from the socket, then converts the line into a RequestLine
     *
     * @return request line, if the request_line read was valid
     */
    std::optional<http::RequestLine> Connection::get_request_line()
    {
        /* read the request line into the buffer */
        auto line_bytes { read_line() };

        /* use a string_view to represent the buffer */
        std::string_view buffer_sv(client_info_.buffer.data(),
            client_info_.buffer.size());

        const auto elements { get_buffer_elements(buffer_sv, *line_bytes) };

        /* if request line has more then 3 elements, drop */
        if (elements.size() != 3)
        {
            return std::nullopt;
        }

        /* get the method from the first element */
        const auto method { parse_method(elements[0]) };

        /* was the method given a valid method? */
        if (!method.has_value())
        {
            return std::nullopt;
        }

        /* return the http::RequestLine */
        return http::RequestLine {
            method.value(),
            std::string(elements[1]),
            std::string(elements[2])
        };
    }

    /**
     * returns the fields from the http request. If there are any errors,
     * return std::nullopt
     *
     * @return optionally a vector of field-name : field-value pairs
     */
    std::optional<std::vector<std::pair<std::string, std::string>>>
    Connection::get_field_lines()
    {
        std::vector<std::pair<std::string, std::string>> result;

        while (true)
        {
            const auto line_bytes { read_line() };

            if (!line_bytes.has_value())
            {
                return std::nullopt;
            }

            if (*line_bytes == 0) break; /* end of headers */

            std::string_view line(client_info_.buffer.data(), *line_bytes);

            const auto colon { line.find(':') };

            /* if unable to find colon, or no field-name */
            if (colon == std::string_view::npos || colon == 0)
            {
                return std::nullopt;
            }

            /* get the field-name (everything before colon) */
            std::string_view field_name { line.substr(0, colon) };

            /* if field_name contains whitespaces, error */
            if (field_name.find_first_of(" \t") != std::string_view::npos)
            {
                return std::nullopt;
            }

            /* get value after the name */
            std::string_view value { line.substr(colon + 1) };

            /* find first character that is not whitespace */
            const auto first { value.find_first_not_of(" \t") };

            /* find the last character that is not a whitespace */
            const auto last  { value.find_last_not_of(" \t") };

            /* if the first value is end of string, return empty */
            value = (first == std::string_view::npos) ? std::string_view{}
                : value.substr(first, last - first + 1);

            /* emplace the pair into the vector */
            result.emplace_back(std::string(field_name), std::string(value));
        }

        return result;
    }

    /**
     * if the message has an error then the the pointer will point to a
     * std::nullopt
     *
     * @return a pointer to an optional message
     */
    std::unique_ptr<OptMsg> Connection::get_request()
    {
        const auto request_line { get_request_line() };

        /* if bad request line, return bad message */
        if (!request_line.has_value())
        {
            return std::make_unique<OptMsg>(std::nullopt);
        }

        std::cout << "target: " << request_line->target << ", version: "
            << request_line->version << std::endl;

        const auto fields { get_field_lines() };

        if (!fields.has_value())
        {
            return std::make_unique<OptMsg>(std::nullopt);
        }

        http::Message message;
        message.start_line = std::move(*request_line);
        message.field_lines = std::move(*fields);

        /* TODO: add body reading */

        return std::make_unique<OptMsg>(std::move(message));
    }

    void Connection::write(char* buffer, const std::size_t buffer_size) const
    {
        io_write(client_info_.file_descriptor, buffer, buffer_size);
    }
}
