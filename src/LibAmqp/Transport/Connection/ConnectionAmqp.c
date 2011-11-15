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
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.amqp.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION_AMQP, old_state_name, connection->state.amqp.name)
#else
#define save_old_state()
#define trace_transition(old_state_name)
#endif

static void transition_to_initialized(amqp_connection_t *connection);
static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name);

void amqp_connection_amqp_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
}

static void cleanup_resources(amqp_connection_t *connection)
{
}
void amqp_connection_amqp_cleanup(amqp_connection_t *connection)
{
    cleanup_resources(connection);
}

int amqp_connection_amqp_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.amqp.name != 0 ? (strcmp(connection->state.amqp.name, state_name) == 0) : false;
}

//static
void amqp_done_callback(amqp_connection_t *connection)
{
    connection->state.amqp.done(connection);
}

static void start_while_initialized(amqp_connection_t *connection)
{
    not_implemented(todo);
}
static void wait_while_initialized(amqp_connection_t *connection)
{
    not_implemented(todo);
}
static void transition_to_initialized(amqp_connection_t *connection)
{
    default_state_initialization(connection, "Initialized");
    connection->state.amqp.start = start_while_initialized;
    connection->state.amqp.wait = wait_while_initialized;
    trace_transition("Initialized");
}



/**********************************************
 Default states
 *********************************************/
static void illegal_state(amqp_connection_t *connection, const char *event)
{
    amqp_connection_error(connection, AMQP_ERROR_ILLEGAL_STATE,
            "Connection amqp tunnel does not support \"%s\" while \"%s\" and connection is \"%s\".",
            event, connection->state.amqp.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection amqp state error");
}

static void default_start(amqp_connection_t *connection)
{
    illegal_state(connection, "Start");
}

static void default_wait(amqp_connection_t *connection)
{
    illegal_state(connection, "Wait");
}

static void default_done(amqp_connection_t *connection)
{
    illegal_state(connection, "Done");
}

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.amqp.start = default_start;
    connection->state.amqp.wait = default_wait;
    connection->state.amqp.done = default_done;

    connection->state.amqp.name = new_state_name;
}
