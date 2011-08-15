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
#include "Transport/Listener.h"

amqp_transport_state_t * amqp_transport_initialize_with_ev_loop(amqp_context_t *context, struct ev_loop *loop)
{
    amqp_transport_state_t *result;
    assert(context != 0 && loop != 0);
    result = AMQP_MALLOC(amqp_transport_state_t);
    result->loop = loop;
    return result;
}

void amqp_transport_cleanup(amqp_transport_state_t *transport_state)
{
    assert(transport_state != 0);

    AMQP_FREE(transport_state);
}

 void socket_endpoint_cleanup(amqp_endpoint_t *endpoint)
{
}

amqp_endpoint_t *amqp__endpoint_initialize(amqp_context_t *context, amqp_endpoint_address_t *address)
{
    amqp_endpoint_t *result = AMQP_MALLOC(amqp_endpoint_t);

    assert(address != 0);
    assert(address->hostname != 0);
    assert(address->hostname[0] != 0);

//    result->read = 0;
//    result->write = 0;
//    result->cleanup = socket_endpoint_cleanup;

    return result;
}

void amqp_endpoint_destroy(amqp_endpoint_t *endpoint)
{
    assert(endpoint != 0);
//    assert(endpoint->cleanup != 0);
//
//    (*endpoint->cleanup)(endpoint);
    AMQP_FREE(endpoint);
}

amqp_connection_t *amqp__create_connection(amqp_context_t *context, amqp_endpoint_t *endpoint, amqp_connection_callback_t callback)
{
    not_implemented(amqp__create_connection);
    return 0;
}
