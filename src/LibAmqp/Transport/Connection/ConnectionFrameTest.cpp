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
SUITE(ConnectionFrame)
{
    TEST_FIXTURE(ConnectionFrameFixture, fixture_should_balance_allocations)
    {
        CHECK(connection->state.frame.name != 0);
        CHECK_EQUAL("Initialized", connection->state.frame.name);
    }

    TEST_FIXTURE(ConnectionFrameFixture, enable_frame)
    {
        connection->state.frame.enable(connection);
        CHECK_EQUAL("Enabled", connection->state.frame.name);

        connection->state.frame.stop(connection);
        CHECK_EQUAL("Stopped", connection->state.frame.name);
    }

    TEST_FIXTURE(ConnectionFrameFixture, stop_while_stopped)
    {
        connection->state.frame.enable(connection);
        connection->state.frame.stop(connection);
        CHECK_EQUAL("Stopped", connection->state.frame.name);

        connection->state.frame.stop(connection);
        CHECK_EQUAL("Stopped", connection->state.frame.name);
    }

    TEST_FIXTURE(ConnectionFrameFixture, read_minimal_frame)
    {
        ConnectionFrameFixture::test_data_p = &test_data::minimal_frame_header;
        connection->state.frame.enable(connection);
        connection->state.frame.read(connection, buffer, 8, done_callback);
        CHECK_EQUAL("Enabled", connection->state.frame.name);
        CHECK_EQUAL(8U, connection->io.frame.frame_size);
    }

    TEST_FIXTURE(ConnectionFrameFixture, read_frame)
    {
        ConnectionFrameFixture::test_data_p = &test_data::sasl_mechanisms_frame;
        connection->state.frame.enable(connection);
        connection->state.frame.read(connection, buffer, 8, done_callback);
        CHECK_EQUAL("Enabled", connection->state.frame.name);
        CHECK_EQUAL(0x15U, connection->io.frame.frame_size);
    }
}
