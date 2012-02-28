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
//#include "Transport/Connection/ConnectionFrame.h"

DECLARE_TRANSITION_FUNCTION(initialized);
DECLARE_TRANSITION_FUNCTION(enabled);
DECLARE_TRANSITION_FUNCTION(reading_frame_header);
DECLARE_TRANSITION_FUNCTION(reading_frame_body);

static const uint32_t amqp_protocol_header = ('A' << 24) | ('M' << 16) | ('Q' << 8) | 'P';

void amqp_connection_frame_reader_initialize(amqp_connection_t *connection)
{
    transition_to_state(connection, initialized);
}

static void cleanup_resources(amqp_connection_t *connection)
{
}
void amqp_connection_frame_reader_cleanup(amqp_connection_t *connection)
{
    cleanup_resources(connection);
}

int amqp_connection_frame_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.frame.name != 0 ? (strcmp(connection->state.frame.name, state_name) == 0) : false;
}

static void read_complete_callback(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    connection->state.frame.read_done(connection, buffer, amount);
}

static void enable_while_initialized(amqp_connection_t *connection)
{
    transition_to_state(connection, enabled);
}
DEFINE_TRANSITION_TO_STATE(initialized)
{
    connection->state.frame.enable = enable_while_initialized;
}

static void read_while_enabled(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_frame_available_f frame_available_callback)
{
    assert(connection && buffer && frame_available_callback);

    amqp_buffer_reset(connection->buffer.read);
    transition_to_state(connection, reading_frame_header);
    connection->io.frame.buffer = buffer;
    connection->io.frame.frame_available_callback = frame_available_callback;
    connection->state.reader.commence_read(connection, buffer, AMQP_FRAME_HEADER_SIZE, read_complete_callback);
}
DEFINE_TRANSITION_TO_STATE(enabled)
{
    connection->state.frame.read = read_while_enabled;
}

static void read_done_while_reading_frame_header(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    if (amount == 0)
    {
//    not_implemented(todo);
        amqp_connection_trace(connection, "Frame read failed - peer has closed the connection");
        connection->state.connection.shutdown(connection);
        return;
    }

    if (amount != AMQP_FRAME_HEADER_SIZE)
    {
        amqp_connection_failed(connection, AMQP_ERROR_PARTIAL_READ, AMQP_CONNECTION_READ_ERROR, "Cannot read frame header. Got %d bytes.", amount);
        return;
    }

    uint32_t frame_size = amqp_buffer_read_uint32(connection->io.frame.buffer, 0);
    connection->io.frame.frame_size = frame_size;

    if ((frame_size >= connection->amqp.connection.limits.max_frame_size) || (frame_size < AMQP_FRAME_HEADER_SIZE) || (frame_size == amqp_protocol_header))
    {
        amqp_connection_failed(connection, AMQP_ERROR_INVALID_FRAME_SIZE, AMQP_CONNECTION_READ_ERROR,
                "Invalid frame size. Size: %08lx, max_frame_size: %lu", (unsigned long) frame_size, (unsigned long) connection->amqp.connection.limits.max_frame_size);
        amqp_dump_buffer(connection->context, buffer, 512);
        return;
    }

    // TODO - close connection of frame size < smallest legal size
    if (frame_size == AMQP_FRAME_HEADER_SIZE)
    {
        // already got the minimum legal frame size
        transition_to_state(connection, enabled);
        connection->io.frame.frame_available_callback(connection, buffer);
        return;
    }

    transition_to_state(connection, reading_frame_body);
    connection->state.reader.commence_read(connection, buffer, frame_size - AMQP_FRAME_HEADER_SIZE, read_complete_callback);
}
static void read_while_reading_frame_header(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_frame_available_f frame_available_callback)
{
    // read in progress so don't start another
}
DEFINE_TRANSITION_TO_STATE(reading_frame_header)
{
    connection->state.frame.read_done = read_done_while_reading_frame_header;
    connection->state.frame.read = read_while_reading_frame_header;
}

static void read_done_while_reading_frame_body(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    if (amount != (connection->io.frame.frame_size - AMQP_FRAME_HEADER_SIZE))
    {
        amqp_connection_failed(connection, AMQP_ERROR_PARTIAL_READ, AMQP_CONNECTION_READ_ERROR, "Cannot read frame body. Got %d bytes. Frame size in header: %lu", amount, (unsigned long) connection->io.frame.frame_size);
        return;
    }

    transition_to_state(connection, enabled);
    connection->io.frame.frame_available_callback(connection, buffer);
}
static void read_while_reading_frame_body(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_frame_available_f frame_available_callback)
{
    // read in progress so don't start another
}
DEFINE_TRANSITION_TO_STATE(reading_frame_body)
{
    connection->state.frame.read_done = read_done_while_reading_frame_body;
    connection->state.frame.read = read_while_reading_frame_body;
}

static void stop_while_stopped(amqp_connection_t *connection)
{
}
static void read_done_while_stopped(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
}
DEFINE_TRANSITION_TO_STATE(stopped)
{
    connection->state.frame.stop = stop_while_stopped;
    connection->state.frame.read_done = read_done_while_stopped;
}

/**********************************************
 Default states
 *********************************************/
static void illegal_state(amqp_connection_t *connection, const char *event)
{
    amqp_connection_error(connection, AMQP_ERROR_ILLEGAL_STATE,
            "Connection frame reader does not support \"%s\" while \"%s\" and connection is \"%s\".",
            event, connection->state.frame.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection frame reader state error");
}

static void default_enable(amqp_connection_t *connection)
{
}
static void default_read(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_frame_available_f frame_available_callback)
{
    illegal_state(connection, "Read");
}
static void default_read_done(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    illegal_state(connection, "ReadDone");
}
static void default_stop(amqp_connection_t *connection)
{
    transition_to_state(connection, stopped);
}

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.frame.name);

    connection->state.frame.enable = default_enable;
    connection->state.frame.read = default_read;
    connection->state.frame.read_done = default_read_done;
    connection->state.frame.stop = default_stop;

    connection->state.frame.name = state_name;

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_FRAME_READER, connection->state.frame.name);
}
