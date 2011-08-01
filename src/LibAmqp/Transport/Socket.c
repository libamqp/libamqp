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



typedef void (*event_handler_t)(struct ev_loop* loop, ev_io* io, const int revents);

static
amqp_event_watcher_t *register_event_watcher(amqp_context_t *context, event_handler_t handler, int fd, const int revents)
{
    amqp_event_watcher_t *result = AMQP_MALLOC(amqp_event_watcher_t);
    result->context = context;
    ev_io_init((struct ev_io *) result, handler, fd, revents);
    return result;
}

static
void adjust_watcher_priority(amqp_event_watcher_t *watcher, int amount)
{
    ev_set_priority((struct ev_io *) watcher, amount);
}

static
void start_event_watcher(struct ev_loop *loop, amqp_event_watcher_t *watcher)
{
    ev_io_start(loop, (struct ev_io *) watcher);
}

//static TODO - fixme
void stop_event_watcher(struct ev_loop *loop, amqp_event_watcher_t *watcher)
{
    ev_io_stop(loop, (struct ev_io *) watcher);
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


static
void read_handler(struct ev_loop* loop, ev_io* io, const int revents)
{
//    amqp_event_watcher_t *watcher = (amqp_event_watcher_t *) io;

    not_implemented(read_handler);
    // TODO - What to do
}

static
int setup_read_handler(struct ev_loop* loop, amqp_context_t *context, int fd, struct sockaddr_storage *address, socklen_t size)
{
    amqp_event_watcher_t *watcher;

    if (set_socket_to_nonblocking(fd) == -1)
    {
        close(fd);
        amqp_io_error(context, "Cannot set incomming socket connection to non-blocking");
        return -1;
    }

    watcher = register_event_watcher(context, read_handler, fd, EV_READ);
    start_event_watcher(loop, watcher);

    return 0;
}

static
void accept_new_connection_handler(struct ev_loop* loop, ev_io* io, const int revents)
{
    amqp_event_watcher_t *watcher = (amqp_event_watcher_t *) io;

    struct sockaddr_storage client_address;
    socklen_t client_address_size = sizeof(client_address);

    int new_connection_fd;
    while ((new_connection_fd = accept(io->fd, (struct sockaddr *) &client_address, &client_address_size)) != -1)
    {
        if (setup_read_handler(loop, watcher->context, new_connection_fd, &client_address, client_address_size) == -1)
        {
            // TODO - consider limiting number of active connections
            return;
        }
    }

    /*
      accept failed - Most likely the cause was benign.

      Completely ignore EAGAIN, EWOULDBLOCK (which may or may not have the same value as EAGAIN) and EINTR.

      On some systems pending errors (ENETDOWN, EPROTO, ENOPROTOOPT, EHOSTDOWN, ENONET, EHOSTUNREACH,
      EOPNOTSUPP, ENETUNREACH) may be returned by accept. Treat them as EAGAIN.
    */
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
            amqp_io_error(watcher->context, "Cannot accept. Pending errors on TCP socket causing early death.");
            break;

        default:
            // TODO - consider resetting the listener
            amqp_io_error(watcher->context, "Cannot accept. TCP socket error.");
            break;
    }
}

static int bind_socket_to_any(amqp_context_t *context, int socket_fd, int port_number)
{
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

    return true;
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

    if (bind_socket_to_any(context, socket_fd, port_number) == -1)
    {
        return false;
    }

    if (listen(socket_fd, amqp_max_listen_queue_length) == -1)
    {
        amqp_io_error(context, "Cannot start listening on socket");
        return false;
    }

    return true;
}

int amqp_setup_listener(amqp_context_t *context, int port_number)
{
    assert(context);
    assert(context->transport_state);
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

    amqp_event_watcher_t *watcher = register_event_watcher(context, accept_new_connection_handler, socket_fd, EV_READ);
    struct ev_loop *loop = context->transport_state->loop;
    adjust_watcher_priority(watcher, -2);
    start_event_watcher(loop, watcher);

    return true;
}
