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

#ifndef LIBAMQP_TRANSPORT_LOW_LEVEL_SOCKET_H
#define LIBAMQP_TRANSPORT_LOW_LEVEL_SOCKET_H

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

#ifndef AMQP_WINDOWS_PORT
typedef int amqp_socket_t;
#include <unistd.h>
#else
typedef HANDLE amqp_socket_t;
#endif

extern void bzero(void *block, size_t n);
extern void amqp_socket_address_tos(char *buffer, size_t buffer_size, struct sockaddr_storage *client_address, socklen_t address_size);
extern int amqp_socket_address_port(struct sockaddr_storage *client_address, socklen_t address_size);
extern int amqp_socket_shutdown_output(amqp_socket_t fd);

inline static
int amqp_set_socket_option(amqp_socket_t fd, int option, int value)
{
    socklen_t value_length   = sizeof(value);
    return setsockopt(fd, SOL_SOCKET, option, &value, value_length);
}

extern int amqp_set_socket_to_nonblocking(amqp_socket_t fd);
extern int amqp_set_socket_to_blocking(amqp_socket_t fd);
extern int amqp_socket_get_error(amqp_socket_t fd);

extern int amqp_set_socket_to_ignore_sigpipe(amqp_socket_t fd);

#ifdef __cplusplus
}
#endif
#endif
