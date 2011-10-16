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
#include "Transport/Sasl/SaslTestSupport.h"
#include "Codec/CodecTestSupport.h"
#include "AmqpTypes/AmqpSymbol.h"


SUITE(Sasl)
{
    class SaslMechanismsFixture : public SuiteSasl::BaseSaslFixture
    {
    public:
        SaslMechanismsFixture();
        ~SaslMechanismsFixture();

    public:
        amqp_symbol_t *symbol;
    };

    SaslMechanismsFixture::SaslMechanismsFixture() : symbol(0)
    {
    }

    SaslMechanismsFixture::~SaslMechanismsFixture()
    {
        amqp_symbol_cleanup(context, symbol);
    }

    TEST_FIXTURE(SaslMechanismsFixture, really_important_test)
    {
        type = amqp_sasl_mechanisms_encode(context, buffer);
        ASSERT(type);
        CHECK(amqp_type_is_symbol(type));

        symbol = amqp_symbol_create_from_type(context, type);
        CHECK(amqp_symbol_compare_with_cstr(symbol, "PLAIN") == 0);

    }
}
