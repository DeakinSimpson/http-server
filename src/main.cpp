#include <iostream>

#include "../include/Socket.hpp"

int main()
{
    std::cout << "Hello World!" << std::endl;

    http_server::Socket socket { "8080" };
    return 0;
}
