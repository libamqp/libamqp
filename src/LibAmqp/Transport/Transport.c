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
#include "Transport/Transport.h"
#include "Transport/Socket.h"

void amqp_transport_initialize(amqp_context_t *context, struct ev_loop *loop)
{
    assert(context != 0);
    assert(loop != 0);

    context->transport_state = AMQP_MALLOC(amqp_transport_state_t);
    context->transport_state->loop = loop;
}

void amqp_transport_cleanup(amqp_context_t *context)
{
    assert(context != 0);

    // TODO - cleanup loop if it was not provided
    AMQP_FREE(context->transport_state);
}

amqp_endpoint_t *amqp__initialize_endpoint(amqp_context_t *context, amqp_endpoint_address_t *address)
{
    amqp_endpoint_t *result = AMQP_MALLOC(amqp_endpoint_t);

    assert(address != 0);
    assert(address->hostname != 0);
    assert(address->hostname[0] != 0);

    not_implemented(amqp__initialize_remote_endpoint);
    return result;
}

amqp_connection_t *amqp__create_connection(amqp_context_t *context, amqp_endpoint_t *endpoint, amqp_connection_callback_t callback)
{
    //not_implemented(amqp__create_connection);
    return 0;
}
