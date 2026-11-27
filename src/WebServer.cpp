//
// Created by Deakin Simpson on 20/11/2026.
//

#include "../include/WebServer.hpp"

#include "HTTP.hpp"
#include <format>
#include <fstream>
#include <sstream>

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
                else if (const auto* http = std::get_if<HttpError>(&request.error()))
                {
                    send(error_response(*http));
                }
                else
                {
                    std::cout << "client disconnected\n";
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


    std::expected<http::Message, HttpError> WebServer::build_get(
        std::unique_ptr<http::Message> request)
    {
        /* create response start_line */
        http::Message response {};
        response.start_line = http::StatusLine{
            "HTTP/1.1", http::StatusCode::Ok, "OK"
        };

        /* get the filepath */
        std::filesystem::path target {
            std::get<http::RequestLine>(request->start_line).target
        };

        /* if client is requesting root server index.html */
        if (target == "/")
        {
            target = target / "index.html";
        }

        const auto filepath { root_ / target.relative_path() };

        /* check that the filepath exists */
        if (!std::filesystem::exists(filepath))
        {
            return http_error(http::StatusCode::Not_Found,
                std::format("{} not found", filepath.string()));
        }

        /* read file */
        std::ifstream file{filepath, std::ios::binary};

        /* check that the file is open */
        if (!file.good())
        {
            return http_error(http::StatusCode::Not_Found, "cannot open file");
        }

        auto length { std::filesystem::file_size(filepath) };
        std::vector<std::byte> bytes(length);
        file.read(  reinterpret_cast<char*>(bytes.data()),
                    static_cast<long>(length));

        response.body = std::move(bytes);

        return response;
    }

    std::vector<std::byte> get_contents(const std::string &path)
    {
        std::ifstream source_file { path, std::ios::binary };
        if (!source_file.good())
        {
            std::cerr << "unable to open source fle" << std::endl;
        }

        auto length { std::filesystem::file_size(path) };
        std::vector<std::byte> result(length);
        source_file.read( reinterpret_cast<char*>(result.data()), static_cast<long>(length) );

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
            response = http_error(
                http::StatusCode::Not_Implemented, "method not supported");
        }

        send(response ? *response : error_response(response.error()));
    }

    /**
     * Takes int message and returns the serialized string
     *
     * @param message the message to be converted to a string
     * @return the string version of the message
     */
    std::string serialize(const http::Message& message)
    {
        const auto& line { std::get<http::StatusLine>(message.start_line) };
        auto [code, detail] { http::get_status_code(line.status_code) };

        /* create status line */
        std::string out { std::format("{} {} {}\r\n", line.version, code, detail) };

        /* add field lines */
        for (const auto& [name, value] : message.field_lines)
        {
            out += std::format("{}: {}\r\n", name, value);
        }

        /* add length and connection close */
        out += std::format(
            "Content-Length: {}\r\nConnection: close\r\n\r\n",
            message.body.size());

        /* convert std::vector<std::byte> to char* */
        out.append(reinterpret_cast<const char*>(
            message.body.data()), message.body.size());

        return out;
    }

    /**
     * gets the message and serializes it to a std::string
     *
     * @param message the message to be sent to the client
     */
    void WebServer::send(const http::Message& message)
    {
        auto bytes { serialize(message) };
        connection_.write(bytes.data(), bytes.size());
    }
}
