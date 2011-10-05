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

#include "AmqpTypes/AmqpMap.h"
#include "debug_helper.h"

SUITE(AmqpTypes)
{
    class AmqpMapFixture  : public SuiteContext::ContextFixture
    {
    public:
        AmqpMapFixture();
        ~AmqpMapFixture();

        static const char *keys[40];
        static const char *data[40];

        static int compare(const char *lhs, const char *rhs);
        static uint32_t hash(const char *data);

        int load_map();

    public:
        amqp_map_t map;
    };

    const char *AmqpMapFixture::keys[40] = {
        "key_000", "key_001", "key_002", "key_003", "key_004", "key_005", "key_006", "key_007", "key_008", "key_009",
        "key_010", "key_011", "key_012", "key_013", "key_014", "key_015", "key_016", "key_017", "key_018", "key_019",
        "key_020", "key_021", "key_022", "key_023", "key_024", "key_025", "key_026", "key_027", "key_028", "key_029",
        "key_030", "key_031", "key_032", "key_033", "key_034", "key_035", "key_036", "key_037", "key_038",
        0
        };
    const char *AmqpMapFixture::data[40] = {
        "Lorem ipsum ", "dolor sit amet ", "consectetur ", "adipisicing elit", ", sed do ",
        "eiusmod tempor ", "incididunt", "ut labore et ", "dolore magna ", "aliqua. Ut ",
        "enim ad minim ", "veniam, ", "quis ", "nostrud ", "exercitation ", "ullamco laboris ",
        "nisi ut aliquip ex ", "ea commodo ", "consequat. ", "Duis aute ", "irure dolor ", "in ",
        "reprehenderit ", "in", " voluptate ", "velit ", "esse ", "cillum ", "dolore eu ",
        "fugiat nulla ", "pariatur. ", "Excepteur sint ", "occaecat cupidatat ", "non proident, ",
        "sunt in ", "culpa qui ", "officia deserunt ", "mollit anim ", "id est laborum.",
        0
     };

    AmqpMapFixture::AmqpMapFixture()
    {
        amqp_map_initialize(context, &map, 31, (amqp_hash_fn_t) hash, (amqp_compare_fn_t) compare);
    }

    AmqpMapFixture::~AmqpMapFixture()
    {
        amqp_map_cleanup(context, &map);
    }

    int AmqpMapFixture::compare(const char *lhs, const char *rhs)
    {
        return strcmp(lhs, rhs);
    }

    uint32_t AmqpMapFixture::hash(const char *data)
    {
        return amqp_hash(data, strlen(data));
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
