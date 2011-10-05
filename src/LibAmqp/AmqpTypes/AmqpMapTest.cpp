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
#include "AmqpTypes/AmqpTypesTestSupport.h"

#include "AmqpTypes/AmqpMap.h"
#include "debug_helper.h"

SUITE(AmqpTypes)
{
    class AmqpMapFixture  : public AmqpTypesFixture
    {
    public:
        AmqpMapFixture();
        ~AmqpMapFixture();

        int load_map();

    public:
        amqp_map_t map;
    };

    AmqpMapFixture::AmqpMapFixture()
    {
        amqp_map_initialize(context, &map, 31, (amqp_hash_fn_t) hash, (amqp_compare_fn_t) compare);
    }

    AmqpMapFixture::~AmqpMapFixture()
    {
        amqp_map_cleanup(context, &map);
    }

    int AmqpMapFixture::load_map()
    {
        int i;
        for (i = 0; data[i]; i++)
        {
            amqp_map_put(context, &map, keys[i], data[i]);
        }
        return i;
    }

    TEST_FIXTURE(AmqpMapFixture, fixture_should_balance_allocations)
    {
        CHECK_EQUAL(32U, amqp_map_capacity(&map));
        CHECK_CLOSE(0.0, amqp_map_factor(&map), 0.001);
    }

    TEST_FIXTURE(AmqpMapFixture, entry_count)
    {
        load_map();
        CHECK_EQUAL(39U, amqp_map_count(&map));
        CHECK(amqp_map_factor(&map) > 1.2);
    }

    TEST_FIXTURE(AmqpMapFixture, add_duplicate_is_not_allowed)
    {
        load_map();
        int rc = amqp_map_put(context, &map, keys[6], data[6]);
        CHECK(!rc);
    }

    TEST_FIXTURE(AmqpMapFixture, lookup)
    {
        load_map();
        for (int i = 0; keys[i]; i++)
        {
            const void *answer = amqp_map_get(&map, keys[i]);
            CHECK(strcmp(data[i], (const char *) answer) == 0);
        }
    }

}
