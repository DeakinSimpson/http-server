//
// Created by Deakin Simpson on 20/11/2026.
//

#pragma once
#include "Connection.hpp"
#include "HTTP.hpp"
#include "Socket.hpp"

namespace http_server
{
    class WebServer
    {
        Socket socket_;
        Connection connection_ {};
    public:
        explicit WebServer(const char* port) : socket_{ port }
        {  }

        void start();
        void process_request(std::unique_ptr<OptMsg> request);
    };
}
