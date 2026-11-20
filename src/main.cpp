#include <iostream>

#include "WebServer.hpp"
#include "../include/Socket.hpp"

int main()
{
    http_server::WebServer web_server { "80" };

    web_server.start();

    return 0;
}
