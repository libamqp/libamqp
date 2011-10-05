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

SUITE(AmqpTypes)
{
    class AmqpMapFixture  : public SuiteContext::ContextFixture
    {
    public:
        AmqpMapFixture();
        ~AmqpMapFixture();

        static const char *data[];

    public:
        amqp_map_t map;
    };

    const char *AmqpMapFixture::data[] = {
        "Lorem ipsum dolor sit amet ",
        "consectetur adipisicing elit",
        ", sed do eiusmod tempor ",
        "incididunt",
         "ut labore et dolore magna ",
         "aliqua. Ut enim ad minim ",
         "veniam, quis nostrud ",
         "exercitation ",
         "ullamco laboris ",
         "nisi ut aliquip ex ",
         "ea commodo consequat. ",
         "Duis aute irure dolor ",
         "in reprehenderit in ",
         "voluptate velit ",
         "esse ",
         "cillum ",
         "dolore eu ",
         "fugiat nulla pariatur. ",
         "Excepteur sint occaecat cupidatat ",
         "non proident, sunt in ",
         "culpa qui officia deserunt ",
         "mollit anim id est laborum.",
         0
     };

    AmqpMapFixture::AmqpMapFixture()
    {
        amqp_map_initialize(context, &map, 31);
    }

    AmqpMapFixture::~AmqpMapFixture()
    {
        amqp_map_cleanup(context, &map);
    }

    TEST_FIXTURE(AmqpMapFixture, fixture_should_balance_allocations)
    {
        CHECK_EQUAL(32U, amqp_map_capacity(&map));
        CHECK_CLOSE(0.0, amqp_map_factor(&map), 0.001);
    }

    TEST_FIXTURE(AmqpMapFixture, lookup)
    {
        for (int i = 0; data[i];i++)
        {

        }
    }

}
