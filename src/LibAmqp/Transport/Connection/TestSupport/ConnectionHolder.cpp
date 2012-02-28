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

#include <TestHarness.h>

#include "Transport/Connection/TestSupport/ConnectionHolder.h"
#include "Transport/Connection/TestSupport/TestIdentityHooks.h"

#include "Plugin/Plugin.h"

#include "debug_helper.h"

#define TIME_ALLOWED    2.0

namespace TestSupport
{
    int ConnectionHolder::running = false;

    void ConnectionHolder::expiry_handler(amqp_context_t *context, amqp_timer_t *timer)
    {
        ev_break(context->thread_event_loop, EVBREAK_ALL);
    }

    ConnectionHolder::ConnectionHolder() : connection(0)
    {
        amqp_context_set_name(context, "client");
        amqp_register_default_plugins(context);
        amqp_context_register_identity_hooks(context, TestIdentityHooks::provide_login, TestIdentityHooks::provide_password, TestIdentityHooks::provide_email);

        loop = ev_default_loop(0);
        context->thread_event_loop = loop;

        timer = amqp_timer_initialize(context, expiry_handler, 0);
        connection = amqp_connection_create(context);

        running = false;
    }

    ConnectionHolder::~ConnectionHolder()
    {
        amqp_connection_destroy(context, connection);
        amqp_timer_destroy(context, timer);
    }

    int ConnectionHolder::run_loop_with_timeout(double time_allowed)
    {
        const ev_tstamp time_allowed_for_event = TIME_ALLOWED;

        ev_tstamp start = ev_time();
        amqp_timer_start(context, timer, TIME_ALLOWED + 0.1);

        ev_run(context->thread_event_loop, EVRUN_ONCE);

        ev_tstamp now = ev_now(context->thread_event_loop);
        amqp_timer_stop(context, timer);

        ev_tstamp time_waiting_for_event = now - start;

        return time_waiting_for_event < time_allowed_for_event;
    }

    int ConnectionHolder::run_loop_with_timeout()
    {
        return run_loop_with_timeout(TIME_ALLOWED);
    }

    UnconnectedConnectionHolder::UnconnectedConnectionHolder()
    {
        connection->socket.hostname = "unknown";
    }

    UnconnectedConnectionHolder::~UnconnectedConnectionHolder()
    {
        connection->socket.hostname = 0;
    }

    void ClientConnectionHolder::stopped_hook(amqp_connection_t *connection)
    {
        ConnectionHolder::running = false;
    }

    ClientConnectionHolder::ClientConnectionHolder()
    {
        connection->state.connection.done = connection->state.connection.fail = connection->state.stopped_hook = stopped_hook;
    }

    ClientConnectionHolder::~ClientConnectionHolder()
    {
    }

    void ClientConnectionHolder::connect_to(const char *hostname, int port_number)
    {
        ConnectionHolder::running = true;
        connection->state.socket.connect(connection, hostname, port_number);
    }
}
