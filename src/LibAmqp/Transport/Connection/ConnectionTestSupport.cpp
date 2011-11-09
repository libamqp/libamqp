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

#include <TestHarness.h>

#include "Transport/Connection/ConnectionTestSupport.h"
#include "Plugin/Plugin.h"

#include "debug_helper.h"

namespace SuiteConnection
{
    int BaseConnectionFixture::running = false;

    BaseConnectionFixture::BaseConnectionFixture() : connection(0)
    {
        amqp_context_set_name(context, "client");
        amqp_register_default_plugins(context);
        amqp_context_register_identity_hooks(context, provide_login, provide_password, provide_email);
        
        loop = ev_default_loop(0);
        context->thread_event_loop = loop;
        timer = amqp_timer_initialize(context, BaseConnectionFixture::expiry_handler, 0);
    }

    BaseConnectionFixture::~BaseConnectionFixture()
    {
        amqp_timer_destroy(context, timer);
    }

    void BaseConnectionFixture::expiry_handler(amqp_context_t *context, amqp_timer_t *timer)
    {
        ev_break(context->thread_event_loop, EVBREAK_ALL);
    }

    void BaseConnectionFixture::write_size(amqp_buffer_t *buffer, uint32_t size)
    {
        amqp_four_byte_t value;
        value._uint = size;
        amqp_hton_32(buffer, value);
    }

#define TIME_ALLOWED    2.0

    int BaseConnectionFixture::run_loop_with_timeout()
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

    void BaseConnectionFixture::_loop_while_socket_state_is(const char *state)
    {
        while (amqp_connection_socket_state_is(connection, state) && run_loop_with_timeout());
    }

    void BaseConnectionFixture::_loop_while_connection_state_is(const char *state)
    {
        while (amqp_connection_is_state(connection, state) && run_loop_with_timeout());
    }

    void BaseConnectionFixture::_loop_while_running()
    {
        while (BaseConnectionFixture::running && run_loop_with_timeout());
    }

    void BaseConnectionFixture::stopped_hook(amqp_connection_t *connection)
    {
        BaseConnectionFixture::running = false;
    }

    void BaseConnectionFixture::do_nothing_action(amqp_connection_t *connection)
    {
    }

    void BaseConnectionFixture::not_implemented_action(amqp_connection_t *connection)
    {
        not_implemented(not_implemented_action);
    }

    void BaseConnectionFixture::setup_stop_hook()
    {
        BaseConnectionFixture::running = true;
        connection->state.stopped_hook = stopped_hook;
    }

    const char *BaseConnectionFixture::provide_login(amqp_context_t *context)
    {
        return "joe";
    }

    const char *BaseConnectionFixture::provide_password(amqp_context_t *context)
    {
        return "secret";
    }

    const char *BaseConnectionFixture::provide_email(amqp_context_t *context)
    {
        return "joe@s.gl";
    }

    void BaseConnectionFixture::connect_to(const char *hostname, int port_number)
    {
        BaseConnectionFixture::running = true;
        connection->state.connection.done = connection->state.connection.fail = connection->state.stopped_hook = stopped_hook;
        connection->state.socket.connect(connection, hostname, port_number);
    }

    BrokerUsingFixture::BrokerUsingFixture()
    {
        dummy_broker = amqp_dummy_broker_initialize(context, 54321, 0);
    }
    BrokerUsingFixture::BrokerUsingFixture(amqp_connection_test_hook_t *test_hooks)
    {
        dummy_broker = amqp_dummy_broker_initialize(context, 54321, test_hooks);
    }

    BrokerUsingFixture::~BrokerUsingFixture()
    {
        amqp_dummy_broker_destroy(context, dummy_broker);
    }

}
