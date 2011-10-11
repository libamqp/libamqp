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

#include "AmqpTypes/AmqpTypesTestSupport.h"

#include "AmqpTypes/AmqpSymbol.h"

SUITE(AmqpTypesSymbol)
{
    class AmqpSymbolFixture  : public SuiteAmqpTypes::AmqpTypesFixture
    {
    public:
        AmqpSymbolFixture();
        ~AmqpSymbolFixture();

        static const char *a;
        static const char *b;
        static const char *c;
        static const char *d;
        static const char *e;
    public:
        amqp_symbol_t ref;
        amqp_symbol_t *symbol_b;
    };

    const char *AmqpSymbolFixture::a = "ABC";
    const char *AmqpSymbolFixture::b = "ABC";
    const char *AmqpSymbolFixture::c = "DEFGH";
    const char *AmqpSymbolFixture::d = "IJKLM";
    const char *AmqpSymbolFixture::e = "DEFGHIJ";

    AmqpSymbolFixture::AmqpSymbolFixture() :  symbol_b(0)
    {
        memset(&ref, '\0', sizeof(amqp_symbol_t));
    }

    AmqpSymbolFixture::~AmqpSymbolFixture()
    {
        amqp_symbol_cleanup(context, &ref);
        amqp_symbol_cleanup(context, symbol_b);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_equality)
    {
        break_one();
        amqp_symbol_initialize(context, &ref, a, strlen(a));
        symbol_b = amqp_symbol_create(context, b, strlen(b));
        CHECK(amqp_symbol_compare(&ref, symbol_b) == 0);
        CHECK(amqp_symbol_compare(symbol_b, &ref) == 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_c_lt_d)
    {
        amqp_symbol_initialize(context, &ref, c, strlen(c));
        symbol_b = amqp_symbol_create(context, d, strlen(d));
        CHECK(amqp_symbol_compare(&ref, symbol_b) < 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_d_gt_c)
    {
        amqp_symbol_initialize(context, &ref, d, strlen(d));
        symbol_b = amqp_symbol_create(context, c, strlen(c));

        CHECK(amqp_symbol_compare(&ref, symbol_b) > 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_c_lt_e)
    {
        amqp_symbol_initialize(context, &ref, c, strlen(c));
        symbol_b = amqp_symbol_create(context, e, strlen(e));
        CHECK(amqp_symbol_compare(&ref, symbol_b) < 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_e_gt_c)
    {
        amqp_symbol_initialize(context, &ref, c, strlen(c));
        symbol_b = amqp_symbol_create(context, e, strlen(e));
        CHECK(amqp_symbol_compare(symbol_b, &ref) > 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_hash)
    {
         amqp_symbol_initialize(context, &ref, a, strlen(a));
         symbol_b = amqp_symbol_create(context, b, strlen(b));
        CHECK_EQUAL(amqp_symbol_hash(&ref), amqp_symbol_hash(symbol_b));
    }
}
