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

amqp_connection_t *amqp_connection_initialize(amqp_context_t *context)
{
    amqp_connection_t *result = AMQP_MALLOC(context, amqp_connection_t);
    amqp_connection_state_initialize(context, result);
    return result;
}

void amqp_connection_destroy(amqp_context_t *context, amqp_connection_t *connection)
{
    connection->state.shutdown(context, connection);
    AMQP_FREE(context, connection);
}

void amqp_connection_connect(amqp_context_t *context, amqp_connection_t *connection, const char *hostname, int port_number)
{

    connection->state.connect(context, connection, hostname, port_number);
}
