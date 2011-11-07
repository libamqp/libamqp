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

#include "Transport/Connection/ConnectionFrameTestSupport.h"
#include "TestData/TestFrames.h"

#include "debug_helper.h"
SUITE(ConnectionSasl)
{
    class ConnectionSaslFixture : public SuiteConnectionFrame::ConnectionFrameFixture
    {
    public:
        ConnectionSaslFixture();
        ~ConnectionSaslFixture();

    public:
    };


    ConnectionSaslFixture::ConnectionSaslFixture()
    {
        connection->socket.hostname = "localhost.localdomain";
    }

    ConnectionSaslFixture::~ConnectionSaslFixture()
    {
        connection->socket.hostname = 0;
    }
/*
    TEST_FIXTURE(ConnectionSaslFixture, fixture_should_balance_allocations)
    {
        CHECK(connection->state.frame.name != 0);
        CHECK_EQUAL("Initialized", connection->state.sasl.name);
    }

    TEST_FIXTURE(ConnectionSaslFixture, sasl_version_rejection)
    {
        set_test_data_for_read(test_data::sasl_protocol_1_1_0);
        connection->state.sasl.connect(connection);

        CHECK_EQUAL("NegotiationRejected", connection->state.negotiator.name);
        CHECK_EQUAL("Failed", connection->state.connection.name);

        CHECK_BUFFERS_MATCH(write_copy, test_data::sasl_protocol_1_0_0);
    }
*/
    TEST_FIXTURE(ConnectionSaslFixture, sasl_version_accepted)
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

        CHECK_EQUAL("Negotiated", connection->state.negotiator.name);
        CHECK_EQUAL("Negotiated", connection->state.sasl.name);
    }

//    TEST_FIXTURE(ConnectionFrameFixture, stop_while_stopped)
//    {
//        connection->state.frame.enable(connection);
//        connection->state.frame.stop(connection);
//        CHECK_EQUAL("Stopped", connection->state.frame.name);
//
//        connection->state.frame.stop(connection);
//        CHECK_EQUAL("Stopped", connection->state.frame.name);
//    }
//
//    TEST_FIXTURE(ConnectionFrameFixture, read_minimal_frame)
//    {
//        ConnectionFrameFixture::test_data_p = &test_data::minimal_frame_header;
//        connection->state.frame.enable(connection);
//        connection->state.frame.read(connection, buffer, 8, done_callback);
//        CHECK_EQUAL("Enabled", connection->state.frame.name);
//        CHECK_EQUAL(8U, connection->io.frame.frame_size);
//    }
//
//    TEST_FIXTURE(ConnectionFrameFixture, read_frame)
//    {
//        ConnectionFrameFixture::test_data_p = &test_data::sasl_mechanisms_frame;
//        connection->state.frame.enable(connection);
//        connection->state.frame.read(connection, buffer, 8, done_callback);
//        CHECK_EQUAL("Enabled", connection->state.frame.name);
//        CHECK_EQUAL(0x15U, connection->io.frame.frame_size);
//    }
}
