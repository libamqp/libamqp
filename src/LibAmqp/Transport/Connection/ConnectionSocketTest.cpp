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

#include "Transport/LowLevel/ListenerThread.h"
#include "Transport/DummyBroker/EchoServer.h"
#include "Transport/Connection/Connection.h"

#include "Transport/Connection/ConnectionSocketTestSupport.h"

#include "debug_helper.h"
SUITE(ConnectionSocket)
{
     TEST_FIXTURE(ConnectionSocketFixture, verify_fixture_cleans_resources)
    {
    }

    TEST_FIXTURE(ConnectionSocketFixture, connect_should_fail_on_lookup_with_dodgy_hostname)
    {
        connection->trace_flags = 0;
        amqp_output_to_buffer(context, message_buffer, sizeof(message_buffer));
        connect_to("unknown-dodgy-host.localdomain", 54321);
        CHECK(!amqp_connection_is(connection, AMQP_CONNECTION_RUNNING));
        CHECK_EQUAL("Failed", connection->state.socket.name);
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "client: error: eai_error(%d): state=Resolving; Failed to lookup a valid address for unknown-dodgy-host.localdomain:54321", EAI_NONAME);
        CHECK_EQUAL(buffer, message_buffer);
    }

    TEST_FIXTURE(ConnectionSocketFixture, connection_can_be_stopped)
    {
        connect_to("localhost", 54321);
        CHECK_EQUAL("Connecting", connection->state.socket.name);
        connection->state.socket.shutdown(connection);
        CHECK_EQUAL("Stopped", connection->state.socket.name);
    }

    TEST_FIXTURE(ConnectionSocketFixture, connect_to_invalid_port_number)
    {
//        connection->trace_flags = 1;
        connect_to("localhost", 54322);
        loop_while_running();
        CHECK_EQUAL("Failed", connection->state.socket.name);
    }

    TEST_FIXTURE(ConnectionSocketFixture, connect_to_good_address)
    {
        connect_to("localhost", 54321);
        loop_while_socket_state_is("Connecting");
        CHECK_EQUAL("Connected", connection->state.socket.name);
        connection->state.socket.shutdown(connection);
        CHECK_EQUAL("Stopped", connection->state.socket.name);
    }
}
