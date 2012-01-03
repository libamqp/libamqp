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
#include "AmqpTypes/AmqpTypesTestSupport.h"

#include "AmqpTypes/AmqpMultiple.h"

SUITE(AmqpTypes)
{
    class AmqpMultiplesFixture  : public AmqpTypesFixture
    {
    public:
        AmqpMultiplesFixture();
        ~AmqpMultiplesFixture();

    public:
        amqp_multiple_symbol_t multiple_ref;
        amqp_multiple_symbol_t *multiple;
    };

    AmqpMultiplesFixture::AmqpMultiplesFixture() : multiple(0)
    {
        memset(&multiple_ref, '\0', sizeof(amqp_multiple_symbol_t));
    }

    AmqpMultiplesFixture::~AmqpMultiplesFixture()
    {
        amqp_multiple_symbol_cleanup(context, &multiple_ref);
        amqp_multiple_symbol_cleanup(context, multiple);
    }

    TEST_FIXTURE(AmqpMultiplesFixture, multiple_symbol_one_value)
    {
        test_data::multiple_symbol_one_value.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        CHECK(amqp_type_is_variable(type));
        CHECK(amqp_type_is_symbol(type));

        multiple = amqp_multiple_symbol_create(context, type);
        CHECK_EQUAL(1, multiple->size);

        amqp_symbol_t *symbol;
        symbol = amqp_multiple_symbol_get(multiple, 0);
        CHECK(amqp_symbol_compare_with_cstr(symbol, "PLAIN") == 0);

        int rc = amqp_multiple_symbol_initialize(context, &multiple_ref, type);
        ASSERT(rc);

        CHECK_EQUAL(1, multiple_ref.size);
        symbol = amqp_multiple_symbol_get(&multiple_ref, 0);
        CHECK(amqp_symbol_compare_with_cstr(symbol, "PLAIN") == 0);
    }

    TEST_FIXTURE(AmqpMultiplesFixture, multiple_symbol_total_length)
    {
        test_data::multiple_symbol_many_values.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        CHECK(amqp_type_is_array(type));
        multiple = amqp_multiple_symbol_create(context, type);
        CHECK_EQUAL(3, multiple->size);
        CHECK_EQUAL(11, amqp_multiple_symbol_total_length(multiple));
    }

    TEST_FIXTURE(AmqpMultiplesFixture, multiple_symbol_to_string)
    {
        test_data::multiple_symbol_many_values.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        CHECK(amqp_type_is_array(type));
        multiple = amqp_multiple_symbol_create(context, type);
        CHECK_EQUAL(3, multiple->size);

        uint8_t buffer[512];
        amqp_multiple_symbol_to_char_bytes(multiple, ", ", buffer, sizeof(buffer));
        CHECK_EQUAL("PLAIN, Foo, Fum", (char *) buffer);
    }

    TEST_FIXTURE(AmqpMultiplesFixture, multiple_symbol_to_string_with_small_buffer)
    {
        test_data::multiple_symbol_many_values.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        CHECK(amqp_type_is_array(type));
        multiple = amqp_multiple_symbol_create(context, type);
        CHECK_EQUAL(3, multiple->size);

        uint8_t buffer[5];
        amqp_multiple_symbol_to_char_bytes(multiple, ", ", buffer, sizeof(buffer));
        CHECK_EQUAL("PLAI", (char *) buffer);
    }

    TEST_FIXTURE(AmqpMultiplesFixture, multiple_symbol_empty_array)
    {
        test_data::empty_array_of_symbols.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        ASSERT(type);
        CHECK(amqp_type_is_array(type));
        CHECK_EQUAL(0U, type->value.array.count);
        CHECK(amqp_type_is_symbol(amqp_type_array_type(type)));

        multiple = amqp_multiple_symbol_create(context, type);
        CHECK_EQUAL(0, multiple->size);

        int rc = amqp_multiple_symbol_initialize(context, &multiple_ref, type);
        ASSERT(rc);
        CHECK_EQUAL(0, multiple_ref.size);
    }

    TEST_FIXTURE(AmqpMultiplesFixture, multiple_symbol_null)
    {
        test_data::multiple_symbol_null.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        multiple = amqp_multiple_symbol_create(context, type);
        CHECK_EQUAL(0, multiple->size);

        int rc = amqp_multiple_symbol_initialize(context, &multiple_ref, type);
        ASSERT(rc);
        CHECK_EQUAL(0, multiple_ref.size);
    }

    TEST_FIXTURE(AmqpMultiplesFixture, multiple_symbol_many_values)
    {
        test_data::multiple_symbol_many_values.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        CHECK(amqp_type_is_array(type));

        multiple = amqp_multiple_symbol_create(context, type);
        CHECK_EQUAL(3, multiple->size);

        // TODO - deal with case where get returns null (compare should return false)
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "PLAIN") == 0);
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 1), "Foo") == 0);
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 2), "Fum") == 0);

        int rc = amqp_multiple_symbol_initialize(context, &multiple_ref, type);
        ASSERT(rc);

        CHECK_EQUAL(3, multiple_ref.size);
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(&multiple_ref, 0), "PLAIN") == 0);
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(&multiple_ref, 1), "Foo") == 0);
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(&multiple_ref, 2), "Fum") == 0);
    }
}
