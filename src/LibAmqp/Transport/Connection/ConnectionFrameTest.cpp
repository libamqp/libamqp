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
SUITE(ConnectionFrame)
{
    class ConnectionFrameFixture : public SuiteConnection::BaseConnectionFixture
    {
    public:
        ConnectionFrameFixture();
        ~ConnectionFrameFixture();

        static void write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback);
        static void read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f done_callback);
        static void hangup_intercept(amqp_connection_t *connection);

        static void done_callback(amqp_connection_t *connection);
    public:
        amqp_buffer_t *buffer;
    };

    ConnectionFrameFixture::ConnectionFrameFixture()
    {
        buffer = amqp_allocate_buffer(context);

        connection = amqp_connection_initialize(context);
        connection->state.writer.commence_write = write_intercept;
        connection->state.reader.commence_read = read_intercept;
    }

    ConnectionFrameFixture::~ConnectionFrameFixture()
    {
        amqp_connection_destroy(context, connection); // TODO - intercept hangup

        amqp_deallocate_buffer(context, buffer);
    }

    void ConnectionFrameFixture::write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback)
    {
        if (done_callback)
        {
            done_callback(connection);
        }
    }
    void ConnectionFrameFixture::read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f done_callback)
    {
        if (done_callback)
        {
            done_callback(connection, amqp_buffer_size(buffer));
        }
    }

    void ConnectionFrameFixture::hangup_intercept(amqp_connection_t *connection)
    {
        SOUTS("hangup called");
    }  
    
    void ConnectionFrameFixture::done_callback(amqp_connection_t *connection)
    {
    }

    TEST_FIXTURE(ConnectionFrameFixture, fixture_should_balance_allocations)
    {
        CHECK(connection->state.frame.name != 0);
        CHECK_EQUAL("Initialized", connection->state.frame.name);
    }

//    TEST_FIXTURE(ConnectionFrameFixture, client_offer)
//    {
//        connection->state.writer.commence_write = write_intercept;
//        connection->state.reader.commence_read = read_intercept;
//
//        connection->state.negotiator.start(connection, AMQP_PREFERRED_VERSION, ConnectionFrameFixture::done_callback, ConnectionFrameFixture::reject_callback);
//        CHECK_EQUAL("Negotiated", connection->state.negotiator.name);
//    }
//
//    }
//    TEST_FIXTURE(ConnectionFrameFixture, broker_response)
//    {
//        connection->state.writer.commence_write = write_intercept;
//        connection->state.reader.commence_read = read_intercept;
//
//        connection->state.negotiator.wait(connection, request_callback);
//        CHECK_EQUAL("WaitToSendServerResponse", connection->state.negotiator.name);
//    }

}
