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

#include "Transport/Connection/TestSupport/ConnectionHolder.h"
#include "Context/TestSupport/DecodeBufferHolder.h"
#include "Context/TestSupport/BufferHolder.h"
#include "Plugin/TestSupport/SaslPluginHolder.h"
#include "Transport/Connection/TestSupport/BufferReadInterceptor.h"
#include "Transport/Connection/TestSupport/WriteInterceptor.h"
#include "Context/TestSupport/TypeHolder.h"
#include "Transport/Frame/TestSupport/FrameHolder.h"

#include "Transport/Connection/Negotiator.h"

#include "debug_helper.h"

SUITE(ConnectionNegotiation)
{
    class NegotiationTestFixture :
            public TestSupport::UnconnectedConnectionHolder,
            public virtual TestSupport::BufferHolder,
            public virtual TestSupport::WriteInterceptor,
            public virtual TestSupport::BufferReadInterceptor
    {
    public:
        NegotiationTestFixture();
        ~NegotiationTestFixture();

        static void done_callback(amqp_connection_t *connection);
        static void reject_callback(amqp_connection_t *connection, uint32_t version);
        static void request_callback(amqp_connection_t *connection, uint32_t version);

    public:
        static uint32_t broker_version;
    };

    uint32_t NegotiationTestFixture::broker_version = AMQP_PREFERRED_VERSION;

    NegotiationTestFixture::NegotiationTestFixture()
    {
    }

    NegotiationTestFixture::~NegotiationTestFixture()
    {
    }

    TEST_FIXTURE(NegotiationTestFixture, fixture_should_balance_allocations)
    {
        CHECK_EQUAL("initialized", connection->state.negotiator.name);
    }

    TEST_FIXTURE(NegotiationTestFixture, encode_prefered_version)
    {
        amqp_negotiator_encode_version(context, AMQP_PREFERRED_VERSION, buffer);

        CHECK_EQUAL('A', amqp_buffer_getc(buffer));
        CHECK_EQUAL('M', amqp_buffer_getc(buffer));
        CHECK_EQUAL('Q', amqp_buffer_getc(buffer));
        CHECK_EQUAL('P', amqp_buffer_getc(buffer));
        CHECK_EQUAL(AMQP_PROTOCOL_ID, amqp_buffer_getc(buffer));
        CHECK_EQUAL(AMQP_PROTOCOL_MAJOR, amqp_buffer_getc(buffer));
        CHECK_EQUAL(AMQP_PROTOCOL_MINOR, amqp_buffer_getc(buffer));
        CHECK_EQUAL(AMQP_PROTOCOL_REVISION, amqp_buffer_getc(buffer));
    }

    TEST_FIXTURE(NegotiationTestFixture, decode_prefered_version)
    {
        amqp_buffer_putc(buffer, 'A');
        amqp_buffer_putc(buffer, 'M');
        amqp_buffer_putc(buffer, 'Q');
        amqp_buffer_putc(buffer, 'P');
        amqp_buffer_putc(buffer, AMQP_PROTOCOL_ID);
        amqp_buffer_putc(buffer, AMQP_PROTOCOL_MAJOR);
        amqp_buffer_putc(buffer, AMQP_PROTOCOL_MINOR);
        amqp_buffer_putc(buffer, AMQP_PROTOCOL_REVISION);

        CHECK_EQUAL((uint32_t) AMQP_PREFERRED_VERSION, amqp_negotiator_decode_version(context, buffer));
    }

    void NegotiationTestFixture::done_callback(amqp_connection_t *connection)
    {
    }
    void NegotiationTestFixture::reject_callback(amqp_connection_t *connection, uint32_t version)
    {
    }

    TEST_FIXTURE(NegotiationTestFixture, client_offer)
    {
        amqp_negotiator_encode_version(connection->context, NegotiationTestFixture::broker_version, buffer);
        set_data_for_read(buffer);

        connection->state.negotiator.start(connection, AMQP_PREFERRED_VERSION, NegotiationTestFixture::done_callback, NegotiationTestFixture::reject_callback);
        CHECK_EQUAL("negotiated", connection->state.negotiator.name);
    }

    void NegotiationTestFixture::request_callback(amqp_connection_t *connection, uint32_t version)
    {
    }

    TEST_FIXTURE(NegotiationTestFixture, broker_response)
    {
        amqp_negotiator_encode_version(connection->context, NegotiationTestFixture::broker_version, buffer);
        set_data_for_read(buffer);

        connection->state.negotiator.wait(connection, request_callback);
        CHECK_EQUAL("waiting_to_send_server_response", connection->state.negotiator.name);
    }
}
