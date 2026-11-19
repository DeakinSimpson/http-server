//
// Created by Deakin Simpson on 19/11/2026.
//

#pragma once
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <netdb.h>
#include <ErrorHandle.hpp>

constexpr int BACKLOG_SIZE { 1024 };

namespace http_server
{
    class Socket
    {
        int file_descriptor_ {};

    public:
        Socket(const char* port) : file_descriptor_{open_listen_fd(port) }
        {
            /* check that the file descriptor is open on a socket */
            check_fd_succeeded();
        }

    private:
        static int open_listen_fd(const char* port);
        void check_fd_succeeded() const;
    };
}