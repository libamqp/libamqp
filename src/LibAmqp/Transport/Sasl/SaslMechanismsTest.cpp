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
#include "AmqpTypes/AmqpMultiple.h"
#include "Plugin/SaslPlain/SaslPlain.h"
#include "Plugin/SaslAnonymous/SaslAnonymous.h"

#include "debug_helper.h"

SUITE(Sasl)
{
    class SaslMechanismsFixture : public SuiteSasl::BaseSaslFixture
    {
    public:
        SaslMechanismsFixture();
        ~SaslMechanismsFixture();

    public:
        amqp_symbol_t *symbol;
        amqp_multiple_symbol_t *multiple;
    };

    SaslMechanismsFixture::SaslMechanismsFixture() : symbol(0), multiple(0)
    {
        amqp_context_register_sasl_plugin(context, amqp_plugin_sasl_plain_create(context));
    }

    SaslMechanismsFixture::~SaslMechanismsFixture()
    {
        amqp_symbol_cleanup(context, symbol);
        amqp_multiple_symbol_cleanup(context, multiple);
    }

    TEST_FIXTURE(SaslMechanismsFixture, encode_sasl_mechanisms_plain_only)
    {
        type = amqp_sasl_mechanisms_encode(context, buffer);
        ASSERT(type);
        CHECK(amqp_type_is_symbol(type));

        symbol = amqp_symbol_create_from_type(context, type);
        CHECK(amqp_symbol_compare_with_cstr(symbol, "PLAIN") == 0);
    }

    TEST_FIXTURE(SaslMechanismsFixture, encode_sasl_mechanisms_plain_as_multiple)
    {
        type = amqp_sasl_mechanisms_encode(context, buffer);
        ASSERT(type);
        CHECK(amqp_type_is_symbol(type));

        multiple = amqp_multiple_symbol_create_from_type(context, type);
        CHECK_EQUAL(1, amqp_multiple_symbol_size(multiple));
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "PLAIN") == 0);
    }

    TEST_FIXTURE(SaslMechanismsFixture, encode_sasl_mechanisms_plain_and_anonymous_as_multiple)
    {
        amqp_context_register_sasl_plugin(context, amqp_plugin_sasl_anonymous_create(context));

        type = amqp_sasl_mechanisms_encode(context, buffer);
        ASSERT(type);

        CHECK(amqp_type_is_array(type));

        multiple = amqp_multiple_symbol_create_from_type(context, type);
        CHECK_EQUAL(2, amqp_multiple_symbol_size(multiple));
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "PLAIN") == 0);
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 1), "ANONYMOUS") == 0);
    }
}
