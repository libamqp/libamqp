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

int amqp_socket_shutdown_output(int fd)
{
    return shutdown(fd, SHUT_WR);
}
