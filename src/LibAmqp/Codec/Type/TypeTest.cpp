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
#include "Codec/Type/Type.h"


SUITE(Type)
{
    class TypeFixture : public SuiteContext::ContextFixture
    {
    public:
        TypeFixture() : type(0) {}
        ~TypeFixture()
        {
            amqp_deallocate_type(context, type);
        }
        amqp_type_t *allocate_type()
        {
            return amqp_allocate_type(context);
        }
        amqp_memory_pool_t *type_pool()
        {
            return &context->memory.amqp_type_t_pool;
        }

    public:
        amqp_type_t *type;
    };
    
    TEST_FIXTURE(TypeFixture, aray_realloc)
    {
        int i;
        amqp_type_t **array_1 = amqp_allocate_amqp_type_t_array(context, 15);
        amqp_type_t **array_2 = amqp_allocate_amqp_type_t_array(context, 15);
    
        for (i = 0; i < 15; i++)
        {
            array_1[i] = amqp_allocate_type(context);
            array_2[i] = array_1[i];
        }
    
        array_1 = amqp_realloc_amqp_type_t_array(context, array_1, 16);
        for (i = 0; i < 15; i++)
        {
            CHECK_EQUAL(array_2[i], array_1[i]);
        }
        CHECK_EQUAL((void *) 0, array_1[16]);
    
        amqp_deallocate_amqp_type_t_array(context, type_pool(), array_1, 15);
        amqp_deallocate_amqp_type_t_array(context, type_pool(), array_2, 0);
    }
    
    TEST_FIXTURE(TypeFixture, typedef_flags_fixed)
    {
        CHECK_EQUAL(amqp_is_fixed_mask,
                amqp_is_null | amqp_is_boolean | amqp_is_unsigned |
                amqp_is_byte | amqp_is_short | amqp_is_int | amqp_is_long |
                amqp_is_float | amqp_is_double | amqp_is_timestamp | amqp_is_uuid |
                amqp_is_decimal32 | amqp_is_decimal64 | amqp_is_decimal128 | amqp_is_char
            );
    }

    TEST_FIXTURE(TypeFixture, typedef_flags_variable)
    {
        CHECK_EQUAL(amqp_is_variable_mask, amqp_is_binary | amqp_is_string | amqp_is_symbol);
    }

    TEST_FIXTURE(TypeFixture, typedef_flags_compound)
    {
        CHECK_EQUAL(amqp_is_compound_mask, amqp_is_list | amqp_is_map);
    }

    TEST_FIXTURE(TypeFixture, typedef_flags_container)
    {
        CHECK_EQUAL(amqp_is_container_mask, amqp_is_list | amqp_is_map | amqp_is_array | amqp_is_composite);
    }

    TEST_FIXTURE(TypeFixture, flags)
    {
        amqp_type_t type = { 0 };

        CHECK(!amqp_type_is_map(&type));
        CHECK(!amqp_type_is_list(&type));
        CHECK(!amqp_type_is_array(&type));
        CHECK(!amqp_type_is_container(&type));
    }
    
    TEST_FIXTURE(TypeFixture, flags_is_map)
    {
        amqp_type_t type = { 0 };
        type.typedef_flags = amqp_is_map;

        CHECK(amqp_type_is_container(&type));
        CHECK(amqp_type_is_map(&type));
        CHECK(!amqp_type_is_list(&type));
        CHECK(!amqp_type_is_array(&type));
    }
    
    TEST_FIXTURE(TypeFixture, flags_is_list)
    {
        amqp_type_t type = { 0 };
        type.typedef_flags = amqp_is_list;

        CHECK(amqp_type_is_container(&type));
        CHECK(!amqp_type_is_map(&type));
        CHECK(amqp_type_is_list(&type));
        CHECK(!amqp_type_is_array(&type));
    }
    
    TEST_FIXTURE(TypeFixture, flags_is_array)
    {
        amqp_type_t type = { 0 };
        type.typedef_flags = amqp_is_array;

        CHECK(amqp_type_is_container(&type));
        CHECK(!amqp_type_is_map(&type));
        CHECK(!amqp_type_is_list(&type));
        CHECK(amqp_type_is_array(&type));
    }
}