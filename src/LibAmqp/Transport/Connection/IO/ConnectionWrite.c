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

#include "Transport/Connection/ConnectionStateMachine.h"

DECLARE_TRANSITION_FUNCTION(initialized);
DECLARE_TRANSITION_FUNCTION(enabled);
DECLARE_TRANSITION_FUNCTION(writing);
DECLARE_TRANSITION_FUNCTION(stopping);
DECLARE_TRANSITION_FUNCTION(stopped);
DECLARE_TRANSITION_FUNCTION(failed);

static void cleanup_write_watcher(amqp_connection_t *connection);

void amqp_connection_writer_initialize(amqp_connection_t *connection)
{
    transition_to_state(connection, initialized);
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

static void deallocate_buffer_queue_element(amqp_connection_t *connection, int element_number)
{
    amqp_deallocate_buffer(connection->context, connection->io.write.buffer_queue[element_number]);
    connection->io.write.buffer_queue[element_number] = 0;
}
static void clear_buffer_queue(amqp_connection_t *connection)
{
    int i;
    for (i = 0; i < AMQP_WRITE_QUEUE_SIZE; i++)
    {
        deallocate_buffer_queue_element(connection, i);
    }
}
static void cleanup_resources(amqp_connection_t *connection)
{
    amqp_io_event_watcher_stop(connection->io.write.watcher);
    cleanup_write_watcher(connection);
    clear_buffer_queue(connection);
}

static void call_done_callback(amqp_connection_t *connection)
{
    amqp_connection_action_f callback = connection->io.write.stopped_callback;
    assert(callback != 0);
    connection->io.write.stopped_callback = 0;
    callback(connection);
}
static inline int at_queue_end(amqp_connection_t *connection)
{
    return connection->io.write.start == connection->io.write.end;
}

static inline int increment_queue_index(int index)
{
    return (index + 1) % AMQP_WRITE_QUEUE_SIZE;
}

static void continue_write_operation(amqp_connection_t *connection)
{
    int n = 0;
    int start = connection->io.write.start;
    amqp_buffer_t *buffer = connection->io.write.buffer_queue[start];
    int available_for_write = amqp_buffer_available(buffer);

    while (available_for_write > 0)
    {
        if ((n = amqp_buffer_write(connection->socket.fd, buffer)) == -1)
        {
            break;
        }
        amqp_buffer_advance_read_index(buffer, n);
        available_for_write -= n;
    }
    
    if (n == -1)
    {
        if (errno == EAGAIN)
        {
            amqp_io_event_watcher_start(connection->io.write.watcher);
            return;
        }
        else
        {
            connection->state.writer.fail(connection, errno);
            return;
        }
    }

    connection->io.write.last_write_time = ev_now(connection->context->thread_event_loop);

    deallocate_buffer_queue_element(connection, start);
    connection->io.write.start = increment_queue_index(start);

    connection->state.writer.write_complete(connection);
}

static void commence_write_operation(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    int end = connection->io.write.end;
    connection->io.write.end = increment_queue_index(end);
    connection->io.write.buffer_queue[end] = buffer;

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
    transition_to_state(connection, failed);
    cleanup_resources(connection);

//    amqp_io_error_code(connection->context, error_code, "Failure while writing to %s:%d", connection->socket.hostname, connection->socket.port_number);

    connection->socket.errno_error_code = error_code;
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_WRITE_ERROR);
    connection->state.connection.fail(connection);
}
static void writer_cleanup(amqp_connection_t *connection)
{
    transition_to_state(connection, stopped);

    cleanup_resources(connection);

    amqp_socket_shutdown_output(connection->socket.fd);
}
static void writer_stop(amqp_connection_t *connection, amqp_connection_action_f stopped_callback)
{
    connection->io.write.stopped_callback = stopped_callback;
    writer_cleanup(connection);
    call_done_callback(connection);
}

static void enable_while_initialized(amqp_connection_t *connection)
{
    assert(connection->io.write.watcher == 0);

    connection->io.write.watcher = amqp_io_event_watcher_initialize(connection->context, connection->context->thread_event_loop, write_event_handler, connection->socket.fd, EV_WRITE);
    connection->io.write.watcher->data.connection = connection;

    transition_to_state(connection, enabled);
}
static void stop_while_initialized(amqp_connection_t *connection, amqp_connection_action_f stopped_callback)
{
    connection->io.write.stopped_callback = stopped_callback;
    transition_to_state(connection, stopped);
    call_done_callback(connection);
}
static void abort_while_initialized(amqp_connection_t *connection)
{
    transition_to_state(connection, stopped);
}
DEFINE_TRANSITION_TO_STATE(initialized)
{
    connection->state.writer.enable = enable_while_initialized;
    connection->state.writer.stop = stop_while_initialized;
    connection->state.writer.abort = abort_while_initialized;
}

static void commence_write_while_enabled(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    if (buffer)
    {
        transition_to_state(connection, writing);
        commence_write_operation(connection, buffer);
    }
}
static void next_write_while_enabled(amqp_connection_t *connection)
{
    // Most likely a spurious io event, just stop the watcher.
    amqp_io_event_watcher_stop(connection->io.write.watcher);
}
DEFINE_TRANSITION_TO_STATE(enabled)
{
    connection->state.writer.write = commence_write_while_enabled;
    connection->state.writer.continue_write = next_write_while_enabled;
    connection->state.writer.stop = writer_stop;
    connection->state.writer.fail = writer_fail;
}

static void commence_write_while_writing(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    if (buffer)
    {
        // Not sure how best to handle this
        // queue write
        not_implemented(commence_write_while_writing);
    }
}
static void next_write_while_writing(amqp_connection_t *connection)
{
    amqp_io_event_watcher_stop(connection->io.write.watcher);
    continue_write_operation(connection);
}
static void stop_while_writing(amqp_connection_t *connection, amqp_connection_action_f stopped_callback)
{
    // TODO -  start a timeout - try to complete write but not at any cost
    connection->io.write.stopped_callback = stopped_callback;
    transition_to_state(connection, stopping);
}
static void write_complete_while_writing(amqp_connection_t *connection)
{
    if (at_queue_end(connection))
    {
        // nothing more to do, the buffer queue is empty
        transition_to_state(connection, enabled);
        return;
    }

    // Can call continue_write_operation() or just return and let the next event loop call
    // continue writing the next buffer.
    // Returning may be fairer to other connections and will minimize stack depth.
}
DEFINE_TRANSITION_TO_STATE(writing)
{
    connection->state.writer.write = commence_write_while_writing;
    connection->state.writer.continue_write = next_write_while_writing;
    connection->state.writer.stop = stop_while_writing;
    connection->state.writer.fail = writer_fail;
    connection->state.writer.write_complete = write_complete_while_writing;
}

static void next_write_while_stopping(amqp_connection_t *connection)
{
    amqp_io_event_watcher_stop(connection->io.write.watcher);
    continue_write_operation(connection);
}
static void write_complete_while_stopping(amqp_connection_t *connection)
{
    if (at_queue_end(connection))
    {
        // nothing more to do, the buffer queue is empty
        writer_stop(connection, connection->io.write.stopped_callback);
        return;
    }

    // Let the next event continue with the next buffer. See write_complete_while_writing() above.
}
DEFINE_TRANSITION_TO_STATE(stopping)
{
    connection->state.writer.continue_write = next_write_while_stopping;
    connection->state.writer.write_complete = write_complete_while_stopping;
    connection->state.writer.fail = writer_fail;
}

static void abort_while_stopped_or_failed(amqp_connection_t *connection)
{
    // Nothing to do here
}
DEFINE_TRANSITION_TO_STATE(stopped)
{
    connection->state.writer.abort = abort_while_stopped_or_failed;
}

DEFINE_TRANSITION_TO_STATE(failed)
{
    connection->state.writer.abort = abort_while_stopped_or_failed;
}

/* Default writer states */
static void illegal_write_state(amqp_connection_t *connection, const char *event)
{
    amqp_error(connection->context, AMQP_ERROR_ILLEGAL_STATE,
            "Connection (%s:%d) writer does not support \"%s\" while writer is \"%s\", socket is \"%s\" and connection is \"%s\".",
            connection->socket.hostname, connection->socket.port_number,
            event, connection->state.writer.name, connection->state.socket.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection writer state error");
}

static void default_write_enable(amqp_connection_t *connection)
{
    illegal_write_state(connection, "Enable");
}

static void default_commence_write(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    illegal_write_state(connection, "Write");
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

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.writer.name);

    connection->state.writer.enable = default_write_enable;
    connection->state.writer.write = default_commence_write;
    connection->state.writer.continue_write = default_write_next;
    connection->state.writer.stop = default_write_stop;
    connection->state.writer.abort = default_write_abort;
    connection->state.writer.fail = default_write_fail;
    connection->state.writer.write_complete = default_write_complete;

    connection->state.writer.name = state_name;

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION_WRITER, connection->state.writer.name);
}
