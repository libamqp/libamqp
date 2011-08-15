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
#include <unistd.h>


extern void bzero(void *block, size_t n);

inline static
int set_socket_option(int fd, int option, int value)
{
    return setsockopt(fd, SOL_SOCKET, option, &value, sizeof(value));
}

inline static
int set_socket_to_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags |O_NONBLOCK);
}

inline static
int set_socket_to_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

#ifdef __cplusplus
}
#endif
#endif
