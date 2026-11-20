//
// Created by Deakin Simpson on 20/11/2026.
//

#include "../include/WebServer.hpp"

namespace http_server
{
    static constexpr int MAXLINE { 8192 }; /* 8kb */

    void WebServer::start()
    {
        std::cout << "web server starting..." << std::endl;

        /* create variables for client info */
        socklen_t client_length {};
        sockaddr_storage client_address {};
        char client_hostname[MAXLINE];
        char port[MAXLINE];

        while (1)
        {
            /* accept the connection from the client */
            client_length = sizeof(client_address);
            connection_fd_ = system_error_handle(accept, "accept failed",
                socket_.get_file_descriptor(),
                reinterpret_cast<sockaddr*>(&client_address),
                &client_length);

            /* get the client connections information */
            getnameinfo_safe(reinterpret_cast<sockaddr*>(&client_address),
                client_length, client_hostname, MAXLINE, port, MAXLINE, 0);

            /* print the client connections informtion */
            std::cout << "Accepted Connection from (" << client_hostname;
            std::cout << ", " << port << ")" << '\n';


            /* close connection with client immediatly */
            close_safe(connection_fd_);
        }
    }


}
