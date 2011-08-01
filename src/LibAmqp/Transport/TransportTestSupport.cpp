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

#include <stdlib.h>

#include "Transport/TransportTestSupport.h"

namespace SuiteTransport
{
    int TransportFixture::in_memory = false;
    
    amqp_endpoint_address_t TransportFixture::endpoint_address = {
        "localhost", AMQP_DEFAULT_PORT
    };

    TransportFixture::TransportFixture() : endpoint(0)
    {
//        amqp_transport_initialize(context, ev_default_loop(0));
//        amqp_setup_listener(context, port_number);

        endpoint = TransportFixture::initialize_endpoint(context);
//        amqp_save_event_loop_for_context(context, ev_default_loop(0));
    }

    TransportFixture::~TransportFixture()
    {
    }

    amqp_endpoint_t *TransportFixture::initialize_endpoint(amqp_context_t *context)
    {
        amqp_endpoint_t *result;
        if (in_memory)
        {
            result = amqp__initialize_endpoint_stubb(context, &TransportFixture::endpoint_address);
        }
        else
        {
            result = amqp__initialize_endpoint(context, &TransportFixture::endpoint_address);
        }
        return result;
    }
}

//void run(int port_number)
//{
//    amqp_context_t *context = amqp_create_context();
//
//    struct ev_loop *loop = ev_default_loop(0);
//
//    amqp_transport_initialize(context, loop);
//    amqp_setup_listener(context, port_number);
//
//    ev_run(loop, 0);
//
//    // Might never be reached
//    amqp_transport_cleanup(context);
//    amqp_destroy_context(context);
//}
