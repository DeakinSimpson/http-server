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
            try
            {
                connection_.accept(socket_);
            }
            catch (const std::exception& e)
            {
                std::cerr << "accept error: " << e.what() << '\n';
                continue;
            }

            try
            {
                connection_.print_client_socket();
                auto request { connection_.get_request() };

                if (request.has_value())
                {
                    process_request(std::move(request.value()));
                }

                /* handle error */
                else
                {
                    /* although this is the same, this will be updated later */
                    switch (request.error())
                    {
                    case Error::BadRequest:
                        bad_request(std::move(*request));
                    case Error::ParseError:
                        bad_request(std::move(*request));
                    default:
                        bad_request(std::move(*request));
                    }
                }

            }
            catch (const std::exception& e)
            {
                std::cerr << "request error: " << e.what() << '\n';
            }

            try
            {
                connection_.close();
            }
            catch (const std::exception& e)
            {
                std::cerr << "close error: " << e.what() << '\n';
            }
        }
    }

    /**
     * this function will take in a request then create a response or drop it
     * based on its contents.
     *
     * @param request the request to be processed
     */
    void WebServer::process_request(std::unique_ptr<http::Message> request)
    {
        /* TODO: process actualy requests */
        const http::Method method {
            std::get<http::RequestLine>(request->start_line).method
        };

        if (method == http::Method::GET)
        {
            std::cout << "GET processing starting..." << std::endl;

            /* TODO: implement html disk file serving */
            char text[] {
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Content-Length: 102\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<!DOCTYPE html>\r\n"
                "<html>\r\n"
                "<head><title>Test</title></head>\r\n"
                "<body><h1>Hello World!</h1></body>\r\n"
                "</html>"
            };

            connection_.write(text, sizeof(text) - 1);

            std::cout << "Serving html response to: ";
            connection_.print_client_socket();
        }
        else
        {
            not_implemented(std::move(request));
        }
    }

    void WebServer::bad_request(std::unique_ptr<http::Message> request) const
    {
        char text[] {
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "Content-Length: 88\r\n"
            "\r\n"
            "{\r\n"
            "  \"error\": \"Bad request\",\r\n"
            "  \"message\": \"Request body could not be read properly.\"\r\n"
            "}"
        };

        connection_.write(text, sizeof(text) - 1);
    }

    void WebServer::not_implemented(std::unique_ptr<http::Message> request) const
    {
        char text[] {
            "HTTP/1.1 501 Not Implemented\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n"
        };

        connection_.write(text, sizeof(text) - 1);
    }
}
