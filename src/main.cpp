#include <iostream>

#include "../include/Socket.hpp"

int main()
{
    std::cout << "Hello World!" << std::endl;

    http_server::Socket socket { "80" };
    return 0;
}
