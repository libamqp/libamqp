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
#include "Transport/EventLoop.h"
#include "Transport/EventThread.h"
#include "Transport/Listener.h"
#include "Transport/Socket.h"
#include "Transport/DummyBroker/DummyBroker.h"

#include "debug_helper.h"

struct arguments
{
    int port_number;
    amqp_accept_event_handle_t accept_handler;
};

static int default_accept_new_connection_handler(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t adress_size)
{
    char buffer[128];
    int n;

    set_socket_to_blocking(fd);

    while ((n = read(fd, buffer, sizeof(buffer) -1)) > 0)
    {
        write (fd, buffer, n);
    }
    close(fd);
    return 0;
}

static void dummy_broker_thread_handler(amqp_event_thread_t *event_thread)
{
    struct arguments *arguments = (struct arguments *) event_thread->argument;

    amqp_io_event_watcher_t *accept_watcher = amqp_listener_initialize(event_thread->context, event_thread->loop, arguments->port_number, arguments->accept_handler);

    amqp_event_thread_run_loop(event_thread);

    amqp_listener_destroy(event_thread->context, accept_watcher);
}


amqp_dummy_broker_t *amqp_dummy_broker_initialize(amqp_context_t *context, int listen_port_number, amqp_accept_event_handle_t accept_handler)
{
    amqp_dummy_broker_t *broker = AMQP_MALLOC(context, amqp_dummy_broker_t);
    struct arguments arguments = {
        .port_number = listen_port_number,
        .accept_handler = accept_handler != 0 ? accept_handler : default_accept_new_connection_handler
    };

    broker->thread =  amqp_event_thread_initialize(context, dummy_broker_thread_handler, 0, &arguments);
    return broker;
}

void amqp_dummy_broker_destroy(amqp_context_t *context, amqp_dummy_broker_t *broker)
{
    amqp_event_thread_destroy(context, broker->thread);
    AMQP_FREE(context, broker);
}
