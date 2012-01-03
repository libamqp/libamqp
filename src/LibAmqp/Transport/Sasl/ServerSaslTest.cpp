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
#include "Transport/Connection/Connection.h"
#include "Transport/Sasl/SaslTestSupport.h"
#include "Codec/CodecTestSupport.h"
#include "AmqpTypes/AmqpSymbol.h"
#include "AmqpTypes/AmqpMultiple.h"
#include "Plugin/SaslPlain/SaslPlain.h"
#include "Plugin/SaslAnonymous/SaslAnonymous.h"
#include "TestData/TestFrames.h"
#include "Transport/Frame/Frame.h"

#include "Transport/Sasl/ServerSasl.h"
#include "Transport/Sasl/ClientSasl.h"

#include "debug_helper.h"

#include "Transport/Frame/FrameTestSupport.h"

SUITE(Sasl)
{
    class ServerSaslFixture : public SuiteFrame::FrameFixture
    {
    public:
        ServerSaslFixture();
        ~ServerSaslFixture();

    public:
        amqp_symbol_t *symbol;
        amqp_multiple_symbol_t *multiple;
    };

    ServerSaslFixture::ServerSaslFixture() : symbol(0), multiple(0)
    {
    }

    ServerSaslFixture::~ServerSaslFixture()
    {
        amqp_symbol_cleanup(context, symbol);
        amqp_multiple_symbol_cleanup(context, multiple);
    }

    TEST_FIXTURE(ServerSaslFixture, encode_sasl_mechanisms)
    {
        test_data::sasl_mechanisms_frame.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);

        ASSERT(frame != 0);

        amqp_sasl_process_mechanisms_frame(connection, frame);
        amqp_buffer_t *buffer = connection->buffer.write;
        amqp_buffer_advance_read_index(buffer, 8);
        type = amqp_decode(context, buffer);
        ASSERT(type);
        CHECK(amqp_type_is_valid(type));
        CHECK(amqp_type_is_composite(type));
        amqp_type_t *described = amqp_type_get_described(type);
        CHECK(amqp_type_is_list(described));
        CHECK(amqp_type_is_symbol(amqp_type_list_element(described, 0)));
        CHECK(amqp_type_is_binary(amqp_type_list_element(described, 1)));
        CHECK(amqp_type_is_string(amqp_type_list_element(described, 2)));
    }
}
