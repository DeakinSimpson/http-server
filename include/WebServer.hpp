//
// Created by Deakin Simpson on 20/11/2026.
//

#pragma once
#include "Connection.hpp"
#include "HTTP.hpp"
#include "Socket.hpp"
#include <filesystem>

namespace http_server
{
    class WebServer
    {
        Socket socket_;
        Connection connection_ {};
        std::filesystem::path root_ {};
    public:
        explicit WebServer(const char* port, std::filesystem::path root_path)
            : socket_{ port }, root_ { std::move(root_path) }
        {  }

        void start();
        void process_request(std::unique_ptr<http::Message> request);
    private:
        void bad_request(std::unique_ptr<http::Message> request) const;
        void not_implemented(std::unique_ptr<http::Message> request) const;
    };
}
