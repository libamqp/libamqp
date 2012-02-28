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

#include "Transport/Connection/ConnectionStateMachine.h"

const ev_tstamp write_complete_time = 0.5;       // TODO - consider scaling according to amount of data being written
const ev_tstamp input_drain_time = 1.0;

DECLARE_TRANSITION_FUNCTION(failed);
DECLARE_TRANSITION_FUNCTION(stopped);
DECLARE_TRANSITION_FUNCTION(stopping);
DECLARE_TRANSITION_FUNCTION(stopping_output);
DECLARE_TRANSITION_FUNCTION(draining_input);

static void timer_expiry_handler(amqp_context_t *context, amqp_timer_t *timer)
{
    amqp_connection_t *connection = (amqp_connection_t *) timer->argument;
    connection->state.connection.timeout(connection);
}

void amqp_connection_state_initialize(amqp_connection_t *connection)
{
    assert(connection->context && connection->context->thread_event_loop);

    connection->buffer.read = amqp_allocate_buffer(connection->context);

    amqp_connection_writer_initialize(connection);
    amqp_connection_reader_initialize( connection);
    amqp_connection_negotiator_initialize(connection);
    amqp_connection_tls_initialize(connection);
    amqp_connection_sasl_initialize(connection);
    amqp_connection_amqp_tunnel_initialize(connection);
    amqp_connection_amqp_initialize(connection);
    amqp_connection_frame_reader_initialize(connection);
    connection->timer = amqp_timer_initialize(connection->context, timer_expiry_handler, connection);
}

void amqp_connection_state_cleanup(amqp_connection_t *connection)
{
    amqp_timer_destroy(connection->context, connection->timer);
    amqp_connection_frame_reader_cleanup(connection);
    amqp_connection_negotiator_cleanup(connection);
    amqp_connection_amqp_cleanup(connection);
    amqp_connection_amqp_tunnel_cleanup(connection);
    amqp_connection_sasl_cleanup(connection);
    amqp_connection_tls_cleanup(connection);
    amqp_connection_reader_cleanup(connection);
    amqp_connection_writer_cleanup(connection);
    amqp_connection_socket_cleanup(connection);

//    amqp_deallocate_buffer(connection->context, connection->buffer.write);
    amqp_deallocate_buffer(connection->context, connection->buffer.read);
    
    AMQP_FREE(connection->context, connection->amqp.connection.local_container_id);
    AMQP_FREE(connection->context, connection->amqp.connection.remote_container_id);
    AMQP_FREE(connection->context, connection->amqp.connection.hostname);
}

int amqp_connection_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.connection.name ? (strcmp(connection->state.connection.name, state_name) == 0) : false;
}

void amqp_connection_enable_io(amqp_connection_t *connection)
{
    connection->state.reader.enable(connection);
    connection->state.writer.enable(connection);
}

void amqp__shutdown_while_in_progress(amqp_connection_t *connection)
{
    amqp_connection_trace(connection, "Shutdown while shutdown in progress.");
//    not_implemented(todo);
}

// Used by tests
static void call_stopped_hook(amqp_connection_t *connection)
{
    if (connection->state.stopped_hook)
    {
        connection->state.stopped_hook(connection);
    }
}

static void timer_stop(amqp_connection_t *connection)
{
    amqp_timer_stop(connection->context, connection->timer);
}

static void timer_start(amqp_connection_t *connection, ev_tstamp delay)
{
    amqp_timer_start(connection->context, connection->timer, delay);
}

static void stop_activity(amqp_connection_t *connection)
{
    connection->state.writer.abort(connection);
    connection->state.reader.stop(connection);
    connection->state.socket.shutdown(connection);
}
static void connection_halt(amqp_connection_t *connection)
{
    timer_stop(connection);
    amqp_connection_flag_clear(connection, AMQP_CONNECTION_RUNNING | AMQP_CONNECTION_SOCKET_CONNECTED | AMQP_CONNECTION_SOCKET_ACCEPTED);
    amqp_connection_shutdown_confirm(connection->context, connection);
    call_stopped_hook(connection);
}

static void do_nothing(amqp_connection_t *connection)
{
}
static void done_or_fail_while_timeout(amqp_connection_t *connection)
{
    transition_to_state(connection, failed);
    connection_halt(connection);
}
DEFINE_TRANSITION_TO_TIMEOUT_STATE()
{
    connection->state.connection.done = done_or_fail_while_timeout;
    connection->state.connection.fail = done_or_fail_while_timeout;
    connection->state.shutdown.drain = do_nothing;
    connection->state.connection.shutdown = do_nothing;
}

static void writer_stop_complete_callback(amqp_connection_t *connection)
{
    connection->state.connection.done(connection);
}

static void read_complete_callback(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    connection->state.connection.read_done(connection, buffer, amount);
}

static void shutdown_connection(amqp_connection_t *connection, amqp_cs_shutdown_mode_t mode)
{
// TODO - remove
connection->trace_flags = 0;
    switch (connection->data.shutdown.mode = mode)
    {
    case amqp_cs_complete_write_drain_input_and_close_socket:
    case amqp_cs_complete_write_and_close_socket:
        timer_start(connection, write_complete_time);
        transition_to_state(connection, stopping_output);
        connection->state.writer.stop(connection, writer_stop_complete_callback);
        break;

    case amqp_cs_close_socket:
    case amqp_cs_abort_socket:
        transition_to_state(connection, stopping);
        connection->state.writer.abort(connection);
        connection->state.reader.stop(connection);
        connection->state.socket.shutdown(connection);
        break;

    default:
        illegal_program_state(connection->data.shutdown.mode);
    }
}

static void done_while_stopping_output(amqp_connection_t *connection)
{
    switch (connection->data.shutdown.mode)
    {
    case amqp_cs_complete_write_drain_input_and_close_socket:
        timer_start(connection, input_drain_time);
        transition_to_state(connection, draining_input);
        amqp_buffer_reset(connection->buffer.read);
        connection->state.reader.reset(connection);
        connection->state.reader.commence_read(connection, connection->buffer.read, amqp_buffer_capacity(connection->buffer.read), read_complete_callback);
        break;

    case amqp_cs_complete_write_and_close_socket:
        transition_to_state(connection, stopping);
        connection->state.reader.stop(connection);
        connection->state.socket.shutdown(connection);
        break;

    default:
        illegal_program_state(connection->data.shutdown.mode);
    }
}
static void timeout_while_stopping_output(amqp_connection_t *connection)
{
    amqp_connection_error(connection, AMQP_ERROR_WRITE_DISCHARGE_TIMEOUT, "Did not complete last write");
    transition_to_timeout(connection, timeout_stopping_output);
    stop_activity(connection);
}

DEFINE_TRANSITION_TO_STATE(stopping_output)
{
    connection->state.connection.done = done_while_stopping_output;
    connection->state.connection.timeout = timeout_while_stopping_output;
}

static void read_done_while_draining_input(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    assert(buffer == connection->buffer.read);

    if (amount > 0)
    {
        transition_to_state(connection, draining_input);   // TODO - transition to same state here, useful for trace
        amqp_buffer_reset(connection->buffer.read);
        connection->state.reader.commence_read(connection, connection->buffer.read, amqp_buffer_capacity(connection->buffer.read), read_complete_callback);
    }
    else
    {
        transition_to_state(connection, stopping);
        connection->state.reader.stop(connection);
        connection->state.socket.shutdown(connection);
    }
}
static void timeout_while_draining_input(amqp_connection_t *connection)
{
    // TODO - distinguish between peer sending forever and peer sending nothing.
    amqp_connection_error(connection, AMQP_ERROR_INPUT_DRAIN_TIMEOUT, "Timeout while draining input");
    transition_to_timeout(connection, timeout_draining_input);
    stop_activity(connection);
}
DEFINE_TRANSITION_TO_STATE(draining_input)
{
    connection->state.connection.read_done = read_done_while_draining_input;
    connection->state.connection.timeout = timeout_while_draining_input;
}

static void done_while_stopping(amqp_connection_t *connection)
{
    switch (connection->data.shutdown.mode)
    {
    case amqp_cs_complete_write_drain_input_and_close_socket:
    case amqp_cs_complete_write_and_close_socket:
    case amqp_cs_close_socket:
        transition_to_state(connection, stopped);

        break;

    case amqp_cs_abort_socket:
        transition_to_state(connection, failed);
        break;

    default:
        illegal_program_state(connection->data.shutdown.mode);
    }

    connection_halt(connection);
}
static void fail_while_stopping(amqp_connection_t *connection)
{
    transition_to_state(connection, failed);
    connection_halt(connection);
}
DEFINE_TRANSITION_TO_STATE(stopping)
{
    // Socket shutdown is immediate unless connecting.
    connection->state.connection.done = done_while_stopping;
    connection->state.connection.fail = fail_while_stopping;
}

DEFINE_TRANSITION_TO_STATE(stopped)
{
    connection->state.shutdown.drain = do_nothing;
    connection->state.connection.shutdown = do_nothing;
}

DEFINE_TRANSITION_TO_STATE(failed)
{
    connection->state.shutdown.drain = do_nothing;
    connection->state.connection.shutdown = do_nothing;
}
/**********************************************
 Default states
 *********************************************/
static void illegal_state(amqp_connection_t *connection, const char *event)
{
    amqp_connection_error(connection, AMQP_ERROR_ILLEGAL_STATE,
            "Connection does not support \"%s\" while \"%s\".",
            event, connection->state.connection.name);
    amqp_fatal_program_error("Connection state error");
}

static void default_drain(amqp_connection_t *connection)
{
    shutdown_connection(connection, amqp_cs_complete_write_drain_input_and_close_socket);
}

static void default_close(amqp_connection_t *connection)
{
    shutdown_connection(connection, amqp_cs_complete_write_and_close_socket);
}

static void default_shutdown(amqp_connection_t *connection)
{
    connection->state.shutdown.close(connection);
}

static void default_fail(amqp_connection_t *connection)
{
    shutdown_connection(connection, amqp_cs_abort_socket);
}

static void default_connect(amqp_connection_t *connection, const char *hostname, int port)
{
    illegal_state(connection, "Connect");
}

static void default_begin(amqp_connection_t *connection, uint32_t version)
{
    illegal_state(connection, "Begin");
}

static void default_reject(amqp_connection_t *connection, uint32_t version)
{
    illegal_state(connection, "Reject");
}

static void default_done(amqp_connection_t *connection)
{
    illegal_state(connection, "Done");
}

static void default_timeout(amqp_connection_t *connection)
{
    illegal_state(connection, "Timeout");
}

void amqp__connection_default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.shutdown.drain = default_drain;
    connection->state.shutdown.close = default_close;

    connection->state.connection.shutdown = default_shutdown;
    connection->state.connection.fail = default_fail;

    if (connection->flags & AMQP_CONNECTION_SOCKET_ACCEPTED)
    {
        connection->state.connection.mode.server.begin = default_begin;
        connection->state.connection.mode.server.reject = default_reject;
    }
    else
    {
        connection->state.connection.mode.client.connect = default_connect;
    }

    connection->state.connection.done = default_done;
    connection->state.connection.timeout = default_timeout;

    connection->state.connection.name = new_state_name;
}

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.connection.name);

    amqp__connection_default_state_initialization(connection, state_name);

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION, connection->state.connection.name);
}

