//
// Created by Deakin Simpson on 19/11/2026.
//

#pragma once
#include "SocketUtils.hpp"

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

        [[nodiscard]] int get_file_descriptor() const
            { return file_descriptor_; }

    private:
        void check_fd_succeeded() const;
    };
}