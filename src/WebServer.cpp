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
                else /* handle error */
                {
                    bad_request();
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


    static std::expected<http::Message, HttpError> build_get(
        std::unique_ptr<http::Message> request)
    {
        /* create response start_line */
        http::Message response {};
        response.start_line = http::StatusLine{
            "HTTP/1.1", http::StatusCode::Ok, "OK"
        };

        /* get the filepath */
        const std::filesystem::path filepath {
            std::get<http::RequestLine>(request->start_line).target
        };

        /* check that the filepath exists */
        if (!std::filesystem::exists(filepath))
        {
            return http_error(http::StatusCode::Not_Found,
                std::format("{} not found", filepath.string()));
        }

        return http_error(http::StatusCode::Not_Implemented, "TESTING");
    }

    static char* build_response(const http::Message& response)
    {
        char result[]
        {
            "<!DOCTYPE html>\r\n"
            "<html>\r\n"
            "<head><title>Test</title></head>\r\n"
            "<body><h1>Hello World!</h1></body>\r\n"
            "</html>\r\n"
        };
        return result;
    }

    /**
     * this function will take in a request then create a response or drop it
     * based on its contents.
     *
     * @param request the request to be processed
     */
    void WebServer::process_request(std::unique_ptr<http::Message> request)
    {
        /* get the method and build response based on method */
        const http::Method method {
            std::get<http::RequestLine>(request->start_line).method
        };

        /* get the response */
        std::expected<http::Message, HttpError> response;
        switch (method)
        {
        case http::Method::GET:
            response = build_get(std::move(request));
            break;
        default:
            not_implemented();
            return;
        }

        /* if response successfully built write and exit */
        if (response.has_value())
        {
            connection_.write(build_response(*response), sizeof(*response) - 1);
            return;
        }

        /* TODO: Update to handle more then just bad_request */
        switch (response.error().status)
        {
        default:
            bad_request();
        }
    }

    void WebServer::bad_request() const
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

    void WebServer::not_implemented() const
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
