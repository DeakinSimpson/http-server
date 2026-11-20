cmake_minimum_required(VERSION 4.4)
project(http_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(http_server
        src/main.cpp
        src/Socket.cpp
        include/Socket.hpp
        include/Socket.hpp
        include/Debug.hpp
        include/ErrorHandle.hpp
        include/ErrorHandle.hpp
        src/WebServer.cpp
        include/WebServer.hpp
        include/WebServer.hpp
        include/SocketUtils.hpp
        include/Connection.cpp
        include/Connection.hpp
        include/Connection.hpp)

include_directories(http_server PRIVATE include)

target_compile_options(http_server PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
