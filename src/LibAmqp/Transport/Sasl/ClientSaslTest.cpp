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
#include "Transport/Connection/Connection.h"
#include "Transport/Sasl/SaslTestSupport.h"
#include "Codec/CodecTestSupport.h"
#include "AmqpTypes/AmqpSymbol.h"
#include "AmqpTypes/AmqpMultiple.h"
#include "Plugin/SaslPlain/SaslPlain.h"
#include "Plugin/SaslAnonymous/SaslAnonymous.h"
#include "TestData/TestFrames.h"
#include "Transport/Frame/Frame.h"

#include "Transport/Sasl/ClientSasl.h"

#include "debug_helper.h"

#include "Transport/Frame/FrameTestSupport.h"

SUITE(Sasl)
{
    class ClientSaslFixture : public SuiteFrame::FrameFixture
    {
    public:
        ClientSaslFixture();
        ~ClientSaslFixture();

    public:
        amqp_symbol_t *symbol;
        amqp_multiple_symbol_t *multiple;
    };

    ClientSaslFixture::ClientSaslFixture() : symbol(0), multiple(0)
    {
    }

    ClientSaslFixture::~ClientSaslFixture()
    {
        amqp_symbol_cleanup(context, symbol);
        amqp_multiple_symbol_cleanup(context, multiple);
    }

    TEST_FIXTURE(ClientSaslFixture, encode_sasl_init)
    {
        test_data::sasl_mechanisms_frame.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame != 0);

        break_one();

        amqp_sasl_process_mechanisms_frame(connection, frame);
    
        CHECK(0);
    }
}
