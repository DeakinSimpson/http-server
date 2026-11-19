//
// Created by Deakin Simpson on 19/11/2026.
//

#pragma once
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <netdb.h>

constexpr int BACKLOG_SIZE { 1024 };

namespace http_server
{
    class Socket
    {
        int file_descriptor_ {};

    public:
        Socket(const char* port) : file_descriptor_{open_listen_fd(port) }
        {
            /* check that the file descriptor is open on a socket */
            check_fd_succeeded();
        }

    private:
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
                if ((listen_fd = socket(
                    address->ai_family,
                    address->ai_socktype,
                    address->ai_protocol)
                    ) < 0)
                {
                    /* if fails, go to next in list */
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

        static void getaddrinfo_safe(const char* host, const char* service,
            const addrinfo* hints, addrinfo** result)
        {
            if (getaddrinfo(host, service, hints, result) != 0)
            {
                throw std::runtime_error("Failed to getaddrinfo()");
            }
        }

        static void setsockopt_safe(int socket, int level, int option_name,
            const void *option_value, socklen_t option_len)
        {
            if (setsockopt(socket, level, option_name, option_value, option_len)
                != 0)
            {
                throw std::runtime_error("setsockopt Failed!");
            }
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

        void check_fd_succeeded() const
        {
            if (file_descriptor_ == -1)
            {
                throw std::runtime_error("File Descriptor failed");
            }
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
            else
            {
                std::cerr << "bind failed: " << strerror(errno) << '\n';

                return false;
            }
        }
    };
}