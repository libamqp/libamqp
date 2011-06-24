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


#include "Memory/MemoryTestSupport.h"

SUITE(Memory)
{
    typedef struct test_type_t
    {
        char padding[100];
    } test_type_t;

    TEST(outstanding_allocations_should_match_count_of_blocks_not_freed)
    {
        test_type_t *p, *q, *r;

        p = (test_type_t *) amqp_malloc(sizeof(test_type_t) TRACE_ARGS);
        CHECK_EQUAL(1UL, amqp_malloc_stats.outstanding_allocations);
        amqp_free(p TRACE_ARGS);
        CHECK_EQUAL(0UL, amqp_malloc_stats.outstanding_allocations);

        q = (test_type_t *) amqp_malloc(sizeof(test_type_t) TRACE_ARGS);
        CHECK_EQUAL(1UL, amqp_malloc_stats.outstanding_allocations);

        r = (test_type_t *) amqp_malloc(sizeof(test_type_t) TRACE_ARGS);
        CHECK_EQUAL(2UL, amqp_malloc_stats.outstanding_allocations);

        amqp_free(q TRACE_ARGS);
        CHECK_EQUAL(1UL, amqp_malloc_stats.outstanding_allocations);

        amqp_free(r TRACE_ARGS);
        CHECK_EQUAL(0UL, amqp_malloc_stats.outstanding_allocations);
    }

    TEST(allocate_macro_should_increase_allocated_block_count)
    {
        test_type_t *q;
        q = AMQP_MALLOC(test_type_t);
        CHECK_EQUAL(1UL, amqp_malloc_stats.outstanding_allocations);
        AMQP_FREE(q);
        CHECK_NULL(q);
        CHECK_EQUAL(0UL, amqp_malloc_stats.outstanding_allocations);
    }
}
