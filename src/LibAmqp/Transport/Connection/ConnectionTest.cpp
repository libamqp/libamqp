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

#include "Context/TestSupport/ContextHolder.h"
#include "Context/TestSupport/DecodeBufferHolder.h"
#include "Context/TestSupport/BufferHolder.h"
#include "Context/TestSupport/TypeHolder.h"
#include "Plugin/TestSupport/SaslPluginHolder.h"
#include "Transport/Connection/TestSupport/ConnectionHolder.h"
#include "Transport/Frame/TestSupport/FrameHolder.h"
#include "Transport/DummyBroker/TestSupport/DummyBrokerHolder.h"

#include "debug_helper.h"

SUITE(Connection)
{
    class ConnectionFixture : 
            public TestSupport::DummyBrokerHolder,
            public virtual TestSupport::ClientConnectionHolder,
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::DecodeBufferHolder,
            public virtual TestSupport::SaslPluginHolder,
            public virtual TestSupport::TypeHolder,
            public virtual TestSupport::FrameHolder
    {
    public:
        ConnectionFixture() : TestSupport::DummyBrokerHolder(0)
        {
            ConnectionHolder::running = true;
            connection->protocols = AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_AMQP;
        }
        ConnectionFixture(amqp_connection_test_hook_t *test_hooks) : TestSupport::DummyBrokerHolder(test_hooks) {}
        ~ConnectionFixture()
        {
            connection->trace_flags = 0;
        }
        void cleanup_connection();
    public:
    };

    void ConnectionFixture::cleanup_connection()
    {
        amqp_connection_shutdown(context, connection);
        while (amqp_connection_is_running(connection) && run_loop_with_timeout());
        CHECK(!amqp_connection_is(connection, AMQP_CONNECTION_RUNNING));
    }
/*
    TEST_FIXTURE(ConnectionFixture, fixture_should_balance_allocations)
    {
    }
    TEST_FIXTURE(ConnectionFixture, limits_should_be_initialized)
    {
        CHECK_EQUAL(AMQP_DEFAULT_MAX_FRAME_SIZE, connection->amqp.connection.limits.max_frame_size);
    }
*/
    TEST_FIXTURE(ConnectionFixture, connection_should_establish_socket_link)
    {
        CHECK_EQUAL("initialized", connection->state.writer.name);
        CHECK_EQUAL("initialized", connection->state.reader.name);
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);

        CHECK(amqp_connection_is(connection, AMQP_CONNECTION_RUNNING));
        loop_while_socket_state_is("connecting");
        CHECK(amqp_connection_is(connection, AMQP_CONNECTION_SOCKET_CONNECTED));

        break_one();
        amqp_connection_shutdown(context, connection);
        loop_while_running();
        CHECK(!amqp_connection_is(connection, AMQP_CONNECTION_RUNNING));
    }
/*


    TEST_FIXTURE(ConnectionFixture, connection_should_establish_sasl_tunnel)
    {
        connection->protocols = AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_AMQP;
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
        loop_while_socket_state_is("connecting");
        loop_while_connection_state_is("connecting_sasl");
        CHECK_EQUAL("connecting_amqp", connection->state.connection.name);
        amqp_connection_shutdown(context, connection);
        loop_while_running();
        CHECK_EQUAL("stopped", connection->state.connection.name);
    }

    TEST_FIXTURE(ConnectionFixture, connection_should_establish_amqp_tunnel)
    {
        connection->protocols = AMQP_PROTOCOL_AMQP;
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
        loop_while_socket_state_is("connecting");
        loop_while_connection_state_is("connecting_amqp");
        CHECK_EQUAL("amqp_tunnel_established", connection->state.connection.name);
        amqp_connection_shutdown(context, connection);
        loop_while_running();
        CHECK_EQUAL("stopped", connection->state.connection.name);
    }

    TEST_FIXTURE(ConnectionFixture, connection_should_establish_sasl_and_amqp_tunnels)
    {
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
        loop_while_socket_state_is("connecting");
        loop_while_connection_state_is("connecting_sasl");
        loop_while_connection_state_is("connecting_amqp");
        CHECK_EQUAL("amqp_tunnel_established", connection->state.connection.name);
        amqp_connection_shutdown(context, connection);
        loop_while_running();
    }

    TEST_FIXTURE(ConnectionFixture, connection_should_open_amqp_connection)
    {
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
        connection->trace_flags &= ~(AMQP_TRACE_CONNECTION_READER | AMQP_TRACE_CONNECTION_WRITER);
        loop_until_connection_state_is("amqp_tunnel_established");
        loop_until_connection_amqp_state_is("open_sent");
        loop_until_connection_amqp_state_is("opened");
        CHECK_EQUAL("opened", connection->state.amqp.name);
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
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
        loop_while_socket_state_is("connecting");
        loop_while_connection_state_is("connecting_sasl");
	loop_until_connection_state_is("stopped");
        CHECK_EQUAL("stopped", connection->state.connection.name);
        CHECK(connection->failure_flags & AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED);
    }

    TEST_FIXTURE(ConnectionHookFixture, connection_should_reject_amqp_version)
    {
        context->debug.level = 0;
        connection->protocols = AMQP_PROTOCOL_AMQP;
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
        loop_while_socket_state_is("connecting");
        loop_while_connection_state_is("connecting_amqp");
	loop_until_connection_state_is("stopped");
        CHECK_EQUAL("stopped", connection->state.connection.name);
        CHECK(connection->failure_flags & AMQP_CONNECTION_AMQP_NEGOTIATION_REJECTED);

    }

    TEST_FIXTURE(ConnectionFixture, shutdown_should_close_amqp_connection)
    {
        connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
        loop_until_connection_state_is("amqp_tunnel_established");
        loop_until_connection_amqp_state_is("opened");
        loop_until_connection_amqp_state_is("opened");
        CHECK_EQUAL("opened", connection->state.amqp.name);
        connection->trace_flags &= ~(AMQP_TRACE_CONNECTION_READER | AMQP_TRACE_CONNECTION_WRITER);
        connection->trace_flags = -1;

        SOUTS(connection->state.connection.name);
        amqp_connection_shutdown(context, connection);
        SOUTS(connection->state.connection.name);

        CHECK_EQUAL("close_sent", connection->state.amqp.name);

        loop_until_connection_amqp_state_is("closed");

        SOUTS(connection->state.amqp.name);
        connection->trace_flags = 0;
    }
    */
}
