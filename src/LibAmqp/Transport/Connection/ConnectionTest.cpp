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

#include "debug_helper.h"

SUITE(Connection)
{
    class ConnectionFixture : public BrokerUsingFixture
    {
    public:
        ConnectionFixture();
        ConnectionFixture(amqp_connection_test_hook_t *test_hooks);
        ~ConnectionFixture();

        void cleanup_connection();

    public:
    };

    ConnectionFixture::ConnectionFixture()
    {
        connection = amqp_connection_create(context);
        setup_stop_hook();
    }
    ConnectionFixture::ConnectionFixture(amqp_connection_test_hook_t *test_hooks) : BrokerUsingFixture(test_hooks)
    {
        connection = amqp_connection_create(context);
        setup_stop_hook();
    }

    ConnectionFixture::~ConnectionFixture()
    {
        amqp_connection_destroy(context, connection);
    }

    void ConnectionFixture::cleanup_connection()
    {
        amqp_connection_shutdown(context, connection);
        while (amqp_connection_is_running(connection) && run_loop_with_timeout());

        CHECK(!amqp_connection_is(connection, AMQP_CONNECTION_RUNNING));
    }

    TEST_FIXTURE(ConnectionFixture, fixture_should_balance_allocations)
    {
    }

    TEST_FIXTURE(ConnectionFixture, limits_should_be_initialized)
    {
        CHECK_EQUAL(AMQP_DEFAULT_MAX_FRAME_SIZE, connection->limits.max_frame_size);
    }

    TEST_FIXTURE(ConnectionFixture, connection_should_establish_socket_link)
    {
        CHECK_EQUAL("Initialized", connection->state.writer.name);
        CHECK_EQUAL("Initialized", connection->state.reader.name);
        connection->state.connection.mode.client.connect(connection, "localhost", 54321);

        CHECK(amqp_connection_is(connection, AMQP_CONNECTION_RUNNING));
        loop_while_socket_state_is("Connecting");
        CHECK(amqp_connection_is(connection, AMQP_CONNECTION_SOCKET_CONNECTED));

        amqp_connection_shutdown(context, connection);
        loop_while_running();
        CHECK(!amqp_connection_is(connection, AMQP_CONNECTION_RUNNING));
    }

    class ConnectionHookFixture : public ConnectionFixture
    {
    public:
        ConnectionHookFixture() : ConnectionFixture(ConnectionHookFixture::hooks()) { }
        ~ConnectionHookFixture() { }

        static void post_initialise_hook(amqp_connection_t *connection);
        static amqp_connection_test_hook_t *hooks();
    public:
    };
    void ConnectionHookFixture::post_initialise_hook(amqp_connection_t *connection)
    {
        connection->context->debug.level = 0;
        connection->specification_version.required.sasl = AMQP_PROTOCOL_VERSION(AMQP_SASL_PROTOCOL_ID, 2, 0, 0);
        connection->specification_version.required.amqp = AMQP_PROTOCOL_VERSION(AMQP_PROTOCOL_ID, 1, 1, 0);
    }
    amqp_connection_test_hook_t *ConnectionHookFixture::hooks()
    {
        static amqp_connection_test_hook_t hooks = {
            ConnectionHookFixture::post_initialise_hook
        };
        return &hooks;
    }

    TEST_FIXTURE(ConnectionHookFixture, connection_should_reject_sasl_version)
    {
        context->debug.level = 0;
        connection->protocols = AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_AMQP;
        connection->state.connection.mode.client.connect(connection, "localhost", 54321);
        loop_while_socket_state_is("Connecting");
        loop_while_connection_state_is("ConnectingSasl");
        CHECK_EQUAL("Stopped", connection->state.connection.name); // TODO - Should it be Failed

        CHECK(connection->failure_flags & AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED);
    }

    TEST_FIXTURE(ConnectionHookFixture, connection_should_reject_amqp_version)
    {
        context->debug.level = 0;
        connection->protocols = AMQP_PROTOCOL_AMQP;
        connection->state.connection.mode.client.connect(connection, "localhost", 54321);
        loop_while_socket_state_is("Connecting");
        loop_while_connection_state_is("ConnectingAmqp");
        CHECK_EQUAL("Stopped", connection->state.connection.name); // TODO - Should it be Failed

        CHECK(connection->failure_flags & AMQP_CONNECTION_AMQP_NEGOTIATION_REJECTED);
    }

    TEST_FIXTURE(ConnectionFixture, connection_should_establish_sasl_tunnel)
    {
        connection->protocols = AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_AMQP;
        connection->state.connection.mode.client.connect(connection, "localhost", 54321);
        loop_while_socket_state_is("Connecting");
        loop_while_connection_state_is("ConnectingSasl");
        CHECK_EQUAL("ConnectingSasl", connection->state.connection.name);
        amqp_connection_shutdown(context, connection);
        loop_while_running();
        CHECK_EQUAL("Stopped", connection->state.connection.name);
    }

//    TEST_FIXTURE(ConnectionFixture, connection_should_establish_amqp_tunnel)
//    {
//        connection->protocols = AMQP_PROTOCOL_AMQP;
//        connection->state.connection.mode.client.connect(connection, "localhost", 54321);
//        loop_while_socket_state_is("Connecting");
//        loop_while_connection_state_is("ConnectingAmqp");
//        CHECK_EQUAL("Connected", connection->state.connection.name);
//        amqp_connection_shutdown(context, connection);
//        loop_while_running();
//        CHECK_EQUAL("Stopped", connection->state.connection.name);
//    }

//    TEST_FIXTURE(ConnectionFixture, connection_should_establish_sasl_and_amqp_tunnels)
//    {
//        connection->state.connection.mode.client.connect(connection, "localhost", 54321);
//        loop_while_socket_state_is("Connecting");
//        loop_while_connection_state_is("ConnectingSasl");
//        loop_while_connection_state_is("ConnectingAmqp");
//        CHECK_EQUAL("Connected", connection->state.connection.name);
//        amqp_connection_shutdown(context, connection);
//        loop_while_running();
//    }
}
