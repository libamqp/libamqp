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

    TEST_FIXTURE(AmqpArrayFixture, array_initialize)
    {
        amqp_array_initialize(context, &ref, 13);
        CHECK_EQUAL(13U, amqp_array_capacity(&ref));
        CHECK_EQUAL(13U, amqp_array_count(&ref));
    }

    TEST_FIXTURE(AmqpArrayFixture, array_create)
    {
        array = amqp_array_create(context, 13);
        CHECK_EQUAL(13U, amqp_array_capacity(array));
        CHECK_EQUAL(13U, amqp_array_count(array));
    }

    TEST_FIXTURE(AmqpArrayFixture, array_element_access)
    {
        static const char *data[] = { "1", "2", "3", "4", "5"};
        array = amqp_array_create(context, 5);
        amqp_array_initialize(context, &ref, 5);
        for (int i = 0; i < 5; i++)
        {
            amqp_array_set(array, i, data[i]);
            amqp_array_set(&ref, i, data[i]);
        }
        for (int i = 0; i < 5; i++)
        {
            CHECK_EQUAL(data[i], amqp_array_get(array, i));
            CHECK_EQUAL(data[i], amqp_array_get(&ref, i));
        }
    }
}
