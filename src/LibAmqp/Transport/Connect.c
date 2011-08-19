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

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "Transport/Connect.h"

// TODO - integrate with libev

static struct addrinfo *lookup_host_address(const char *host_name, int port)
{
    struct addrinfo hints;
    struct addrinfo *result;
    int rc;
    char service[16];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    snprintf(service, sizeof(service), "%d", port);

    rc = getaddrinfo(host_name, service, &hints, &result);

    if (rc != 0)
    {
       warn("%s", gai_strerror(rc));
       return 0;
    }

    return result;
}

static int establish_connection(const char *host_name, struct addrinfo *entries)
{
    struct addrinfo *iterator;
    int socket_fd;
    int error = 0;
    char message[256];

    for (iterator = entries; iterator; iterator = iterator->ai_next)
    {
        socket_fd = socket(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol);
        if (socket_fd < 0)
        {
            error = errno;
            continue;
        }
        if (connect(socket_fd, iterator->ai_addr, iterator->ai_addrlen) == 0)
        {
            return socket_fd;
        }
        error = errno;
        close(socket_fd);
    }

    strerror_r(errno, message, sizeof(message));
    warn("Cannot connect to %s - %s", host_name, message);
    return -1;
}

int amqp_blocking_tcp_connect_to(const char *host_name, int port)
{
    struct addrinfo *host_entries;

    if ((host_entries =  lookup_host_address(host_name, port)) != 0)
    {
        int fd = establish_connection(host_name, host_entries);
        freeaddrinfo(host_entries);
        return fd;
    }

    return -1;
}
