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

SUITE(AmqpTypes)
{
    class AmqpSymbolMapFixture  : public AmqpTypesFixture
    {
    public:
        AmqpSymbolMapFixture();
        ~AmqpSymbolMapFixture();

    public:
        amqp_map_t map;
    };

    AmqpSymbolMapFixture::AmqpSymbolMapFixture()
    {
    }

    AmqpSymbolMapFixture::~AmqpSymbolMapFixture()
    {
    }

    TEST_FIXTURE(AmqpSymbolMapFixture, symbol_map_should_not_trigger_allocation_errors)
    {
        amqp_symbol_map_initialize(context, &map, 63);
        for (int i = 0; i < key_count(); i++)
        {
            amqp_symbol_t *symbol = amqp_symbol_create(context, keys[i], strlen(keys[i]));
            amqp_map_put(context, &map, symbol, data[i]);
        }
        amqp_symbol_map_cleanup(context, &map);
    }

    TEST_FIXTURE(AmqpSymbolMapFixture, symbol_map_put_and_get_are_nothing_special)
    {
        amqp_symbol_map_initialize(context, &map, 63);
        amqp_symbol_t *symbol = amqp_symbol_create(context, keys[6], strlen(keys[6]));
        CHECK(amqp_map_put(context, &map, symbol, data[6]));
        CHECK(amqp_map_put(context, &map, symbol, data[6]) == 0);
        amqp_symbol_map_cleanup(context, &map);
    }
}
