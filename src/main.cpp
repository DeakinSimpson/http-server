#include <iostream>

#include "WebServer.hpp"
#include "../include/Socket.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Must provide only the filepath for the root of the web server" << std::endl;
        throw std::invalid_argument("Invalid Input Argument");
    }

    http_server::WebServer web_server { "80", argv[1] };

    web_server.start();

    return 0;
}
