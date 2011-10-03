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

#include "Context/Context.h"
#include "Transport/Connection/Connections.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"
#include "Transport/Connection/ConnectionFrame.h"
#include "debug_helper.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.frame.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_FRAME_READER, old_state_name, connection->state.frame.name)
#define trace_disconnect(connection, ...) \
if (connection->trace_flags & AMQP_TRACE_DISCONNECTS) \
    _amqp_connection_trace(connection, __FILE__, __LINE__, __VA_ARGS__)

#else
#define save_old_state()
#define trace_transition(old_state_name)
#define trace_disconnect(context, format,  ...)
#endif

static void transition_to_initialized(amqp_connection_t *connection);

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name);

void amqp_connection_frame_reader_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
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
/*
static void read_complete(amqp_connection_t *connection, int amount)
{
    connection->state.frame.read_done(connection, amount);
}
static void start_while_initialized(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback, amqp_connection_negotiate_callback_f reject_callback)
{
}
static void wait_while_initialized(amqp_connection_t *connection, amqp_connection_negotiate_callback_f request_callback)
{
}*/
static void transition_to_initialized(amqp_connection_t *connection)
{
    default_state_initialization(connection, "Initialized");
//    connection->state.frame.reset = reset_while_initialized;

    trace_transition("Created");
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
    illegal_state(connection, "Enable");
}
static void default_read(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_action_f done_callback)
{
    illegal_state(connection, "Read");
}
static void default_read_done(amqp_connection_t *connection, int amount)
{
    illegal_state(connection, "ReadDone");
}
static void default_stop(amqp_connection_t *connection)
{
    illegal_state(connection, "Stop");
}

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.frame.enable = default_enable;
    connection->state.frame.read = default_read;
    connection->state.frame.read_done = default_read_done;
    connection->state.frame.stop = default_stop;

    connection->state.frame.name = new_state_name;
}
