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

