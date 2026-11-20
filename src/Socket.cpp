//
// Created by Deakin Simpson on 19/11/2026.
//

#include "Socket.hpp"
#include "SocketUtils.hpp"
#include "Debug.hpp"

namespace http_server
{
    void Socket::check_fd_succeeded() const
    {
        if (file_descriptor_ == -1)
        {
            throw std::runtime_error("File Descriptor failed");
        }
    }
}
