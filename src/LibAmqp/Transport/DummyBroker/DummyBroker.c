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

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/DummyBroker/DummyBroker.h"

#include "debug_helper.h"

static int accept_handler(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *accept_handler_arguments)
{
    amqp_context_t *context = me->context;

    assert(context && context->thread_event_loop);
    assert(accept_handler_arguments && accept_handler_arguments->connections);
    amqp_connection_accept(context, fd, client_address, address_size, accept_handler_arguments);

    return 0;
}

amqp_dummy_broker_t *amqp_dummy_broker_initialize(amqp_context_t *context, int listen_port_number, amqp_connection_test_hook_t *test_hooks)
{
    amqp_event_loop_t *default_event_loop = 0;

    amqp_dummy_broker_t *result = AMQP_MALLOC(context, amqp_dummy_broker_t);
    result->accept_handler_arguments = amqp_accept_handler_arguments_initialize(context, test_hooks, 0);
    result->listener_thread = amqp_listener_thread_initialize(context, default_event_loop, listen_port_number, accept_handler, "dummy-broker", result->accept_handler_arguments);
    return result;
}

void amqp_dummy_broker_destroy(amqp_context_t *context, amqp_dummy_broker_t *broker)
{
    amqp_listener_thread_destroy(context, broker->listener_thread);
    amqp_accept_handler_arguments_destroy(context, broker->accept_handler_arguments);
    AMQP_FREE(context, broker);
}
