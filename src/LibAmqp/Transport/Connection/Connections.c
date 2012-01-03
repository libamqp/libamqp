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

#include <assert.h>
#include <ev.h>

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/Connections.h"
#include "debug_helper.h"

struct amqp_connections_t
{
    int running_connection_count;   /* no need to lock as all calls currently come from listener thread */
    amqp_connection_t *connection;  // TODO - support more than one connection
};

amqp_connections_t *amqp_connections_initialize(amqp_context_t *context)
{
    amqp_connections_t *result = AMQP_MALLOC(context, amqp_connections_t);
    result->running_connection_count = 0;
    return result;
}

void amqp_connections_destroy(amqp_context_t *context, amqp_connections_t *connections)
{
    assert(connections != 0);
    AMQP_FREE(context, connections);
}

void amqp_connections_add(amqp_context_t *context, amqp_connections_t *connections, amqp_connection_t *connection)
{
    assert(connections != 0 && connections->connection == 0); // currently can only add one connection

    connections->connection = connection;
    connections->running_connection_count++;
}

void amqp_connections_shutdown_confirm(amqp_connections_t *connections)
{
    if (connections)
    {
        connections->running_connection_count--;
    }
}

static void expiry_handler(amqp_context_t *context, amqp_timer_t *timer)
{
    ev_break(context->thread_event_loop, EVBREAK_ONE);
}

void amqp_connections_shutdown_hook(amqp_context_t *context, amqp_event_loop_t *loop, amqp_connections_t *connections)
{
    amqp_timer_t *timer = amqp_timer_initialize(context, expiry_handler, 0);
    ev_tstamp start = ev_time();
    ev_tstamp remaining;
    const ev_tstamp shutdown_delay = 1.0;

    if (connections->running_connection_count > 0)
    {
        amqp_connection_shutdown(context, connections->connection);
    }

    amqp_timer_start(context, timer, shutdown_delay);
    while (connections->running_connection_count > 0)
    {
        ev_run(loop, EVRUN_ONCE);
        remaining = shutdown_delay - (ev_now(loop) - start);
        if (remaining < 0.01)
        {
            break;
        }
        amqp_timer_start(context, timer, remaining + 0.1);
    }

    amqp_timer_stop(context, timer);
    amqp_timer_destroy(context, timer);

    if (connections->running_connection_count > 0)
    {
        amqp_connection_error(connections->connection, AMQP_ERROR_ILLEGAL_STATE, "Not all connections have shutdown. Number outstanding = %ld", (long) connections->running_connection_count);
    }
}
