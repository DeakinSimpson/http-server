//
// Created by Deakin Simpson on 20/11/2026.
//

#include "../include/WebServer.hpp"

#include "HTTP.hpp"

namespace http_server
{
    /**
     * starts the web servers loop
     */
    void WebServer::start()
    {
        std::cout << "web server starting..." << std::endl;
        while (true)
        {
            /* accept the connection from the client */
            connection_.accept(socket_);

            connection_.print_client_socket();

            process_request(connection_.get_request());

            connection_.close();
        }
    }

    /**
     * this function will take in a request then create a response or drop it
     * based on its contents.
     *
     * @param request the request to be processed
     */
    void WebServer::process_request(std::unique_ptr<OptMsg> request)
    {
        /* malformed request header */
        if (!request->has_value())
        {
            bad_request(std::move(request));
            return;
        }

        const http::Method method {
            std::get<http::RequestLine>(request->value().start_line).method
        };

        if (method == http::Method::GET)
        {
            std::cout << "GET processing starting..." << std::endl;

            bad_request(std::move(request));
        }
        else
        {
            not_implemented(std::move(request));
        }
    }

    void WebServer::bad_request(std::unique_ptr<OptMsg> request)
    {
        char text[] {
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close"
            "Content-Length: 88\r\n"
            "\r\n"
            "{\r\n"
            "  \"error\": \"Bad request\",\r\n"
            "  \"message\": \"Request body could not be read properly.\"\r\n"
            "}"
        };

        connection_.write(text, sizeof(text) - 1);
    }

    void WebServer::not_implemented(std::unique_ptr<OptMsg> request)
    {
        char text[] {
            "HTTP/1.1 501 Not Implemented\r\n"
        };

        connection_.write(text, sizeof(text) - 1);
    }
}
