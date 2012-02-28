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

#include "Context/Context.h"
#include "Transport/Connection/Session.h"
#include "Transport/Connection/Connection.h"
#include "debug_helper.h"

amqp_session_t *amqp_session_create(amqp_connection_t *connection)
{
    assert(connection);
    assert(connection->state.amqp.create_session);
    return connection->state.amqp.create_session(connection);
}

int amqp_connection_next_channel(amqp_connection_t *connection)
{
    int i;

    for (i = 0; i < connection->amqp.connection.limits.channel_max; i++)
    {
        if (connection->sessions.sessions[i] == 0)
        {
            return i;
        }
    }
    return -1;
}

static uint32_t suggest_next_outgoing_id(amqp_connection_t *connection)
{
    return 0;
}
static uint32_t max_window_size()
{
    return (((uint32_t) 0) - 1) >> 1;
}

void amqp_session_initialize(amqp_connection_t *connection, amqp_session_t *session, int channel)
{
    session->connection = connection;
    session->local.channel = channel;
    session->local.next_outgoing_id = suggest_next_outgoing_id(connection);
    session->local.incomming_window = max_window_size(connection);
    session->local.outgoing_window = max_window_size(connection);
    session->local.handle_max = 32;         // TODO - magic number
}

void amqp_session_cleanup(amqp_session_t *session)
{
    if (session)
    {
        amqp_free(session->connection->context, session);
    }
}
