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
    /* default variables */
    static constexpr int BACKLOG_SIZE { 1024 };
    static constexpr int MAX_LINE { 8192 }; /* 8kb */
    static constexpr int BUFFER_SIZE { 32768 }; /* this is to match nginx */

    /* struct that represents a fd */
    struct IOData
    {
        int                             file_descriptor     {};
        int                             unread_bytes_count  {};
        char*                           next_byte           {};
        std::array<char, BUFFER_SIZE>   buffer              {};
    };

    /**
     * this function inits the fd for reading, sets the data to default.
     *
     * @param data where the data structure the fd will be linked to
     * @param file_descriptor the file descriptor that will be read from
     */
    static void io_init(IOData& data, const int file_descriptor)
    {
        data.file_descriptor = file_descriptor;
        data.unread_bytes_count = 0;
        data.next_byte = data.buffer.data(); /* next_byte start of buffer */
    }

    /**
     * reads n bytes from data.file_descriptor to user_buffer
     *
     * @param data the data that will be read from
     * @param user_buffer the buffer that will be written to from the fd
     * @param n the number of bytes that will be read from the fd buffer
     * @return the number of bytes read (-1 if read error)
     */
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

    /**
     * this function will read 1 byte at a time via io_read until it reads \n
     * this is when it will stop reading and return the number of bytes that
     * were read.
     *
     * @param data the data that will be read
     * @param user_buffer where the buffer will be written
     * @param max_length the max length the line can be
     * @return the number of bytes in the line
     */
    static ssize_t io_readline(IOData& data, char* user_buffer, size_t max_length)
    {

        size_t  i;
        int     read_count;
        char    c;

        /* read 1 byte at a time until max_length bytes read, EOF or \n */
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

    /**
     * this function trims the \r\n (crlf) from the end of the bugger
     *
     * @param buffer the buffer that will be trimmed
     * @param length the length of the bytes in the buffer
     * @return the new length of bytes in the buffer
     */
    static std::size_t trim_crlf(char* buffer, size_t length)
    {
        /* remove /r/n from the end of the http line */
        while (length > 0 && (buffer[length - 1] == '\n'
            || buffer[length - 1] == '\r'))
        {
            buffer[--length] = '\0';
        }

        /* return the new length of the buffer */
        return length;
    }

    /**
     * this function handles the getnameinfo function by adding exception
     * checking around it.
     *
     * @param socket_address client socket address that will read
     * @param socket_address_length the length of the socket address
     * @param host the host output
     * @param host_length the max host name output
     * @param service the service (port) output
     * @param service_length the max length of the output
     * @param flags any flags that will be set
     */
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

    /**
     * this function is primarily used for getting the sockets information,
     * when nullptr is passed into host, the information will be grabbed for
     * the servers own socket and can be used to open the file descriptor.
     *
     * @param host the host that getaddrinfo
     * @param service the port that will be used in getaddrinfo
     * @param hints any hints that will be used when getting the addrinfo
     * @param result where the result will be stored
     */
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

    /**
     * this function is a wrapper for setsockopt but handles errors
     */
    static void setsockopt_safe(int socket, int level, int option_name,
                                const void *option_value, socklen_t option_len)
    {
        system_error_handle(setsockopt, "setsockopt failed",
            socket, level, option_name, option_value, option_len);
    }

    /**
     * closes the file descriptor, if it fails, throw runtime error
     *
     * @param file_descriptor the file descriptor that will be closed
     */
    static void close_safe(int file_descriptor)
    {
        if (close(file_descriptor) != 0)
        {
            throw std::runtime_error("Failed to close file descriptor");
        }
    }

    /**
     * calls freeaddrinfo and sets the pointer to a nullptr so it is not accessed
     * again later.
     *
     * @param addr_info the addressinfo that will be freed
     */
    static void freeaddrinfo_safe(addrinfo* addr_info)
    {
        freeaddrinfo(addr_info);
        addr_info = nullptr;
    }

    /**
     * prints the host and the socket of the addrinfo
     *
     * @param addr_info the addrinfo that will be printed
     */
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

    /**
     * binds to the socket and to the file_descriptor
     *
     * @param listen_fd the fd that will be listening
     * @param address the address that will be bound to (the hosts addr)
     * @return true if the bind is successfull
     */
    static bool bind_safe(int listen_fd, const addrinfo* address)
    {
        if (bind(listen_fd, address->ai_addr, address->ai_addrlen) == 0)
        {
            return true;
        }

        std::cerr << "bind failed: " << strerror(errno) << '\n';
        return false;
    }

    /**
     * this function gets the addrinfo of the host machine, this will likely be
     * a list of available addrinfos,
     *
     * the function then walks the list, making attempts to open a file descriptor
     * on the socket using socket(), if there are any errors, log then walk to
     * next addrinfo in list.
     *
     * the function then sets the socket options and attempts to bind to the
     * sockets file descriptor. if this fails, the function will close the file
     * descriptor and try the next in the list.
     *
     * the function then sets the socket/file descriptor to listen on the port
     *
     * if anything fails, return -1
     *
     * @param port the port that the file descriptor will be listening on
     * @return the file desciptor, -1 if fails
     */
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

    /**
     *
     * @param file_descriptor the fd where the bytes will be writen too
     * @param user_buffer the bytes that will be written to fd
     * @param num_bytes the number of bytes being written
     * @return the number of bytes written (negative if error)
     */
    static ssize_t io_write(int file_descriptor, char* user_buffer,
                         std::size_t num_bytes)
    {
        std::size_t remaining_bytes { num_bytes };
        ssize_t bytes_written {};
        char* buffer_ptr { user_buffer };

        /* write until no remaining bytes */
        while (remaining_bytes > 0)
        {
            if ((bytes_written =
                write(file_descriptor, buffer_ptr, remaining_bytes)) <= 0)
            { /* an error occured while writing */

                /* if sig interrupt, rewrite */
                if (errno == EINTR)
                {
                    return 0;
                }

                /* otherwise fail */
                return -1;
            }

            /* decrement bytes */
            remaining_bytes -= bytes_written;
            buffer_ptr += bytes_written;
        }

        /* return bytes written */
        return static_cast<ssize_t>(num_bytes);
    }
}
