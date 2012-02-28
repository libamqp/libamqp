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

#include <errno.h>
#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include "Transport/Connection/ConnectionStateMachine.h"

DECLARE_TRANSITION_FUNCTION(initialized);
DECLARE_TRANSITION_FUNCTION(resolving);
DECLARE_TRANSITION_FUNCTION(aborting_lookup);
DECLARE_TRANSITION_FUNCTION(connecting);
DECLARE_TRANSITION_FUNCTION(connected);
DECLARE_TRANSITION_FUNCTION(stopped);
DECLARE_TRANSITION_FUNCTION(failed);
DECLARE_TRANSITION_FUNCTION(accepted);

static const char *duplicate_and_clip(const char *hostname)
{
    const int space = 48;
    const int room = space - 1;
    const int clipped = room - 3;
    char *result = (char *) malloc(space);

    size_t n = strlen(hostname);
    if (n > room)
    {
        strncpy(result, hostname, clipped);
        strcpy(result + clipped, "...");
    }
    else
    {
        strcpy(result, hostname);
    }
    return result;
}
static void save_peer_address(amqp_connection_t *connection, const char *hostname, int port_number)
{
    connection->socket.hostname = duplicate_and_clip(hostname);
    connection->socket.port_number = port_number;
}

void amqp_connection_accepted_socket_initialize(amqp_connection_t *connection, int fd, struct sockaddr_storage *client_address, socklen_t address_size)
{
    char hostname[48];

    transition_to_state(connection, accepted);

    amqp_socket_address_tos(hostname, sizeof(hostname), client_address, address_size);
    save_peer_address(connection, hostname, amqp_socket_address_port(client_address, address_size));

//printf("Accepted connection from: %d, %s:%d\n", fd, connection->socket.hostname, connection->socket.port_number);

    connection->socket.fd = fd;
}

void amqp_connection_socket_initialize(amqp_connection_t *connection)
{
    connection->socket.fd = -1;
    transition_to_state(connection, initialized);
}

static void close_connection_socket(amqp_connection_t *connection);
void amqp_connection_socket_cleanup(amqp_connection_t *connection)
{
    close_connection_socket(connection);
    free((void *) connection->socket.hostname);
}

int amqp_connection_socket_is_state(const amqp_connection_t *connection, const char *state_name)
{
    int result;
    if (connection->state.socket.name == 0) return false;
    result = strcmp(connection->state.socket.name, state_name) == 0;
    return result;
}

static void cleanup_connect_watcher(amqp_connection_t *connection)
{
    if (connection->socket.connect.watcher)
    {
        amqp_io_event_watcher_stop(connection->socket.connect.watcher);
        amqp_io_event_watcher_destroy(connection->context, connection->socket.connect.watcher);
        connection->socket.connect.watcher = 0;
    }
}

static void close_connection_socket(amqp_connection_t *connection)
{
    if (connection->socket.fd != -1)
    {
        close(connection->socket.fd);
        connection->socket.fd = -1;
    }
}

static void free_address_info(amqp_connection_t *connection)
{
    if (connection->socket.connect.lookup.addrinfo)
    {
        freeaddrinfo(connection->socket.connect.lookup.addrinfo);
        connection->socket.connect.lookup.addrinfo = connection->socket.connect.lookup.iterator = 0;
    }
}

static void cleanup_everything(amqp_connection_t *connection)
{
    cleanup_connect_watcher(connection);
    close_connection_socket(connection);
    free_address_info(connection);
}

static void stop_and_notify_connection_shutdown(amqp_connection_t *connection)
{
    transition_to_state(connection, stopped);
    cleanup_everything(connection);
    connection->state.connection.done(connection);
}
static void stop_and_notify_connection_failed(amqp_connection_t *connection)
{
    transition_to_state(connection, failed);
    cleanup_everything(connection);
    connection->state.connection.fail(connection);
}

static void possibly_log_socket_error(amqp_connection_t *connection, int error_code)
{
    switch(error_code) {
    case EPIPE:
    case ECONNREFUSED:
    case ETIMEDOUT:
    case EHOSTUNREACH:
#ifdef EHOSTDOWN
    case EHOSTDOWN:
#endif
    case ENETUNREACH:
        break;

    default:
        // - improve message
        amqp_io_error(connection->context, "Failure while connecting to %s:%d", connection->socket.hostname, connection->socket.port_number);
    }
}
static void connection_attempt_failed(amqp_connection_t *connection, int error_code)
{
    possibly_log_socket_error(connection, error_code);
    close_connection_socket(connection);
    if (connection->socket.connect.lookup.iterator != 0)
    {
        connection->state.socket.try_connect(connection);
    }
    else
    {
        free_address_info(connection);
        connection->state.socket.fail(connection, error_code);
    }
}
static void connect_event_handler(amqp_event_loop_t* loop, ev_io *io, const int revents)
{
    int error;
    amqp_io_event_watcher_t *watcher = (amqp_io_event_watcher_t *) io;
    amqp_connection_t *connection = watcher->data.connection;

    assert(revents == EV_WRITE);

    cleanup_connect_watcher(connection);

    error = amqp_socket_get_error(io->fd);
    if (error != 0)
    {
        connection_attempt_failed(connection, error);
        return;
    }

    connection->state.socket.done(connection);
}
static void start_connect_event_watcher(amqp_connection_t *connection)
{
    connection->socket.connect.watcher = amqp_io_event_watcher_initialize(connection->context, connection->context->thread_event_loop, connect_event_handler, connection->socket.fd, EV_WRITE);
    connection->socket.connect.watcher->data.connection = connection;
    amqp_io_event_watcher_start(connection->socket.connect.watcher);
}
static void attempt_connection_to_next_address(amqp_connection_t *connection)
{
    int error_code;
    struct addrinfo *iterator = connection->socket.connect.lookup.iterator;

    assert(iterator != 0);

    connection->socket.connect.lookup.iterator = connection->socket.connect.lookup.iterator->ai_next;

    connection->socket.fd = socket(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol);
    if (connection->socket.fd == -1)
    {
        connection_attempt_failed(connection, errno);
        return;
    }

    amqp_set_socket_to_nonblocking(connection->socket.fd);
    amqp_set_socket_to_ignore_sigpipe(connection->socket.fd);

    if (connect(connection->socket.fd, iterator->ai_addr, iterator->ai_addrlen) == 0)
    {
        connection->state.socket.done(connection);
        return;
    }

    error_code = errno;
    if (error_code == EINPROGRESS)
    {
        start_connect_event_watcher(connection);
        return;
    }

    connection_attempt_failed(connection, error_code);
}


static void address_resolution_complete(amqp_connection_t *connection, struct addrinfo *addrinfo)
{
    transition_to_state(connection, connecting);
    connection->socket.connect.lookup.addrinfo = connection->socket.connect.lookup.iterator = addrinfo;
    connection->state.socket.try_connect(connection);
}
static void address_resolution_failed(amqp_connection_t *connection, int eai_error)
{
    connection->socket.connect.lookup.eai_error_code = eai_error;
    amqp_error(connection->context, eai_error, "state=%s; Failed to lookup a valid address for %s:%d", connection->state.socket.name, connection->socket.hostname, connection->socket.port_number);

    connection->state.socket.fail(connection, eai_error);
}
static void connect_while_initialized(amqp_connection_t *connection, const char *hostname, int port_number)
{
    struct addrinfo *addrinfo;
    int eai_error;

    transition_to_state(connection, resolving);

    save_peer_address(connection, hostname, port_number);

    eai_error = amqp_lookup_host_address(hostname, port_number, &addrinfo); // TODO - may block - hand off to another thread
    if (eai_error != 0)
    {
        address_resolution_failed(connection, eai_error);
    }
    else
    {
        address_resolution_complete(connection, addrinfo);
    }
}
DEFINE_TRANSITION_TO_STATE(initialized)
{
    connection->state.socket.connect = connect_while_initialized;
}

static void shutdown_while_resolving(amqp_connection_t *connection)
{
    // Have to wait for resolution thread to finish
    transition_to_state(connection, aborting_lookup);
}
static void fail_while_resolving(amqp_connection_t *connection, int error_code)
{
    stop_and_notify_connection_failed(connection);
}
static void done_while_resolving(amqp_connection_t *connection)
{
    // TODO - pickup result of resolution from worker thread
    not_implemented(done_while_resolving);
}
DEFINE_TRANSITION_TO_STATE(resolving)
{
    connection->state.socket.shutdown = shutdown_while_resolving;
    connection->state.socket.fail = fail_while_resolving;
    connection->state.socket.done = done_while_resolving;
}

DEFINE_TRANSITION_TO_STATE(aborting_lookup)
{
}

static void try_connect_while_connecting(amqp_connection_t *connection)
{
    attempt_connection_to_next_address(connection);
}
static void fail_while_connecting(amqp_connection_t *connection, int error_code)
{
    stop_and_notify_connection_failed(connection);
}
static void done_while_connecting(amqp_connection_t *connection)
{
    free_address_info(connection);
    transition_to_state(connection, connected);
    connection->state.connection.done(connection);
}
DEFINE_TRANSITION_TO_STATE(connecting)
{
    connection->state.socket.done = done_while_connecting;
    connection->state.socket.fail = fail_while_connecting;
    connection->state.socket.try_connect = try_connect_while_connecting;
}

DEFINE_TRANSITION_TO_STATE(connected)
{
}

DEFINE_TRANSITION_TO_STATE(stopped)
{
}

DEFINE_TRANSITION_TO_STATE(failed)
{
}

DEFINE_TRANSITION_TO_STATE(accepted)
{
}

/**********************************************
 Default states
 *********************************************/
static void illegal_socket_state(amqp_connection_t *connection, const char *event)
{
    amqp_connection_error(connection, AMQP_ERROR_ILLEGAL_STATE,
            "Connection TCP socket does not support \"%s\" while \"%s\" and connection is \"%s\".",
            event, connection->state.socket.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection TCP socket state error");
}

static void default_connect(amqp_connection_t *connection, const char *hostname, int port)
{
    illegal_socket_state(connection, "Connect");
}

static void default_done(amqp_connection_t *connection)
{
    illegal_socket_state(connection, "Done");
}

static void default_try_connect(amqp_connection_t *connection)
{
    illegal_socket_state(connection, "Try Connect");
}

static void default_fail(amqp_connection_t *connection, int error_code)
{
    illegal_socket_state(connection, "Fail");
}

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.socket.name);

    connection->state.socket.shutdown = stop_and_notify_connection_shutdown;
    connection->state.socket.connect = default_connect;
    connection->state.socket.done = default_done;
    connection->state.socket.fail = default_fail;
    connection->state.socket.try_connect = default_try_connect;

    connection->state.socket.name = state_name;

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION_SOCKET, connection->state.socket.name);
}