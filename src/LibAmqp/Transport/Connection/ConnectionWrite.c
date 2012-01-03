/*
   Copyright 2011-2012 StormMQ Limited

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distribtransition_to_uted under the License is distributed on an "AS IS" BASIS,
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

#include "Buffer/Buffer.h"
#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"

#include "debug_helper.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.writer.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION_WRITER, old_state_name, connection->state.writer.name)
#else
#define save_old_state()
#define trace_transition(old_state_name)
#endif

#define DEFINE_TRANSITION_TO(state) \
static void initialise_actions_for ## state(amqp_connection_t *connection); \
static void transition_to_ ## state(amqp_connection_t *connection) \
{ \
    default_state_initialization(connection, #state, initialise_actions_for ## state); \
} \
static void initialise_actions_for ## state(amqp_connection_t *connection)

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection));

static void transition_to_initialized(amqp_connection_t *connection);
static void transition_to_enabled(amqp_connection_t *connection);
static void transition_to_writing(amqp_connection_t *connection);
static void transition_to_stopping(amqp_connection_t *connection);
static void transition_to_stopped(amqp_connection_t *connection);
static void transition_to_failed(amqp_connection_t *connection);
static void cleanup_write_watcher(amqp_connection_t *connection);

void amqp_connection_writer_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
}
void amqp_connection_writer_cleanup(amqp_connection_t *connection)
{
    cleanup_write_watcher(connection);
}

int amqp_connection_writer_is_state(const amqp_connection_t *connection, const char *state_name)
{
    if (connection->state.writer.name == 0) return false;
    return amqp_connection_is(connection, AMQP_CONNECTION_SOCKET_CONNECTED) && strcmp(connection->state.writer.name, state_name) == 0;
}

static void cleanup_write_watcher(amqp_connection_t *connection)
{
    if (connection->io.write.watcher)
    {
        amqp_io_event_watcher_stop(connection->io.write.watcher);
        amqp_io_event_watcher_destroy(connection->context, connection->io.write.watcher);
        connection->io.write.watcher = 0;
    }
}
static void call_done_callback(amqp_connection_t *connection)
{
    amqp_connection_action_f done_callback = connection->io.write.done_callback;

    assert(done_callback != 0);

    connection->io.write.done_callback = 0;
    done_callback(connection);
}


static void write_what_is_available(amqp_connection_t *connection)
{
    int n;
    int available_for_write = amqp_buffer_available(connection->io.write.buffer);

    while (available_for_write > 0)
    {
        if ((n = amqp_buffer_write(connection->socket.fd, connection->io.write.buffer)) == -1)
        {
            break;
        }
        amqp_buffer_advance_read_index(connection->io.write.buffer, n);
        available_for_write -= n;
    }

    if (n == -1)
    {
        if (errno == EAGAIN)
        {
            // TODO - must exercise this
            amqp_io_event_watcher_start(connection->io.write.watcher);
            return;
        }
        else
        {
            connection->state.writer.fail(connection, errno);
            return;
        }
    }

    connection->state.writer.write_complete(connection);
}

static void continue_write_operation(amqp_connection_t *connection)
{
    write_what_is_available(connection);
}

static void commence_write_operation(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f write_callback)
{
    connection->io.write.buffer = buffer;
    connection->io.write.done_callback = write_callback;

    continue_write_operation(connection);
}

static void write_event_handler(amqp_event_loop_t* loop, ev_io *io, const int revents)
{
    amqp_io_event_watcher_t *watcher = (amqp_io_event_watcher_t *) io;
    amqp_connection_t *connection = watcher->data.connection;

    assert(revents == EV_WRITE);
    connection->state.writer.continue_write(connection);
}

static void writer_fail(amqp_connection_t *connection, int error_code)
{
    amqp_io_event_watcher_stop(connection->io.write.watcher);
    cleanup_write_watcher(connection);

    transition_to_failed(connection);
//    amqp_io_error_code(connection->context, error_code, "Failure while writing to %s:%d", connection->socket.hostname, connection->socket.port_number);
    connection->socket.errno_error_code = error_code;
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_WRITE_ERROR);
    connection->state.connection.fail(connection);
}
static void writer_cleanup(amqp_connection_t *connection)
{
    transition_to_stopped(connection);

    amqp_io_event_watcher_stop(connection->io.write.watcher);
    cleanup_write_watcher(connection);

    amqp_socket_shutdown_output(connection->socket.fd);
}
static void writer_stop(amqp_connection_t *connection, amqp_connection_action_f done_callback)
{
    connection->io.write.done_callback = done_callback;
    writer_cleanup(connection);
    call_done_callback(connection);
}

static void enable_while_initialized(amqp_connection_t *connection)
{
    assert(connection->io.write.watcher == 0);

    connection->io.write.watcher = amqp_io_event_watcher_initialize(connection->context, connection->context->thread_event_loop, write_event_handler, connection->socket.fd, EV_WRITE);
    connection->io.write.watcher->data.connection = connection;

    transition_to_enabled(connection);
}
static void stop_while_initialized(amqp_connection_t *connection, amqp_connection_action_f done_callback)
{
    connection->io.write.done_callback = done_callback;
    transition_to_stopped(connection);
    call_done_callback(connection);
}
static void abort_while_initialized(amqp_connection_t *connection)
{
    transition_to_stopped(connection);
}
DEFINE_TRANSITION_TO(initialized)
{
    connection->state.writer.enable = enable_while_initialized;
    connection->state.writer.stop = stop_while_initialized;
    connection->state.writer.abort = abort_while_initialized;
}

static void commence_write_while_enabled(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback)
{
    transition_to_writing(connection);
    commence_write_operation(connection, buffer, done_callback);
}
static void next_write_while_enabled(amqp_connection_t *connection)
{
    // Most likely a spurious io event, just stop the watcher.
    amqp_io_event_watcher_stop(connection->io.write.watcher);
}
DEFINE_TRANSITION_TO(enabled)
{
    connection->state.writer.commence_write = commence_write_while_enabled;
    connection->state.writer.continue_write = next_write_while_enabled;
    connection->state.writer.stop = writer_stop;
    connection->state.writer.fail = writer_fail;
}

static void commence_write_while_writing(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback)
{
    // Not sure how best to handle this
    not_implemented(writer_commence_write_while_busy);
}
static void next_write_while_writing(amqp_connection_t *connection)
{
    amqp_io_event_watcher_stop(connection->io.write.watcher);
    continue_write_operation(connection);
}
static void stop_while_writing(amqp_connection_t *connection, amqp_connection_action_f stop_callback)
{
    // TODO -  start a timeout - try to complete write but not at any cost
    connection->io.write.done_callback = stop_callback;
    transition_to_stopping(connection);
}
static void write_complete_while_writing(amqp_connection_t *connection)
{
    transition_to_enabled(connection);
    call_done_callback(connection);
}
DEFINE_TRANSITION_TO(writing)
{
    connection->state.writer.commence_write = commence_write_while_writing;
    connection->state.writer.continue_write = next_write_while_writing;
    connection->state.writer.stop = stop_while_writing;
    connection->state.writer.fail = writer_fail;
    connection->state.writer.write_complete = write_complete_while_writing;
}

static void next_write_while_stopping(amqp_connection_t *connection)
{
    amqp_io_event_watcher_stop(connection->io.write.watcher);
    write_what_is_available(connection);
}
static void write_complete_while_stopping(amqp_connection_t *connection)
{
    writer_stop(connection, connection->io.write.done_callback);
}
static void fail_while_stopping(amqp_connection_t *connection, int error_code)
{
    // Nothing to do here
}
DEFINE_TRANSITION_TO(stopping)
{
    connection->state.writer.continue_write = next_write_while_stopping;
    connection->state.writer.write_complete = write_complete_while_stopping;
    connection->state.writer.fail = fail_while_stopping;
}

static void abort_while_stopped_or_failed(amqp_connection_t *connection)
{
    // Nothing to do here
}
DEFINE_TRANSITION_TO(stopped)
{
    connection->state.writer.abort = abort_while_stopped_or_failed;
}

DEFINE_TRANSITION_TO(failed)
{
    connection->state.writer.abort = abort_while_stopped_or_failed;
}

/* Default writer states */
static void illegal_write_state(amqp_connection_t *connection, const char *event)
{
    amqp_error(connection->context, AMQP_ERROR_ILLEGAL_STATE,
            "Connection (%s:%d) writer does not support \"%s\" while \"%s\", socket is \"%s\" and connection is \"%s\".",
            connection->socket.hostname, connection->socket.port_number,
            event, connection->state.writer.name, connection->state.socket.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection writer state error");
}

static void default_write_enable(amqp_connection_t *connection)
{
    illegal_write_state(connection, "Enable");
}

static void default_commence_write(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback)
{
    illegal_write_state(connection, "Commence write");
}

static void default_write_next(amqp_connection_t *connection)
{
    illegal_write_state(connection, "Continue write");
}

static void default_write_stop(amqp_connection_t *connection, amqp_connection_action_f done_callback)
{
    done_callback(connection);
}

static void default_write_abort(amqp_connection_t *connection)
{
    writer_cleanup(connection);
}

static void default_write_fail(amqp_connection_t *connection, int error_code)
{
    illegal_write_state(connection, "Fail");
}

static void default_write_complete(amqp_connection_t *connection)
{
    illegal_write_state(connection, "WriteComplete");
}

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection))
{
    save_old_state();

//    assert(action_initializer);

    connection->state.writer.enable = default_write_enable;
    connection->state.writer.commence_write = default_commence_write;
    connection->state.writer.continue_write = default_write_next;
    connection->state.writer.stop = default_write_stop;
    connection->state.writer.abort = default_write_abort;
    connection->state.writer.fail = default_write_fail;
    connection->state.writer.write_complete = default_write_complete;

    connection->state.writer.name = state_name;

    action_initializer(connection);

    trace_transition(old_state_name);
}
