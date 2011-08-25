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
#include "Transport/DummyBroker/DummyBroker.h"
#include "Context/ContextTestSupport.h"
#include "Transport/Connection.h"

#include "debug_helper.h"

SUITE(Transport)
{
    class ConnectionFixture : public SuiteContext::ContextFixture
    {
    public:
        ConnectionFixture();
        ~ConnectionFixture();

    public:
        amqp_event_loop_t *loop;
        amqp_dummy_broker_t *broker;
        amqp_connection_t *connection;
    };

    ConnectionFixture::ConnectionFixture()
    {
        loop = ev_default_loop(0);
        context->loop = loop;
        broker = amqp_dummy_broker_initialize(context, 54321, 0);
        connection = amqp_connection_initialize(context);
    }

    ConnectionFixture::~ConnectionFixture()
    {
        amqp_connection_destroy(context, connection);
        amqp_dummy_broker_destroy(context, broker);
    }

    TEST_FIXTURE(ConnectionFixture, connect_should_fail_on_lookup_with_dodgy_hostname)
    {
        amqp_connection_connect(context, connection, "unknown-dodgy-host.localdomain", 54321);
        CHECK(!connection->running);
        CHECK_EQUAL("Lookup failed", connection->state.name);
    }

    TEST_FIXTURE(ConnectionFixture, connection_can_be_stopped)
    {
        amqp_connection_connect(context, connection, "localhost", 54321);
        CHECK(connection->running);
        CHECK_EQUAL("Connecting", connection->state.name);
        connection->state.shutdown(context, connection);
        CHECK_EQUAL("Stopped", connection->state.name);
    }

    TEST_FIXTURE(ConnectionFixture, connect_to_invalid_address)
    {
        amqp_connection_connect(context, connection, "localhost", 54322);
        CHECK(connection->running);
        while (connection->running)
        {
            ev_run(context->loop, EVRUN_ONCE);
        }
        CHECK_EQUAL("Connect failed", connection->state.name);
    }

    TEST_FIXTURE(ConnectionFixture, connect_to_good_address)
    {
        amqp_connection_connect(context, connection, "localhost", 54321);
        CHECK(connection->running);
        while (connection->running && !amqp_connection_is_state(connection, "Connected"))
        {
            ev_run(context->loop, EVRUN_ONCE);
        }
        CHECK_EQUAL("Connected", connection->state.name);
        connection->state.shutdown(context, connection);
        CHECK_EQUAL("Stopped", connection->state.name);
    }
}
