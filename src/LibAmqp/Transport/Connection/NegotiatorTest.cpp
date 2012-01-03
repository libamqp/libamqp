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

#include "Transport/Connection/ConnectionTestSupport.h"
#include "Transport/Connection/Negotiator.h"

#include "debug_helper.h"

SUITE(ConnectionNegotiation)
{
    class NegotiationFixture : public SuiteConnection::BaseConnectionFixture
    {
    public:
        NegotiationFixture();
        ~NegotiationFixture();

        static void write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback);
        static void read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f done_callback);

        static void done_callback(amqp_connection_t *connection);
        static void reject_callback(amqp_connection_t *connection, uint32_t version);
        static void request_callback(amqp_connection_t *connection, uint32_t version);

    public:
        static uint32_t broker_version;
        amqp_buffer_t *buffer;
    };

    uint32_t NegotiationFixture::broker_version = AMQP_PREFERRED_VERSION;

    NegotiationFixture::NegotiationFixture()
    {
        connection = amqp_connection_create(context);
        buffer = amqp_allocate_buffer(context);
    }

    NegotiationFixture::~NegotiationFixture()
    {
        amqp_deallocate_buffer(context, buffer);
        amqp_connection_destroy(context, connection);
    }

    void NegotiationFixture::write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback)
    {
        if (done_callback)
        {
            done_callback(connection);
        }
    }
    void NegotiationFixture::read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f done_callback)
    {
        amqp_negotiator_encode_version(connection->context, NegotiationFixture::broker_version, buffer);
        if (done_callback)
        {
            if (buffer == 0)
            {
                amqp_allocate_buffer(connection->context);
            }
            done_callback(connection, buffer, amqp_buffer_size(buffer));
        }
    }

    TEST_FIXTURE(NegotiationFixture, fixture_should_balance_allocations)
    {
        CHECK_EQUAL("Initialized", connection->state.negotiator.name);
    }

    TEST_FIXTURE(NegotiationFixture, encode_prefered_version)
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

    TEST_FIXTURE(NegotiationFixture, decode_prefered_version)
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

    void NegotiationFixture::done_callback(amqp_connection_t *connection)
    {
    }
    void NegotiationFixture::reject_callback(amqp_connection_t *connection, uint32_t version)
    {
    }
    TEST_FIXTURE(NegotiationFixture, client_offer)
    {
        connection->state.writer.commence_write = write_intercept;
        connection->state.reader.commence_read = read_intercept;

        connection->state.negotiator.start(connection, AMQP_PREFERRED_VERSION, NegotiationFixture::done_callback, NegotiationFixture::reject_callback);
        CHECK_EQUAL("Negotiated", connection->state.negotiator.name);
    }

    void NegotiationFixture::request_callback(amqp_connection_t *connection, uint32_t version)
    {
    }
    TEST_FIXTURE(NegotiationFixture, broker_response)
    {
        connection->state.writer.commence_write = write_intercept;
        connection->state.reader.commence_read = read_intercept;

        connection->state.negotiator.wait(connection, request_callback);
        CHECK_EQUAL("WaitToSendServerResponse", connection->state.negotiator.name);
    }
}
