/*
   Copyright 2011-2012 StormMQ Limited

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

#include "Context/Context.h"
#include "Transport/LowLevel/Listener.h"
#include "Transport/LowLevel/Socket.h"

#include "debug_helper.h"

static
int new_connection(amqp_io_event_watcher_t *accept_watcher, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t client_address_size)
{
    // TODO - it seems best to let the new connection handler decide about non-blocking
//    if (amqp_set_socket_to_nonblocking(fd) == -1)
//    {
//        amqp_io_error(accept_watcher->context, "Cannot set new incomming connection's socket to non-blocking");
//         return -1;
//    }

    if (accept_watcher->data.accept.handler == 0)
    {
        amqp_io_error(accept_watcher->context, "Cannot accept new connection. No accept handler provided.");
        return -1;
    }

    if ((*accept_watcher->data.accept.handler)(accept_watcher, loop, fd, client_address, client_address_size, accept_watcher->data.accept.arguments) == -1)
    {
        amqp_io_error(accept_watcher->context, "New connection handler failed");
        return -1;
    }

    return 0;
}

static
void accept_new_connection_handler(amqp_event_loop_t* loop, ev_io *io, const int revents)
{
    amqp_io_event_watcher_t *watcher = (amqp_io_event_watcher_t *) io;
    struct sockaddr_storage client_address;
    int new_connection_fd;

    socklen_t client_address_size = sizeof(client_address);

    while ((new_connection_fd = accept(watcher->io.fd, (struct sockaddr *) &client_address, &client_address_size)) != -1)
    {
        if (new_connection(watcher, loop, new_connection_fd, &client_address, client_address_size) == -1)
        {
            // TODO - connection was not created, what to do?
            close(new_connection_fd);
            return;
        }

        // TODO - consider limiting number of active connections
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

static int bind_socket_to_any(amqp_context_t *context, amqp_socket_t socket_fd, int port_number)
{
    struct sockaddr_in6 sin6;
    bzero(&sin6, sizeof(sin6));

    sin6.sin6_family = AF_INET6;
    sin6.sin6_flowinfo = 0;
    sin6.sin6_port = htons(port_number);
    sin6.sin6_addr = in6addr_any;

    if (bind(socket_fd, (struct sockaddr *) &sin6, sizeof(sin6)) == -1)
    {
        amqp_io_error(context, "Cannot bind to IPv6 port: %d", port_number);
        return false;
    }

    return true;
}

static int start_listening_on_socket(amqp_context_t *context, amqp_socket_t socket_fd, int port_number)
{
    if (amqp_set_socket_to_nonblocking(socket_fd) == -1)
    {
        amqp_io_error(context, "Cannot set non-blocking flag on socket");
        return false;
    }

    if (amqp_set_socket_to_ignore_sigpipe(socket_fd) == -1)
    {
        amqp_io_error(context, "Cannot set socket to ignore SIGPIPE");
        return false;
    }

    if (amqp_set_socket_option(socket_fd, SO_REUSEADDR, 1) == -1)
    {
        amqp_io_error(context, "Cannot set SO_REUSEADDR option on socket");
        return false;
    }

    // TODO - too promiscuous? Consider using config to specify allowed interfaces.
    if (bind_socket_to_any(context, socket_fd, port_number) == -1)
    {
        return false;
    }

    if (listen(socket_fd, context->config.max_listen_queue_length) == -1)
    {
        amqp_io_error(context, "Cannot start listening on socket");
        return false;
    }

    return true;
}

amqp_io_event_watcher_t *amqp_listener_initialize(amqp_context_t *context, amqp_event_loop_t *loop, int port_number, amqp_accept_event_handle_t accept_handler,  amqp_accept_handler_arguments_t *arguments)
{
    amqp_socket_t socket_fd;
    amqp_io_event_watcher_t *result;
    assert(context != 0);

    socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        amqp_io_error(context, "Cannot open socket");
        return 0;
    }

    if (!start_listening_on_socket(context, socket_fd, port_number))
    {
        close(socket_fd);
        return 0;
    }

    result = amqp_io_event_watcher_initialize(context, loop, accept_new_connection_handler, socket_fd, EV_READ);
    result->data.accept.handler = accept_handler;
    result->data.accept.arguments = arguments;
    amqp_io_event_watcher_adjust_priority(result, -2);
    amqp_io_event_watcher_start(result);

    return result;
}

void amqp_listener_destroy(amqp_context_t *context, amqp_io_event_watcher_t *accept_watcher)
{
    close(accept_watcher->io.fd);
    amqp_io_event_watcher_destroy(context, accept_watcher);
}
