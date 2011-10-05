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
#include "Context/ContextTestSupport.h"

#include "AmqpTypes/AmqpSymbol.h"

SUITE(AmqpTypes)
{
    class AmqpSymbolFixture  : public SuiteContext::ContextFixture
    {
    public:
        AmqpSymbolFixture();
        ~AmqpSymbolFixture();

        amqp_symbol_t *initialize(const char *s);
        void initialize(const char *lhs, const char *rhs);

        static const char *a;
        static const char *b;
        static const char *c;
        static const char *d;
        static const char *e;
    public:
        amqp_symbol_t *symbol_a;
        amqp_symbol_t *symbol_b;
    };

    const char *AmqpSymbolFixture::a = "ABC";
    const char *AmqpSymbolFixture::b = "ABC";
    const char *AmqpSymbolFixture::c = "DEFGH";
    const char *AmqpSymbolFixture::d = "IJKLM";
    const char *AmqpSymbolFixture::e = "DEFGHIJ";

    AmqpSymbolFixture::AmqpSymbolFixture() : symbol_a(0), symbol_b(0)
    {
    }

    AmqpSymbolFixture::~AmqpSymbolFixture()
    {
        amqp_symbol_cleanup(context, symbol_b);
        amqp_symbol_cleanup(context, symbol_a);
    }

    amqp_symbol_t *AmqpSymbolFixture::initialize(const char *s)
    {
        return amqp_symbol_create(context, 0, (const unsigned char *) s, strlen(s));
    }
    void AmqpSymbolFixture::initialize(const char *lhs, const char *rhs)
    {
        symbol_a = initialize(lhs);
        symbol_b = initialize(rhs);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_equality)
    {
        initialize(a, b);
        CHECK(amqp_symbol_compare(symbol_a, symbol_b) == 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_c_lt_d)
    {
        initialize(c, d);
        CHECK(amqp_symbol_compare(symbol_a, symbol_b) < 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_d_gt_c)
    {
        initialize(d, c);
        CHECK(amqp_symbol_compare(symbol_a, symbol_b) > 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_c_lt_e)
    {
        initialize(c, e);
        CHECK(amqp_symbol_compare(symbol_a, symbol_b) < 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_e_gt_c)
    {
        initialize(e, c);
        CHECK(amqp_symbol_compare(symbol_a, symbol_b) > 0);
    }

    TEST_FIXTURE(AmqpSymbolFixture, symbol_hash)
    {
        initialize(a, b);
        CHECK_EQUAL(amqp_symbol_hash(symbol_a), amqp_symbol_hash(symbol_b));
    }
}
