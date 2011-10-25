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
#include "Transport/AcceptHandler.h"

#include "Transport/Connection/Connections.h"

#include "debug_helper.h"

amqp_accept_handler_arguments_t *amqp_accept_handler_arguments_initialize(amqp_context_t *context, amqp_connection_test_hook_t *test_hooks)
{
    amqp_accept_handler_arguments_t *result = AMQP_MALLOC(context, amqp_accept_handler_arguments_t);
    result->connections = amqp_connections_initialize(context);
    result->test_hooks = test_hooks;
    return result;
}

void amqp_accept_handler_arguments_destroy(amqp_context_t *context, amqp_accept_handler_arguments_t *arguments)
{
    if (arguments)
    {
        amqp_connections_destroy(context, arguments->connections);
        AMQP_FREE(context, arguments);
    }
}
