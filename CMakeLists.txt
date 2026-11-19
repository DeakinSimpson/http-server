cmake_minimum_required(VERSION 4.4)
project(http_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(http_server
        src/main.cpp
        src/Socket.cpp
        include/Socket.hpp
        include/Socket.hpp
        include/Debug.h)

include_directories(http_server PRIVATE include)
