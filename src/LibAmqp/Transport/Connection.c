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

#include "Transport/Connection.h"
#include "Context/Context.h"

static void default_state_initialization(amqp_connection_state_t *state);
static void transition_to_stopped(amqp_context_t *context, amqp_connection_state_t *state);
static void transition_to_started(amqp_context_t *context, amqp_connection_state_t *state);


amqp_connection_t *amqp_connection_initialize(amqp_context_t *context)
{
    amqp_connection_t *result = AMQP_MALLOC(context, amqp_connection_t);
    transition_to_stopped(context, &result->state);
    return result;
}

void amqp_connection_destroy(amqp_context_t *context, amqp_connection_t *connection)
{
    AMQP_FREE(context, connection);
}

static void illegal_state(amqp_context_t *context, amqp_connection_state_t *state, const char *event)
{
    amqp_error(context, AMQP_ERROR_ILLEGAL_STATE, "Connection handler entered an illegal state. State: %s, event: %s", state->name, event);
}

static void default_stop(amqp_context_t *context, amqp_connection_state_t *state)
{
    illegal_state(context, state, "Stop");
}

static void default_start(amqp_context_t *context, amqp_connection_state_t *state)
{
    illegal_state(context, state, "Start");
}

static void default_state_initialization(amqp_connection_state_t *state)
{
    state->stop = default_stop;
    state->start = default_start;
}

static void start_connection(amqp_context_t *context, amqp_connection_state_t *state)
{
    transition_to_started(context, state);
}

static void transition_to_stopped(amqp_context_t *context, amqp_connection_state_t *state)
{
    default_state_initialization(state);
    state->start = start_connection;
}

static void stop_connection(amqp_context_t *context, amqp_connection_state_t *state)
{
    transition_to_stopped(context, state);
}

static void transition_to_started(amqp_context_t *context, amqp_connection_state_t *state)
{
    default_state_initialization(state);
    state->stop = stop_connection;
}
