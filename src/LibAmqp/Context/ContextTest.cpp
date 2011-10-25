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

// TODO - these tests should not be looking into buffer internals
#include "Buffer/BufferInternal.h"

SUITE(Context)
{
    TEST_FIXTURE(ContextFixture, fixture_dtor_should_not_raise_an_exception)
    {
        // nothing to do here
    }

    TEST_FIXTURE(ContextFixture, clone_should_balance_its_allocation_counts)
    {
        amqp_context_t *clone = amqp_context_clone(context);
        amqp_buffer_t *buffer = amqp_allocate_buffer(clone);

        void *p = amqp_malloc(clone, 128);

        amqp_deallocate_buffer(clone, buffer);
        
        amqp_free(clone, p);

        int clone_allocations_ok = amqp_context_destroy(clone);
        CHECK(clone_allocations_ok);
    }

    TEST_FIXTURE(ContextFixture, fixture_should_have_a_usable_decode_buffer)
    {
        static unsigned char bytes[] = { 0x01, 0xa5, 0x03 };
        test_data::TestData test_bytes(bytes, sizeof(bytes));

        load_decode_buffer(test_bytes);

        CHECK_EQUAL(1, amqp_buffer_getc(decode_buffer));
        CHECK_EQUAL(0xa5, amqp_buffer_getc(decode_buffer));
        CHECK_EQUAL(0x03, amqp_buffer_getc(decode_buffer));
    }
}

