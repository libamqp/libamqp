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

    public:
        amqp_symbol_t symbol_a;
        amqp_symbol_t symbol_b;
    };

    AmqpSymbolFixture::AmqpSymbolFixture()
    {
        memset(&symbol_a, '\0', sizeof(amqp_symbol_t));
        memset(&symbol_b, '\0', sizeof(amqp_symbol_t));
    }

    AmqpSymbolFixture::~AmqpSymbolFixture()
    {
        amqp_symbol_cleanup(&symbol_b);
        amqp_symbol_cleanup(&symbol_a);
    }

    TEST_FIXTURE(AmqpSymbolFixture, fixture_should_balance_allocations)
    {
//        CHECK_EQUAL(256U, amqp_map_capacity(&map));
//        CHECK_CLOSE(0.0, amqp_map_factor(&map), 0.001);
    }


}
