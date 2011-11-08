/*
   Copyright 2011 StormMQ Limited

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#include <string.h>
#include <arpa/inet.h>

#include "Context/Context.h"
#include "Transport/LowLevel/Socket.h"

void bzero(void *block, size_t n)
{
    memset(block, '\0', n);
}

void amqp_socket_address_tos(char *buffer, size_t buffer_size, struct sockaddr_storage *socket_address, socklen_t address_size)
{
    char address_buffer[48];
    const char *address = 0;
    int length;

    switch (socket_address->ss_family) {
    case AF_INET:
        if (address_size >= sizeof(struct sockaddr_in))
        {
            address = inet_ntop(AF_INET, &((struct sockaddr_in *) socket_address)->sin_addr, address_buffer, sizeof(address_buffer));
        }
        break;

    case AF_INET6:
        if (address_size >= sizeof(struct sockaddr_in6))
        {
            address = inet_ntop(AF_INET, &((struct sockaddr_in6 *) socket_address)->sin6_addr, address_buffer, sizeof(address_buffer));
        }
        break;
    }

    if (address == 0) address = "<unknown>";
    length = strlen(address);
    if (length > buffer_size) length = buffer_size - 1;
    strncpy(buffer, address, length);
}

int amqp_socket_address_port(struct sockaddr_storage *socket_address, socklen_t address_size)
{
    switch (socket_address->ss_family) {
    case AF_INET:
        return ntohs(((struct sockaddr_in *) socket_address)->sin_port);

    case AF_INET6:
        return ntohs(((struct sockaddr_in6 *) socket_address)->sin6_port);

    default:
        return -1;
    }
}

int amqp_socket_shutdown_output(amqp_socket_t fd)
{
    return shutdown(fd, SHUT_WR);
}

int amqp_set_socket_to_nonblocking(amqp_socket_t fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int amqp_set_socket_to_blocking(amqp_socket_t fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

int amqp_socket_get_error(amqp_socket_t fd)
{
    int so_error = 0;
    socklen_t so_error_length = sizeof(so_error);
    int rc = getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &so_error_length);
    return rc != -1 ? so_error : errno;
}

int amqp_set_socket_to_ignore_sigpipe(amqp_socket_t fd)
{
#ifndef AMQP_WINDOWS_PORT
#ifdef SO_NOSIGPIPE
    int value = 1;
    return amqp_set_socket_option(fd, SO_NOSIGPIPE, value);
#else
#error "Ignore SIGPIPE"
#endif
#endif
}
