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

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include "Context/Context.h"
#include "Transport/LowLevel/EventThread.h"
#include "Transport/LowLevel/Listener.h"
#include "Transport/LowLevel/Socket.h"
#include "Transport/LowLevel/ListenerThread.h"

#include "Transport/Connection/Connections.h" /* TODO - circular dependency :( */
#include "Transport/Connection/ConnectionTestHook.h"

#include "debug_helper.h"


static void connection_shutdown_hook(amqp_context_t *context, amqp_event_loop_t *loop, amqp_accept_handler_arguments_t *accept_handler_arguments)
{
    if (accept_handler_arguments && accept_handler_arguments->connections)
    {
        amqp_connections_shutdown_hook(context, loop, accept_handler_arguments->connections);
    }
}

static void listener_thread_handler(amqp_event_thread_t *event_thread)
{
    struct amqp_listener_thread_t *listener_thread = (struct amqp_listener_thread_t *) event_thread->thread_arguments;

    amqp_io_event_watcher_t *accept_watcher = amqp_listener_initialize(event_thread->context, event_thread->loop, listener_thread->port_number, listener_thread->accept_handler, listener_thread->accept_handler_arguments);

    amqp_event_thread_run_loop_with_shutdown_hook(event_thread, connection_shutdown_hook, listener_thread->accept_handler_arguments);

    amqp_listener_destroy(event_thread->context, accept_watcher);
}

amqp_listener_thread_t *amqp_listener_thread_initialize(amqp_context_t *context, amqp_event_loop_t *loop, int listen_port_number, amqp_accept_event_handle_t accept_handler, const char *context_name, amqp_accept_handler_arguments_t *accept_handler_arguments)
{
    amqp_listener_thread_t *result = AMQP_MALLOC(context, amqp_listener_thread_t);
    result->port_number = listen_port_number;
    result->accept_handler = accept_handler;
    result->context_name = context_name;
    result->accept_handler_arguments = accept_handler_arguments;

    result->thread =  amqp_event_thread_initialize(context, listener_thread_handler, loop, result, context_name);
    return result;
}

void amqp_listener_thread_destroy(amqp_context_t *context, amqp_listener_thread_t *listener_thread)
{
    if (listener_thread)
    {
        amqp_event_thread_destroy(context, listener_thread->thread);
        AMQP_FREE(context, listener_thread);
    }
}
