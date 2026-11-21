//
// Created by Deakin Simpson on 20/11/2026.
//

#include "../include/WebServer.hpp"

#include "HTTP.hpp"

namespace http_server
{
    void WebServer::start()
    {
        std::cout << "web server starting..." << std::endl;
        while (1)
        {
            /* accept the connection from the client */
            connection_.accept(socket_);

            connection_.print_client_socket();

            process_request(connection_.get_request());

            // connection_.read_line();
            // connection_.print_buffer();

            /* close connection with client immediatly */
            connection_.close();
        }
    }

    void WebServer::process_request(std::unique_ptr<http::Message> request)
    {

    }
}
