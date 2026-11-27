//
// Created by Deakin Simpson on 20/11/2026.
//

#pragma once
#include <sys/socket.h>
#include "HTTP.hpp"
#include "SocketUtils.hpp"
#include "Socket.hpp"
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <expected>

namespace http_server
{
    struct ConnectionInfo
    {
        socklen_t                       length                  {};
        sockaddr_storage                address                 {};
        char                            hostname[MAX_LINE]      {};
        char                            port[MAX_LINE]          {};
        int                             file_descriptor         {};
        std::array<char, BUFFER_SIZE>   buffer                  {};
        IOData                          buffer_data             {};
    };

    class Connection
    {
        ConnectionInfo client_info_;
    public:
        Connection();

        void accept(const Socket& socket);
        void print_client_socket() const;
        void close() const;
        std::expected<std::unique_ptr<http::Message>, RequestError> get_request();
        void write(char* buffer, std::size_t buffer_size) const;

    private:
        std::expected<http::RequestLine, RequestError> get_request_line();
        std::expected<std::size_t, ConnectionError> read_line();
        void print_buffer() const;
        std::expected<std::vector<std::pair<std::string, std::string>>, RequestError>
            get_field_lines();
    };
}