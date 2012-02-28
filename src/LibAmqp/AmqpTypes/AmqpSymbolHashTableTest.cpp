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

#include "AmqpTypes/AmqpTypesTestFixture.h"
#include "AmqpTypes/AmqpSymbol.h"

SUITE(AmqpTypes)
{
    class AmqpSymbolMapTestFixture  : public AmqpTypesTestFixture
    {
    public:
        AmqpSymbolMapTestFixture();
        ~AmqpSymbolMapTestFixture();

    public:
        amqp_hash_table_t map;
    };

    AmqpSymbolMapTestFixture::AmqpSymbolMapTestFixture()
    {
    }

    AmqpSymbolMapTestFixture::~AmqpSymbolMapTestFixture()
    {
    }

    TEST_FIXTURE(AmqpSymbolMapTestFixture, symbol_map_should_not_trigger_allocation_errors)
    {
        amqp_symbol_hash_table_initialize(context, &map, 63);
        for (int i = 0; i < key_count(); i++)
        {
            amqp_symbol_t *symbol = amqp_symbol_create(context, keys[i], strlen(keys[i]));
            amqp_hash_table_put(context, &map, symbol, data[i]);
        }
        amqp_symbol_hash_table_cleanup(context, &map);
    }

    TEST_FIXTURE(AmqpSymbolMapTestFixture, symbol_map_put_and_get_are_nothing_special)
    {
        amqp_symbol_hash_table_initialize(context, &map, 63);
        amqp_symbol_t *symbol = amqp_symbol_create(context, keys[6], strlen(keys[6]));
        CHECK(amqp_hash_table_put(context, &map, symbol, data[6]));
        CHECK(amqp_hash_table_put(context, &map, symbol, data[6]) == 0);
        amqp_symbol_hash_table_cleanup(context, &map);
    }
}
