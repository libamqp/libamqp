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
#include "Transport/Connection/TestSupport/ConnectionHolder.h"
#include "Transport/Connection/TestSupport/WriteInterceptor.h"
#include "Transport/Connection/TestSupport/ReadInterceptor.h"

#include "Context/TestSupport/TestSupport.h"
#include "TestData/TestFrames.h"

#include "debug_helper.h"

SUITE(ConnectionSasl)
{
    class ConnectionSaslFixture :
            public virtual TestSupport::ClientConnectionHolder,
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::WriteInterceptor,
            public virtual TestSupport::ReadInterceptor
    {
    public:
        ConnectionSaslFixture();
        ~ConnectionSaslFixture();

        static void done_callback(amqp_connection_t *connection);
        static void done_callback(amqp_connection_t *connection, amqp_buffer_t *buffer);
    };

    void ConnectionSaslFixture::done_callback(amqp_connection_t *connection)
    {
    }

    void ConnectionSaslFixture::done_callback(amqp_connection_t *connection, amqp_buffer_t *buffer)
    {
    }

    ConnectionSaslFixture::ConnectionSaslFixture()
    {
        connection->socket.hostname = "localhost.localdomain";
    }

    ConnectionSaslFixture::~ConnectionSaslFixture()
    {
        connection->socket.hostname = 0;
    }

    TEST_FIXTURE(ConnectionSaslFixture, fixture_should_balance_allocations)
    {
        CHECK(connection->state.frame.name != 0);
        CHECK_EQUAL("initialized", connection->state.sasl.name);
    }

    TEST_FIXTURE(ConnectionSaslFixture, sasl_version_rejection)
    {
        set_test_data_for_read(test_data::sasl_protocol_1_1_0);
        connection->state.sasl.connect(connection);

        CHECK_BUFFERS_MATCH(intercepted_write(), test_data::sasl_protocol_1_0_0);
        CHECK_EQUAL("rejected", connection->state.negotiator.name);
        CHECK_EQUAL("failed", connection->state.sasl.name);

    }

    TEST_FIXTURE(ConnectionSaslFixture, sasl_client_side_handshake)
    {
        static test_data::TestData *test_frames[] =
        {
            &test_data::sasl_protocol_1_0_0,
            &test_data::sasl_mechanisms_frame,
            &test_data::sasl_outcome_frame,
            0
        };

        set_test_data_for_read(test_frames);

        connection->state.sasl.connect(connection);

        CHECK_EQUAL("negotiated", connection->state.negotiator.name);
        CHECK_EQUAL("negotiated", connection->state.sasl.name);
    }

    static void dummy_send_action(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback)
    {
        done_callback(connection);
    }

    TEST_FIXTURE(ConnectionSaslFixture, sasl_server_side_handshake)
    {
        static test_data::TestData *test_frames[] =
        {
            &test_data::sasl_init_frame,
            0
        };

        amqp_connection_accepted_socket_initialize(connection, -1, 0, 0);
        connection->state.negotiator.send = dummy_send_action;
        set_test_data_for_read(test_frames);
        connection->state.sasl.tunnel.accept(connection, AMQP_SASL_PREFERRED_VERSION);
        CHECK_EQUAL("negotiated", connection->state.sasl.name);
    }
}
