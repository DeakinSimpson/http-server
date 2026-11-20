//
// Created by Deakin Simpson on 20/11/2026.
//

#pragma once
#include "Socket.hpp"

namespace http_server
{
    class WebServer
    {
        Socket socket_;
        int connection_fd_ {};
    public:
        explicit WebServer(const char* port) : socket_{ port }
        {  }

        void start();
    };
}
