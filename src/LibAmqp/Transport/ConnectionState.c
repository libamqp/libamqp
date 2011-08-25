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

#include "Transport/Connection.h"
#include "Context/Context.h"
#include "debug_helper.h"

#ifdef AMQP_TRACE_STATE
#define trace_transition(context, connection, old_state_name) _trace_transition(context, __FILE__, __LINE__, __func__, connection, old_state_name)
static void _trace_transition(const amqp_context_t *context, const char * filename, int line_number, const char *function, amqp_connection_t *connection, const char *old_state_name)
{
    _amqp_debug(context, 2, filename, line_number, function, "Connection unknown to unknown: transitioned from %s to %s", old_state_name, connection->state.name);
}
#else
#define trace_transition(context, connection, old_state_name)
#endif


static void default_state_initialization(amqp_connection_state_t *state);

static void transition_to_stopped_or_initial_state(amqp_context_t *context, amqp_connection_t *connection);
static void transition_to_lookup_failed(amqp_context_t *context, amqp_connection_t *connection);
static void transition_to_connecting_start(amqp_context_t *context, amqp_connection_t *connection);
static void transition_to_connect_failed(amqp_context_t *context, amqp_connection_t *connection);
static void transition_to_connecting_next(amqp_context_t *context, amqp_connection_t *connection);
static void transition_to_connected(amqp_context_t *context, amqp_connection_t *connection);


void amqp_connection_state_initialize(amqp_context_t *context, amqp_connection_t *connection)
{
    transition_to_stopped_or_initial_state(context, connection);
}

int amqp_connection_is_state(amqp_connection_t *connection, const char *state_name)
{
    int result;
    if (connection->state.name == 0) return false;
    result = strcmp(connection->state.name, state_name) == 0;
    return result;
}

static void cleanup_write_watcher(amqp_context_t *context, amqp_connection_t *connection)
{
    amqp_io_event_watcher_stop(connection->write_watcher);
    amqp_io_event_watcher_destroy(context, connection->write_watcher);
    connection->write_watcher = 0;
}

static void cleanup_read_watcher(amqp_context_t *context, amqp_connection_t *connection)
{
    amqp_io_event_watcher_stop(connection->read_watcher);
    amqp_io_event_watcher_destroy(context, connection->read_watcher);
    connection->read_watcher = 0;
}

static void close_connection_socket(amqp_connection_t *connection)
{
    close(connection->socket_fd);
    connection->socket_fd = 0;
}

static void free_address_info(amqp_connection_t *connection)
{
    freeaddrinfo(connection->addrinfo);
    connection->addrinfo = connection->iterator = 0;
}

static void possibly_log_socket_error(amqp_context_t *context, amqp_connection_t *connection, int error_code)
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
        amqp_io_error(context, "Failure while connecting to %s:%d", connection->hostname, connection->port_number);
    }
}

static int create_socket(amqp_context_t *context, amqp_connection_t *connection)
{
    while (connection->iterator && (connection->socket_fd = socket(connection->iterator->ai_family, connection->iterator->ai_socktype, connection->iterator->ai_protocol)) == -1)
    {
        connection->iterator = connection->iterator->ai_next;
    }

    if (connection->iterator == 0)
    {
        int error_code = errno;

        /* Could not open a socket using any of the addrinfo entries */
        connection->state.connect_failed(context, connection, error_code);
        return -1;
    }

    return 0;
}

static void connect_event_handler(amqp_event_loop_t* loop, ev_io *io, const int revents)
{
    int error;
    amqp_io_event_watcher_t *watcher = (amqp_io_event_watcher_t *) io;
    amqp_connection_t *connection = watcher->data.connection;

    assert(revents == EV_WRITE);

    amqp_io_event_watcher_stop(watcher);

    error = amqp_socket_get_error(io->fd);
    if (error != 0)
    {
        possibly_log_socket_error(watcher->context, connection, error);
        connection->state.next(watcher->context, connection);
        return;
    }

    connection->state.connect_finish(watcher->context, connection);
}

static void attempt_connection(amqp_context_t *context, amqp_connection_t *connection)
{
    int rc, ec;

    amqp_set_socket_to_nonblocking(connection->socket_fd);
    connection->write_watcher = amqp_io_event_watcher_initialize(context, context->loop, connect_event_handler, connection->socket_fd, EV_WRITE);
    connection->write_watcher->data.connection = connection;

    rc = connect(connection->socket_fd, connection->iterator->ai_addr, connection->iterator->ai_addrlen);
    if (rc == 0)
    {
        connection->state.connect_finish(context, connection);
        return;
    }

    ec = errno;
    if (ec != EINPROGRESS)
    {
        possibly_log_socket_error(context, connection, ec);

        /* Try the next address returned by the DNS lookup */
        connection->state.next(context, connection);
        return;
    }

    amqp_io_event_watcher_start(connection->write_watcher);
}

static void connect_while_stopped_or_failed(amqp_context_t *context, amqp_connection_t *connection, const char *hostname, int port_number)
{
    struct addrinfo *addrinfo;
    int eai_error;

    transition_to_connecting_start(context, connection);

    connection->hostname = strdup(hostname);
    connection->port_number = port_number;

    eai_error = amqp_lookup_host_address(hostname, port_number, &addrinfo);
    if (eai_error != 0)
    {
        connection->state.lookup_failed(context, connection, eai_error);
        return;
    }

    connection->addrinfo = connection->iterator = addrinfo;

    if (create_socket(context, connection) == -1)
    {
        return;
    }

    attempt_connection(context, connection);
}

static void next_while_connecting(amqp_context_t *context, amqp_connection_t *connection)
{
    transition_to_connecting_next(context, connection);

    close_connection_socket(connection);
    cleanup_write_watcher(context, connection);

    connection->iterator = connection->iterator->ai_next;

    if (create_socket(context, connection) == -1)
    {
        return;
    }

    attempt_connection(context, connection);
}

static void write_event_handler(amqp_event_loop_t* loop, ev_io *io, const int revents)
{
    not_implemented(write_event_handler);
}

static void read_event_handler(amqp_event_loop_t* loop, ev_io *io, const int revents)
{
    not_implemented(read_event_handler);
}

static void connect_finish_while_connecting(amqp_context_t *context, amqp_connection_t *connection)
{
    transition_to_connected(context, connection);

    amqp_io_event_watcher_cb_set(connection->write_watcher, write_event_handler);

    connection->read_watcher = amqp_io_event_watcher_initialize(context, context->loop, read_event_handler, connection->socket_fd, EV_READ);
    connection->read_watcher->data.connection = connection;
}

static void shutdown_while_stopped(amqp_context_t *context, amqp_connection_t *connection)
{
    /* nothing to do */
}

static void transition_to_stopped_or_initial_state(amqp_context_t *context, amqp_connection_t *connection)
{
#ifdef AMQP_TRACE_STATE
    const char* old_state_name = connection->state.name;
#endif
    default_state_initialization(&connection->state);

    connection->running = false;
    connection->state.name = "Stopped";
    connection->state.shutdown = shutdown_while_stopped;
    connection->state.connect = connect_while_stopped_or_failed;

#ifdef AMQP_TRACE_STATE
    if (old_state_name != 0)
    {
        trace_transition(context, connection, old_state_name);
    }
#endif
}

static void shutdown_while_connect_start_or_connect_next(amqp_context_t *context, amqp_connection_t *connection)
{
    transition_to_stopped_or_initial_state(context, connection);
    free_address_info(connection);
    close_connection_socket(connection);
    cleanup_write_watcher(context, connection);
    free((void *) connection->hostname);
}
static void lookup_failed_while_connect_start(amqp_context_t *context, amqp_connection_t *connection, int error_code)
{
    transition_to_lookup_failed(context, connection);
    connection->eai_error_code = error_code;
    amqp_error(context, error_code, "Failed to lookup a valid addres for %s:%d", connection->hostname, connection->port_number);
}
static void connect_failed_while_connect_start(amqp_context_t *context, amqp_connection_t *connection, int error_code)
{
    transition_to_connect_failed(context, connection);
    connection->errno_error_code = error_code;
    amqp_error(context, error_code, "Failed to connect to %s%d - Could not create a socket for any address.", connection->hostname, connection->port_number);

}
static void transition_to_connecting_start(amqp_context_t *context, amqp_connection_t *connection)
{
#ifdef AMQP_TRACE_STATE
    const char* old_state_name = connection->state.name;
#endif
    default_state_initialization(&connection->state);
    connection->running = true;
    connection->state.name = "Connecting";
    connection->state.shutdown = shutdown_while_connect_start_or_connect_next;
    connection->state.lookup_failed = lookup_failed_while_connect_start;
    connection->state.connect_failed = connect_failed_while_connect_start;
    connection->state.next = next_while_connecting;
    connection->state.connect_finish = connect_finish_while_connecting;
    trace_transition(context, connection, old_state_name);
}

static void connect_failed_while_connect_next(amqp_context_t *context, amqp_connection_t *connection, int error_code)
{
    transition_to_connect_failed(context, connection);
    connection->errno_error_code = error_code;
    amqp_error(context, error_code, "Failed to connect to %s:%d.", connection->hostname, connection->port_number);
}
static void transition_to_connecting_next(amqp_context_t *context, amqp_connection_t *connection)
{
#ifdef AMQP_TRACE_STATE
    const char* old_state_name = connection->state.name;
#endif
    default_state_initialization(&connection->state);
    connection->state.name = "Connecting (still)";
    connection->state.shutdown = shutdown_while_connect_start_or_connect_next;
    connection->state.connect_failed = connect_failed_while_connect_next;
    connection->state.next = next_while_connecting;
    connection->state.connect_finish = connect_finish_while_connecting;
    trace_transition(context, connection, old_state_name);
}

static void shutdown_while_connected(amqp_context_t *context, amqp_connection_t *connection)
{
    transition_to_stopped_or_initial_state(context, connection);

    // TODO -  send shutdown command
    close_connection_socket(connection);
    cleanup_write_watcher(context, connection);
    cleanup_read_watcher(context, connection);
    free((void *) connection->hostname);
}
static void transition_to_connected(amqp_context_t *context, amqp_connection_t *connection)
{
#ifdef AMQP_TRACE_STATE
    const char* old_state_name = connection->state.name;
#endif
    default_state_initialization(&connection->state);
    connection->state.name = "Connected";
    connection->state.shutdown = shutdown_while_connected;
    trace_transition(context, connection, old_state_name);
}

static void shutdown_connection_while_failed(amqp_context_t *context, amqp_connection_t *connection)
{
    transition_to_stopped_or_initial_state(context, connection);
    connection->eai_error_code = 0;
    connection->errno_error_code = 0;
    free((void *) connection->hostname);
}

static void transition_to_lookup_failed(amqp_context_t *context, amqp_connection_t *connection)
{
#ifdef AMQP_TRACE_STATE
    const char* old_state_name = connection->state.name;
#endif
    default_state_initialization(&connection->state);

    connection->running = false;
    connection->state.name = "Lookup failed";
    connection->state.shutdown = shutdown_connection_while_failed;
    connection->state.connect = connect_while_stopped_or_failed;
    trace_transition(context, connection, old_state_name);
}

static void transition_to_connect_failed(amqp_context_t *context, amqp_connection_t *connection)
{
#ifdef AMQP_TRACE_STATE
    const char* old_state_name = connection->state.name;
#endif
    default_state_initialization(&connection->state);

    connection->running = false;
    connection->state.name = "Connect failed";
    connection->state.shutdown = shutdown_connection_while_failed;
    connection->state.connect = connect_while_stopped_or_failed;
    trace_transition(context, connection, old_state_name);
}

/* Default states */
static void illegal_state(amqp_context_t *context, amqp_connection_t *connection, const char *event)
{
    amqp_error(context, AMQP_ERROR_ILLEGAL_STATE, "Connection handler entered an illegal state. State: %s, event: %s", connection->state.name, event);
    abort(); // TODO - remove ?
}

static void default_shutdown(amqp_context_t *context, amqp_connection_t *connection)
{
    illegal_state(context, connection, "Shutdown");
}

static void default_connect(amqp_context_t *context, amqp_connection_t *connection, const char *hostname, int port)
{
    illegal_state(context, connection, "Connect");
}

static void default_lookup_failed(amqp_context_t *context, amqp_connection_t *connection, int error_code)
{
    illegal_state(context, connection, "Lookup Failed");
}

static void default_connect_failed(amqp_context_t *context, amqp_connection_t *connection, int error_code)
{
    illegal_state(context, connection, "Connect Failed");
}

static void default_next(amqp_context_t *context, amqp_connection_t *connection)
{
    illegal_state(context, connection, "Next");
}

static void default_connect_finish(amqp_context_t *context, amqp_connection_t *connection)
{
    illegal_state(context, connection, "Connect Finish");
}

//static void default_read(amqp_context_t *context, amqp_connection_t *connection, amqp_event_loop_t *loop, amqp_io_event_watcher_t *watcher)
//{
//    illegal_state(context, connection, "Read");
//}
//
//static void default_write(amqp_context_t *context, amqp_connection_t *connection, amqp_event_loop_t *loop, amqp_io_event_watcher_t *watcher)
//{
//    illegal_state(context, connection, "Write");
//}

static void default_state_initialization(amqp_connection_state_t *state)
{
    state->shutdown = default_shutdown;
    state->connect = default_connect;
    state->lookup_failed = default_lookup_failed;
    state->connect_failed = default_connect_failed;
    state->next = default_next;
    state->connect_finish = default_connect_finish;
//    state->read = default_read;
//    state->write = default_write;
}