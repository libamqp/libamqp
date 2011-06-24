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
            deallocate_type(type);
        }
        amqp_memory_pool_t *type_pool()
        {
            return &context->pools.amqp_type_t_pool;
        }
        amqp_type_t *allocate_type()
        {
            return (amqp_type_t *) amqp_allocate(type_pool());
        }
        void deallocate_type(amqp_type_t *type)
        {
            amqp_deallocate(type_pool(), type);
        }

    public:
        amqp_type_t *type;
    };
    
    
    TEST_FIXTURE(TypeFixture, aray_realloc)
    {
        int i;
        amqp_type_t **array_1 = amqp_allocate_amqp_type_t_array(15);
        amqp_type_t **array_2 = amqp_allocate_amqp_type_t_array(15);
    
        for (i = 0; i < 15; i++)
        {
            array_1[i] = allocate_type();
            array_2[i] = array_1[i];
        }
    
        array_1 = amqp_realloc_amqp_type_t_array(array_1, 16);
        for (i = 0; i < 15; i++)
        {
            CHECK_EQUAL(array_2[i], array_1[i]);
        }
        CHECK_EQUAL((void *) 0, array_1[16]);
    
        amqp_deallocate_amqp_type_t_array(type_pool(), array_1, 15);
        amqp_deallocate_amqp_type_t_array(type_pool(), array_2, 0);
    }
    
    TEST_FIXTURE(TypeFixture, flags)
    {
        amqp_type_t type = { 0 };
        CHECK_EQUAL(sizeof(unsigned) * 2, sizeof(type.flags));
    
        CHECK(!amqp_type_is_map(&type));
        CHECK(!amqp_type_is_list(&type));
        CHECK(!amqp_type_is_array(&type));
        CHECK(!amqp_type_is_compound(&type));
    }
    
    TEST_FIXTURE(TypeFixture, flags_is_map)
    {
        amqp_type_t type = { 0 };
        type.flags.structure.flags.is_map = true;
    
        CHECK(amqp_type_is_compound(&type));
        CHECK(amqp_type_is_map(&type));
        CHECK(!amqp_type_is_list(&type));
        CHECK(!amqp_type_is_array(&type));
    }
    
    TEST_FIXTURE(TypeFixture, flags_is_list)
    {
        amqp_type_t type = { 0 };
        type.flags.structure.flags.is_list = true;
    
        CHECK(amqp_type_is_compound(&type));
        CHECK(!amqp_type_is_map(&type));
        CHECK(amqp_type_is_list(&type));
        CHECK(!amqp_type_is_array(&type));
    }
    
    TEST_FIXTURE(TypeFixture, flags_is_array)
    {
        amqp_type_t type = { 0 };
        type.flags.structure.flags.is_array = true;
    
        CHECK(amqp_type_is_compound(&type));
        CHECK(!amqp_type_is_map(&type));
        CHECK(!amqp_type_is_list(&type));
        CHECK(amqp_type_is_array(&type));
    }
    
    static void copy_flags(amqp_type_t *t1, amqp_type_t *t2)
    {
        t2->flags.structure = t1->flags.structure;
    }
    
    TEST_FIXTURE(TypeFixture, flags_assign)
    {
        amqp_type_t t1 = { 0 };
        amqp_type_t t2 = { 0 };
    
        t1.flags.structure.flags.is_list = true;
        copy_flags(&t1, &t2);
    
        CHECK(amqp_type_is_compound(&t2));
        CHECK(!amqp_type_is_map(&t2));
        CHECK(amqp_type_is_list(&t2));
        CHECK(!amqp_type_is_array(&t2));
    }
}