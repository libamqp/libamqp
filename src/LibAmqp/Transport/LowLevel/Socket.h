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

#ifndef LIBAMQP_TRANSPORT_SOCKET_H
#define LIBAMQP_TRANSPORT_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif
#include <errno.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

extern void bzero(void *block, size_t n);

inline static
int amqp_set_socket_option(int fd, int option, int value)
{
    socklen_t value_length   = sizeof(value);
    return setsockopt(fd, SOL_SOCKET, option, &value, value_length);
}

inline static
int amqp_set_socket_to_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags |O_NONBLOCK);
}

inline static
int amqp_set_socket_to_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

inline static
int amqp_socket_get_error(int fd)
{
    int so_error = 0;
    socklen_t so_error_length = sizeof(so_error);
    int rc = getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &so_error_length);
    return rc != -1 ? so_error : errno;
}

#ifdef __cplusplus
}
#endif
#endif
