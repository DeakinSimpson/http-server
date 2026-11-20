//
// Created by Deakin Simpson on 20/11/2026.
//

#pragma once
#include <iostream>
#include <netdb.h>
#include <unistd.h>

#include "Debug.hpp"
#include "ErrorHandle.hpp"

namespace http_server
{
    static constexpr int BACKLOG_SIZE { 1024 };
    static constexpr int MAX_LINE { 8192 }; /* 8kb */
    static constexpr int BUFFER_SIZE { 16000 };

    struct IOData
    {
        int                             file_descriptor     {};
        int                             unread_bytes_count  {};
        char*                           next_byte           {};
        std::array<char, BUFFER_SIZE>   buffer              {};
    };

    static void io_init(IOData& data, const int file_descriptor)
    {
        data.file_descriptor = file_descriptor;
        data.unread_bytes_count = 0;
        data.next_byte = data.buffer.data(); /* next_byte start of buffer */
    }

    static ssize_t io_read(IOData& data, char* user_buffer, size_t n)
    {
        while (data.unread_bytes_count <= 0) /* fill the buffer */
        {
            data.unread_bytes_count = read(data.file_descriptor,
                data.buffer.data(), sizeof(data.buffer.data()));

            if (data.unread_bytes_count < 0)
            {
                const int error_code { errno };
                if (error_code != EINTR)
                {
                    std::cerr << "buffer read error: " << strerror(error_code)
                        << std::endl;

                    return -1;
                }
            }
            else if (data.unread_bytes_count == 0) /* EOF */
            {
                return 0;
            }
            else
            {
                data.next_byte = data.buffer.data();
            }
        }

        /* copy data from data buffer to buffer */
        int bytes_read = std::min(static_cast<int>(n), data.unread_bytes_count);
        std::memcpy(user_buffer, data.next_byte, bytes_read);

        /* set next byte to end of buffer and decrement the unread bytes */
        data.next_byte += bytes_read;
        data.unread_bytes_count -= bytes_read;

        /* return the number of bytes read */
        return bytes_read;
    }

    static ssize_t io_readline(IOData& data, char* user_buffer, size_t max_length)
    {

        size_t  i;
        int     read_count;
        char    c;

        for (i = 1; i < max_length; ++i)
        {
            if ((read_count = io_read(data, &c, 1)) == 1)
            {
                *user_buffer++ = c;

                if (c == '\n')
                {
                    ++i;
                    break;
                }
            }
            else if (read_count == 0)
            {
                if (i == 1)
                {
                    return 0; /* EOF no data read */
                }
                break; /* EOF > 0 bytes read */
            }
            else
            {
                return -1;
            }
        }

        *user_buffer     = 0;
        return i - 1;
    }

    static std::size_t trim_crlf(char* buffer, size_t length)
    {
        /* remove /r/n from the end of the http line */
        while (length > 0 && buffer[length - 1] == '\n'
            || buffer[length - 1] == '\r')
        {
            buffer[--length] = '\0';
        }

        /* return the new length of the buffer */
        return length;
    }

    static void getnameinfo_safe(
        const sockaddr*     socket_address,
        socklen_t           socket_address_length,
        char*               host,                   size_t host_length,
        char*               service,                size_t service_length,
        int                 flags)
    {
        system_error_handle(getnameinfo, "getnameinfo failed",
            socket_address, socket_address_length, host, host_length, service,
            service_length, flags);
    }

    static void getaddrinfo_safe(const char* host, const char* service,
            const addrinfo* hints, addrinfo** result)
    {
        if (const int status = getaddrinfo(host, service, hints, result);
            status != 0)
        {
            std::cerr << "getaddrinfo failed: "
                      << gai_strerror(status) << '\n';

            throw std::runtime_error("getaddrinfo failed");
        }
    }

    static void setsockopt_safe(int socket, int level, int option_name,
        const void *option_value, socklen_t option_len)
    {
        system_error_handle(setsockopt, "setsockopt failed",
            socket, level, option_name, option_value, option_len);
    }

    static void close_safe(int file_descriptor)
    {
        if (close(file_descriptor) != 0)
        {
            throw std::runtime_error("Failed to close file descriptor");
        }
    }

    static void freeaddrinfo_safe(addrinfo* addr_info)
    {
        freeaddrinfo(addr_info);
        addr_info = nullptr;
    }

    static void print_addrinfo(const addrinfo* addr_info)
    {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        if (getnameinfo(addr_info->ai_addr, addr_info->ai_addrlen,
            host, sizeof(host), service, sizeof(service),
            NI_NUMERICHOST | NI_NUMERICSERV) == 0)
        {
            std::cout << "listening on [" << host << "]:" << service;
            std::cout << "(family: " <<
                ((addr_info->ai_family == AF_INET6) ? "IPv6" : "IPv4")
                << ")\n";
        }
    }

    static bool bind_safe(int listen_fd, const addrinfo* address)
    {
        if (bind(listen_fd, address->ai_addr, address->ai_addrlen) == 0)
        {
            return true;
        }

        std::cerr << "bind failed: " << strerror(errno) << '\n';
        return false;
    }

    static int open_listen_fd(const char* port)
    {
        addrinfo    hints {};
        addrinfo*   addr_list;
        addrinfo*   address;
        int         listen_fd { -1 };
        int         opt_val { 1 };

        /* set the hits to use for getting the address */
        hints.ai_family = AF_UNSPEC;                /* allow IPv4 & IPv6 */
        hints.ai_socktype = SOCK_STREAM;            /* accept connections */
        hints.ai_flags = AI_PASSIVE;                /* on any IP addr */
        hints.ai_flags |= AI_NUMERICSERV;           /* using port num */

        /* get the list of addresses */
        getaddrinfo_safe(nullptr, port, &hints, &addr_list);

        /* go over each item in the linked list until the fd binds */
        for (address = addr_list; address != nullptr;
            address = address->ai_next)
        {
            /* create socket descriptor */
            try
            {
                listen_fd = system_error_handle(
                    &socket,
                    "socket failed",
                    address->ai_family,
                    address->ai_socktype,
                    address->ai_protocol
                );
            }
            catch (const std::system_error& e)
            {
                std::cerr << e.what() << '\n';
                continue;
            }

            /* remove "Adress already in use" error from bind */
            setsockopt_safe(listen_fd,
                SOL_SOCKET,
                SO_REUSEADDR,
                &opt_val,
                sizeof(int));

            /* bind the file descriptor to the address */
            if (bind_safe(listen_fd, address))
            {
                /* successfully bind*/
                print_addrinfo(address);
                break;
            }

            /* if bind fails, close descriptor and try next */
            close_safe(listen_fd);
        }

        /* clean up */
        freeaddrinfo_safe(addr_list);

        /* no address worked */
        if (!address)
        {
            return -1;
        }

        /* set the port to list, ready to accept connection requests */
        if (listen(listen_fd, BACKLOG_SIZE) < 0)
        {
            close_safe(listen_fd);
            return -1;
        }

        return listen_fd;
    }
}
