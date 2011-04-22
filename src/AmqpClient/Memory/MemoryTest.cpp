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

#include "TestHarness.h"

#include "Memory/Memory.h"
#include "Buffer/Buffer.h"

namespace
{
    void block_count_check(Test& test, TestResult& result)
    {
        if (amqp_malloc_stats.outstanding_allocations > 0)
        {
            test.addFailureTo(result, "Calls to malloc() and free() do not match");
        }
    }

    TestHook hook(amqp_reset_malloc_allocation_stats, block_count_check);

    void SetUp()
    {
        amqp_reset_malloc_allocation_stats();
    }

    void TearDown()
    {
    }
}

TEST(Memory, usage)
{
    void *p;
    amqp_buffer_t *q;

    p = amqp_malloc(100 TRACE_ARGS);
    CHECK_EQUAL(1UL, amqp_malloc_stats.outstanding_allocations);
    amqp_free(p TRACE_ARGS);
    CHECK_EQUAL(0UL, amqp_malloc_stats.outstanding_allocations);

    q = (amqp_buffer_t *) amqp_malloc(100 TRACE_ARGS);
    CHECK_EQUAL(1UL, amqp_malloc_stats.outstanding_allocations);
    amqp_free(q TRACE_ARGS);
    CHECK_EQUAL(0UL, amqp_malloc_stats.outstanding_allocations);
}

TEST(Memory, allocate_macro)
{
    amqp_buffer_t *q;
    q = AMQP_MALLOC(amqp_buffer_t);
    CHECK_EQUAL(1UL, amqp_malloc_stats.outstanding_allocations);
    AMQP_FREE(q );
    CHECK_EQUAL(0UL, amqp_malloc_stats.outstanding_allocations);
}

