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

#include <errno.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Context/Context.h"
#include "Transport/Socket.h"
#include "Transport/SocketInternal.h"


void amqp_transport_initialize(amqp_context_t *context)
{
    assert(context != 0);

    context->transport_state = AMQP_MALLOC(amqp_transport_state_t);
    // TODO initialise fields

    not_implemented(amqp__socket_support_initialize);
}

void amqp_transport_cleanup(amqp_context_t *context)
{
    assert(context != 0);

    // TODO - cleanup
    AMQP_FREE(context->transport_state);
}

void amqp_save_event_loop_for_context(amqp_context_t *context, struct ev_loop *loop)
{
    assert(context != 0);
    assert(context->transport_state != 0);
    assert(loop != 0);

    context->transport_state->loop = loop;
}


static void bzero(void *block, size_t n)
{
    memset(block, '\0', n);
}

static int set_socket_option(int fd, int option, int value)
{
    return setsockopt(fd, SOL_SOCKET, option, &value, sizeof(value));
}

static int set_socket_to_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags |O_NONBLOCK);
}

static int start_listening_on_socket(amqp_context_t *context, int socket_fd, int port_number)
{
    if (set_socket_to_nonblocking(socket_fd) == -1)
    {
        amqp_io_error(context, "Cannot set non-blocking flag on socket");
        return false;
    }

    if (set_socket_option(socket_fd, SO_REUSEADDR, 1) == -1)
    {
        amqp_io_error(context, "Cannot set SO_REUSEADDR option on socket");
        return false;
    }

    struct sockaddr_in6 sin6;
    bzero(&sin6, sizeof(sin6));

    sin6.sin6_family = AF_INET6;
    sin6.sin6_flowinfo = 0;
    sin6.sin6_port = htons(port_number);
    sin6.sin6_addr = in6addr_any;

    if (bind(socket_fd, (struct sockaddr *) &sin6, sizeof(sin6)) == -1)
    {
        amqp_io_error(context, "Cannot bind to IPv6 port");
        return false;
    }

    if (listen(socket_fd, amqp_max_listen_queue_length) == -1)
    {
        amqp_io_error(context, "Cannot start listening on socket");
        return false;
    }

    return true;
}

static
void amqp__accept_handler(struct ev_loop* loop, ev_io* io, const int revents)
{
    assert(loop);
    assert(io);
    assert(revents == EV_READ);

    amqp_context_t *context = (amqp_context_t *) io->data;


    struct sockaddr_storage client_address;
    socklen_t client_address_size = sizeof(client_address);

    const int new_connection_fd = accept(io->fd, (struct sockaddr *) &client_address, &client_address_size);

    if (new_connection_fd == -1)
    {
        switch (errno)
        {
            case EAGAIN:
#if EAGAIN != EWOULDBLOCK
            case EWOULDBLOCK:
#endif
            case EINTR:
                break;
            case ENETDOWN:
            case EPROTO:
            case ENOPROTOOPT:
#ifdef EHOSTDOWN
            case EHOSTDOWN:
#endif
#ifdef ENONET
            case ENONET:
#endif
            case EHOSTUNREACH:
            case EOPNOTSUPP:
            case ENETUNREACH:
                amqp_io_error(context, "Cannot accept. Pending errors on TCP socket causing early death.");
                break;

            default:
                // TODO - consider resetting the listener
                amqp_io_error(context, "Cannot accept. TCP socket error.");
        }
        return;
    }

    if (set_socket_to_nonblocking(new_connection_fd) == -1)
    {
        close(new_connection_fd);
        amqp_io_error(context, "Cannot set client socket to non-blocking");
        return;
    }

    // TODO - consider limiting number of active connections
    // TODO - consider draining accept queue
}

int amqp_setup_listener(amqp_context_t *context, int port_number)
{
    assert(context->transport_state->loop);

    int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        amqp_io_error(context, "Cannot open socket");
        return false;
    }

    if (!start_listening_on_socket(context, socket_fd, port_number))
    {
        close(socket_fd);
        return false;
    }

    context->transport_state->listen_fd = socket_fd;
    context->transport_state->accept_watcher = AMQP_MALLOC(struct ev_io);
    ev_io_init(context->transport_state->accept_watcher, amqp__accept_handler, socket_fd, EV_READ);
    ev_set_priority(context->transport_state->accept_watcher, -2);
    context->transport_state->accept_watcher->data = context;

    ev_io_start(context->transport_state->loop, context->transport_state->accept_watcher);
    return true;
}

#ifdef JUNK
int foo()
{
    struct sockaddr_storage cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
        error("ERROR on accept");
    }

    char buffer[256];
    bzero(buffer, 256);
    int n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    close(newsockfd);
    close(sockfd);

    return false;
}
#endif