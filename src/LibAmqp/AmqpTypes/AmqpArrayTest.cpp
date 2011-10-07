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

#include "AmqpTypes/AmqpArray.h"
#include "debug_helper.h"

SUITE(AmqpTypes)
{
    class AmqpArrayFixture  : public AmqpTypesFixture
    {
    public:
        AmqpArrayFixture();
        ~AmqpArrayFixture();

        int load_array();

    public:
        amqp_array_t ref;
        amqp_array_t *array;
    };

    AmqpArrayFixture::AmqpArrayFixture() : array(0)
    {
        memset(&ref, '\0', sizeof(amqp_array_t));
    }

    AmqpArrayFixture::~AmqpArrayFixture()
    {
        amqp_array_cleanup(context, &ref);
        amqp_array_cleanup(context, array);
    }

    TEST_FIXTURE(AmqpArrayFixture, fixture_should_balance_allocations)
    {
//        CHECK_EQUAL(32U, amqp_array_capacity(&array));
//        CHECK_CLOSE(0.0, amqp_array_factor(&array), 0.001);
    }

/*
    TEST_FIXTURE(AmqpArrayFixture, entry_count)
    {
        load_array();
        CHECK_EQUAL(39U, amqp_array_count(&array));
        CHECK(amqp_array_factor(&array) > 1.2);
    }

    TEST_FIXTURE(AmqpArrayFixture, add_duplicate_is_not_allowed)
    {
        load_array();
        int rc = amqp_array_put(context, &array, keys[6], data[6]);
        CHECK(!rc);
    }
*/
}
