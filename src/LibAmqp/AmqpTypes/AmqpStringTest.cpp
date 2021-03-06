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

#include <TestHarness.h>
#include "AmqpTypes/AmqpTypesTestFixture.h"

#include "AmqpTypes/AmqpString.h"
#include "Codec/Type/TypeExtension.h"

#include "debug_helper.h"

SUITE(AmqpTypes)
{
    class AmqpStringTestFixture  : public AmqpTypesTestFixture
    {
    public:
        AmqpStringTestFixture();
        ~AmqpStringTestFixture();

    public:
        amqp_string_t ref;
        amqp_string_t *string;
    };

    AmqpStringTestFixture::AmqpStringTestFixture() : string(0)
    {
        memset(&ref, '\0', sizeof(amqp_string_t));
    }

    AmqpStringTestFixture::~AmqpStringTestFixture()
    {
        amqp_string_cleanup(context, &ref);
        amqp_string_cleanup(context, string);
    }

    TEST_FIXTURE(AmqpStringTestFixture, string_type_access)
    {
        test_data::encoded_lorem_ipsum.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        CHECK(amqp_type_is_string(type));
        string = amqp_string_create_from_type(context, type);

        int n = strlen(test_data::lorem_ipsum) + 1;
        uint8_t data[n];
        memset(data, '\0', n);

        amqp_string_to_bytes(string, data, n);
        CHECK_EQUAL(257U, amqp_string_size(string));
        CHECK_EQUAL((const char *) data, test_data::lorem_ipsum);
    }

    TEST_FIXTURE(AmqpStringTestFixture, string_clone)
    {
        const char *value = "Hello World";
        amqp_string_initialize(context, &ref, value, strlen(value));
        CHECK(amqp_string_compare_with_cstr(&ref, value) == 0);
        string = amqp_string_clone(context, &ref);
        CHECK(amqp_string_compare_with_cstr(string, value) == 0);
    }

    TEST_FIXTURE(AmqpStringTestFixture, string_compare)
    {
        test_data::hello_world.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        CHECK(amqp_type_is_string(type));
        string = amqp_string_create_from_type(context, type);

        CHECK_EQUAL(11U, amqp_string_size(string));
        CHECK(amqp_string_compare_with_cstr(string, "Hello World") == 0);
    }
}
