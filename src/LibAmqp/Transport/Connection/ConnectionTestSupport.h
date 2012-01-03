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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_TEST_SUPPORT_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_TEST_SUPPORT_H

#include "libamqp_common.h"
#include "Transport/DummyBroker/DummyBroker.h"
#include "Transport/LowLevel/Connect.h"
#include "Transport/LowLevel/Timer.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTestHook.h"

#include "Context/ContextTestSupport.h"

#define loop_while_socket_state_is(s) \
    CHECK_EQUAL(s, connection->state.socket.name); \
    _loop_while_socket_state_is(s)

#define loop_while_connection_state_is(s) \
    CHECK_EQUAL(s, connection->state.connection.name); \
    _loop_while_connection_state_is(s)

#define loop_until_connection_state_is(s) \
    while (!amqp_connection_is_state(connection, s) && run_loop_with_timeout())

#define loop_until_connection_amqp_state_is(s) \
    while (!amqp_connection_amqp_is_state(connection, s) && run_loop_with_timeout())

#define loop_while_connection_amqp_state_is(s) \
    while (amqp_connection_amqp_is_state(connection, s) && run_loop_with_timeout()); \
    CHECK(!amqp_connection_amqp_is_state(connection, s))


#define loop_while_running() \
    while (BaseConnectionFixture::running) \
    { \
        int event_triggered = BaseConnectionFixture::run_loop_with_timeout(); \
        CHECK(event_triggered); \
        if (!event_triggered) { \
            break; \
        } \
    }

namespace SuiteConnection
{
    class BaseConnectionFixture : public SuiteContext::ContextFixture
    {
    public:
        BaseConnectionFixture();
        ~BaseConnectionFixture();

        void _loop_while_socket_state_is(const char *state);
        void _loop_while_connection_state_is(const char *state);
        void _loop_while_running();
        int run_loop_with_timeout();
        void connect_to(const char *hostname, int port_number);
        void setup_stop_hook();

        static void write_size(amqp_buffer_t *buffer, uint32_t size);
        static void stopped_hook(amqp_connection_t *connection);
        static void do_nothing_action(amqp_connection_t *connection);
        static void not_implemented_action(amqp_connection_t *connection);

        static const char *provide_login(amqp_context_t *context);
        static const char *provide_password(amqp_context_t *context);
        static const char *provide_email(amqp_context_t *context);

    public:
        amqp_event_loop_t *loop;
        amqp_connection_t *connection;
        amqp_timer_t *timer;

        static int running;
        static void expiry_handler(amqp_context_t *context, amqp_timer_t *timer);
    };

    class BrokerUsingFixture : public BaseConnectionFixture
    {
    public:
        BrokerUsingFixture();
        BrokerUsingFixture(amqp_connection_test_hook_t *test_hooks);
        ~BrokerUsingFixture();
    public:
        amqp_dummy_broker_t *dummy_broker;
    };
}
#endif
