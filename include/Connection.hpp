//
// Created by Deakin Simpson on 20/11/2026.
//

#pragma once
#include <sys/socket.h>
#include <sys/_types/_socklen_t.h>
#include "SocketUtils.hpp"
#include "Socket.hpp"

namespace http_server
{
    struct ConnectionInfo
    {
        socklen_t           length                  {};
        sockaddr_storage    address                 {};
        char                hostname[MAX_LINE]      {};
        char                port[MAX_LINE]          {};
        int                 file_descriptor         {};
    };

    class Connection
    {
        ConnectionInfo client_info_;
    public:
        void accept(Socket& socket);
        void print_client_socket();
        void close();
    };
}