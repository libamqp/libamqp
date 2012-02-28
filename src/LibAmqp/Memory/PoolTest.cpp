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
#include <stdlib.h>

#include "libamqp_common.h"
#include <TestHarness.h>
#include "Memory/MemoryTestFixture.h"

#include "Memory/Pool.h"
#include "Memory/PoolInternal.h"

#include "debug_helper.h"

#define CHECK_POOL_ALLOCATIONS(pool) \
    do \
    { \
        try { \
            if (UnitTest::Check((pool)->stats.outstanding_allocations != 0)) \
                UnitTest::CurrentTest::Results()->OnTestFailure(*UnitTest::CurrentTest::Details(), "Number of allocates from pool does not match number of deletes."); \
        } \
        catch (...) { \
            UnitTest::CurrentTest::Results()->OnTestFailure(*UnitTest::CurrentTest::Details(), "Unhandled exception in CHECK_POOL_ALLOCATIONS()"); \
        } \
    } while (0)

SUITE(Pool)
{
    typedef struct test_type_t
    {
        char important_stuff[16];
    } test_type_t;

    class PoolAllocator
    {
    public:
        PoolAllocator(amqp_context_t *context, amqp_memory_pool_t *pool_);
        ~PoolAllocator();
        test_type_t *allocate_one();
        void remove_one();

    private:
        int count, capacity;
        amqp_context_t *m_context;
        amqp_memory_pool_t *pool;
        test_type_t **allocations;
    };

    // TODO -  exposing too much from pool internal tests to other suites.
    class PoolFixture : public SuiteMemory::MemoryTestFixture
    {
    public:
        PoolFixture();
        ~PoolFixture();
        void initialize_pool(int n);
        void initialize_pool();
        test_type_t *allocate_from_pool(int n);
        test_type_t *allocate_from_pool();
        void return_last_allocated_to_pool();

    public:
        amqp_memory_pool_t pool;

    private:
        PoolAllocator *allocator;
    };

    class InitializedPoolFixture : public PoolFixture
    {
    public:
        InitializedPoolFixture()
        {
            initialize_pool();
        }
        ~InitializedPoolFixture() { }
    };

        PoolAllocator::PoolAllocator(amqp_context_t *context, amqp_memory_pool_t *pool_) :  count(0), capacity(256), m_context(context), pool(pool_)
    {
    }

    PoolAllocator::~PoolAllocator()
    {
        for (int i = 0; i < count; i++)
        {
            amqp_deallocate(m_context, pool, allocations[i]);
        }
        free(allocations);
    }

    test_type_t *PoolAllocator::allocate_one()
    {
        if (count == 0)
        {
            allocations = (test_type_t **) malloc(sizeof(test_type_t *) * capacity);
        }
        if (count == capacity)
        {
            capacity *= 2;
            allocations = (test_type_t **) realloc(allocations, sizeof(test_type_t *) * capacity);
        }
        return allocations[count++] = (test_type_t *) amqp_allocate(m_context, pool);
    }

    void PoolAllocator::remove_one()
    {
        if (count > 0)
        {
            count--;
            amqp_deallocate(m_context, pool, allocations[count]);
        }
    }

    PoolFixture::PoolFixture() : allocator(0)
    {
        memset(&pool, '\0', sizeof(pool));
    }

    PoolFixture::~PoolFixture()
    {
        delete allocator;
        if (pool.initialized)
        {
            CHECK_POOL_ALLOCATIONS(&pool);
        }
    }

    static void allocate_callback(amqp_context_t *context, amqp_memory_pool_t *pool, test_type_t *object)
    {
        for (int i = 0; i < (int) sizeof(object->important_stuff); i++)
        {
            object->important_stuff[i] = 'A' + i;
        }
    }

    static void deallocate_callback(amqp_context_t *context, amqp_memory_pool_t *pool, void *object)
    {
    }

    void PoolFixture::initialize_pool(int n)
    {
        CHECK(n > 128);
        if (!pool.initialized)
        {
            amqp_initialize_pool_suggesting_block_size(&pool, sizeof(test_type_t), n, "test");
            amqp_pool_specify_initialization_callbacks(&pool, (amqp_pool_callback_t) allocate_callback, deallocate_callback);
        }
    }

    void PoolFixture::initialize_pool()
    {
        if (!pool.initialized)
        {
            amqp_initialize_pool(&pool, sizeof(test_type_t), "test");
            amqp_pool_specify_initialization_callbacks(&pool, (amqp_pool_callback_t) allocate_callback, deallocate_callback);
        }
    }

    test_type_t *PoolFixture::allocate_from_pool(int n)
    {
        test_type_t *result;
        if (allocator == 0)
        {
            allocator = new PoolAllocator(&m_context, &pool);
        }
        for (int i = 0; i < n; i++)
        {
            result = allocator->allocate_one();
        }
        return result;
    }

    test_type_t *PoolFixture::allocate_from_pool()
    {
        return allocate_from_pool(1);
    }

    void PoolFixture::return_last_allocated_to_pool()
    {
        allocator->remove_one();
    }

    TEST_FIXTURE(InitializedPoolFixture, callback_should_initialize_object)
    {
        test_type_t *p = (test_type_t *) amqp_allocate(&m_context, &pool);
        CHECK_EQUAL('A', p->important_stuff[0]);
        for (unsigned i = 0; i < sizeof(p->important_stuff); i++)
        {
            CHECK_EQUAL('A' + (int) i, p->important_stuff[i]);
        }
        amqp_deallocate(&m_context, &pool, p);
    }

#ifdef LIBAMQP_DISABLE_MEMORY_POOL
    TEST_FIXTURE(InitializedPoolFixture, verify_allocation_works_when_pool_disabled)
    {
        test_type_t *p, *q;
        p = (test_type_t *) amqp_allocate(&m_context, &pool);
        q = (test_type_t *) amqp_allocate(&m_context, &pool);
        amqp_deallocate(&m_context, &pool, p);
        amqp_deallocate(&m_context, &pool, q);
    }
#else

#ifdef LIBAMQP_DEBUG_ALLOCATIONS
    TEST_FIXTURE(InitializedPoolFixture, allocation_should_initialize_chain)
    {
        test_type_t *p = (test_type_t *) amqp_allocate(&m_context, &pool);
        CHECK_NOT_NULL(p);

        amqp_memory_allocation_t *allocation = pool.allocation_chain;

        CHECK_EQUAL(allocation->header.debug.previous, &pool.allocation_chain);
        CHECK_EQUAL(allocation->header.debug.next, (void *) 0);

        CHECK_EQUAL((test_type_t *) &allocation->data, p);

        amqp_deallocate(&m_context, &pool, p);
    }

    TEST_FIXTURE(InitializedPoolFixture, many_allocations_should_grow_chain)
    {
        test_type_t *p = (test_type_t *) amqp_allocate(&m_context, &pool);
        CHECK_NOT_NULL(p);

        amqp_memory_allocation_t *allocation_p = pool.allocation_chain;

        test_type_t *q = (test_type_t *) amqp_allocate(&m_context, &pool);
        CHECK_NOT_NULL(q);

        amqp_memory_allocation_t *allocation_q = pool.allocation_chain;
        CHECK_EQUAL((test_type_t *) &allocation_q->data, q);

        CHECK_EQUAL(allocation_q->header.debug.previous, &pool.allocation_chain);
        CHECK_EQUAL(allocation_q->header.debug.next, allocation_p);

        amqp_deallocate(&m_context, &pool, q);
        amqp_deallocate(&m_context, &pool, p);
    }

    TEST_FIXTURE(InitializedPoolFixture, deleting_an_allocation_should_shrink_chain)
    {
        test_type_t *p = (test_type_t *) amqp_allocate(&m_context, &pool);
        test_type_t *q = (test_type_t *) amqp_allocate(&m_context, &pool);

        amqp_deallocate(&m_context, &pool, q);

        amqp_memory_allocation_t *allocation = pool.allocation_chain;
        CHECK_EQUAL((test_type_t *) &allocation->data, p);
        CHECK_EQUAL(allocation->header.debug.previous, &pool.allocation_chain);
        CHECK_EQUAL(allocation->header.debug.next, (void *) 0);

        amqp_deallocate(&m_context, &pool, p);
        CHECK_EQUAL(pool.allocation_chain, (void *) 0);
    }
#endif

    TEST_FIXTURE(InitializedPoolFixture, first_allocate_from_pool_allocates_block)
    {
        CHECK_NULL(pool.block_list);

        test_type_t *p = (test_type_t *) amqp_allocate(&m_context, &pool);
        CHECK_NOT_NULL(p);

        CHECK_NOT_NULL(pool.block_list);
        CHECK_NULL(pool.block_list->header.previous);
        CHECK_NULL(pool.block_list->header.next);

        amqp_deallocate(&m_context, &pool, p);
    }

    TEST_FIXTURE(InitializedPoolFixture, allocate_from_pool)
    {
        test_type_t *p = (test_type_t *) amqp_allocate(&m_context, &pool);

        CHECK_NOT_NULL(pool.block_list);
        CHECK_NULL(pool.block_list->header.previous);
        CHECK_NULL(pool.block_list->header.next);

        CHECK_EQUAL(0xfe, pool.block_list->header.mask.bytes[0]);
        CHECK_NOT_NULL(p);

        amqp_deallocate(&m_context, &pool, p);
    }

    TEST_FIXTURE(InitializedPoolFixture, allocate_from_pool_increases_allocation_count)
    {
        int i;

        for (i = 0; i < LONG_BIT; i++)
        {
            allocate_from_pool();
            CHECK_EQUAL((unsigned long) (i + 1), pool.stats.outstanding_allocations);
            CHECK_EQUAL((unsigned long) (i + 1), pool.stats.total_allocation_calls);
        }
        while (i > 0)
        {
            i--;
            return_last_allocated_to_pool();
            CHECK_EQUAL((unsigned long) i, pool.stats.outstanding_allocations);
        }
        CHECK_EQUAL((unsigned long) LONG_BIT, pool.stats.total_allocation_calls);
    }

    TEST_FIXTURE(InitializedPoolFixture, allocations_should_be_reflected_in_mask)
    {
        allocate_from_pool(1);
        CHECK_EQUAL(0xfeU, (unsigned) pool.block_list->header.mask.bytes[0]);

        allocate_from_pool(6);
        CHECK_EQUAL(0x80U, (unsigned) pool.block_list->header.mask.bytes[0]);

        allocate_from_pool(1);
        CHECK_EQUAL(0x00U, (unsigned) pool.block_list->header.mask.bytes[0]);
        CHECK_EQUAL(0xffU, (unsigned) pool.block_list->header.mask.bytes[1]);

        allocate_from_pool(1);
        CHECK_EQUAL(0xfeU, (unsigned) pool.block_list->header.mask.bytes[1]);
        CHECK_EQUAL(0xffU, (unsigned) pool.block_list->header.mask.bytes[2]);

        allocate_from_pool(6);
        CHECK_EQUAL(0x80U, (unsigned) pool.block_list->header.mask.bytes[1]);

        allocate_from_pool(1);
        CHECK_EQUAL(0x00U, (unsigned) pool.block_list->header.mask.bytes[1]);
        CHECK_EQUAL(0xffU, (unsigned) pool.block_list->header.mask.bytes[2]);

        allocate_from_pool(1);
        CHECK_EQUAL(0xfeU, (unsigned) pool.block_list->header.mask.bytes[2]);
    }

    class BlockChainingTestFixture : public PoolFixture
    {
    public:
        BlockChainingTestFixture() { }
        ~BlockChainingTestFixture() { }

        void test()
        {
            allocate_from_pool(1);
            amqp_memory_block_t *first_block = pool.block_list;
            CHECK_NULL(pool.block_list->header.previous);
            CHECK_NULL(pool.block_list->header.next);

            allocate_from_pool(pool.allocations_per_block - 1);
            CHECK_EQUAL(first_block, pool.block_list);
            CHECK_NULL(pool.block_list->header.previous);
            CHECK_NULL(pool.block_list->header.next);

            allocate_from_pool(1);
            amqp_memory_block_t *second_block = pool.block_list;
            CHECK_EQUAL(first_block, pool.block_list->header.next);
            CHECK_NULL(pool.block_list->header.previous);
            CHECK_NULL(pool.block_list->header.next->header.next);
            CHECK_EQUAL(second_block, pool.block_list->header.next->header.previous);
        }
    };

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block)
    {
        initialize_pool();
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_256)
    {
        initialize_pool(256);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_512)
    {
        initialize_pool(512);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_768)
    {
        initialize_pool(768);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_1024)
    {
        initialize_pool(1024);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_1526)
    {
        initialize_pool(1526);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_2048)
    {
        initialize_pool(2048);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_4096)
    {
        initialize_pool(4096);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, free_allocation_in_block_should_result_in_block_removal_from_chain)
    {
        initialize_pool(256);

        CHECK_NULL(pool.block_list); // no blocks on the pool

        // allocate first block
        allocate_from_pool();
        amqp_memory_block_t *first_block = pool.block_list;

        // allocate remainder of slots in the first block
        allocate_from_pool(pool.allocations_per_block - 1);
        CHECK_EQUAL(first_block, pool.block_list);

        // allocate second block
        allocate_from_pool();
        amqp_memory_block_t *second_block = pool.block_list;
        CHECK(first_block != second_block);

        // allocate remainder of slots in the second block
        allocate_from_pool(pool.allocations_per_block - 1);
        CHECK_EQUAL(second_block, pool.block_list);

        // allocate another causing a third block to be allocated
        test_type_t *p = (test_type_t *) amqp_allocate(&m_context, &pool);
        amqp_memory_block_t *third_block = pool.block_list;
        CHECK(second_block != third_block);

        CHECK_NULL(first_block->header.next);
        CHECK_EQUAL(second_block, first_block->header.previous);
        CHECK_EQUAL(first_block, second_block->header.next);
        CHECK_EQUAL(third_block, second_block->header.previous);
        CHECK_EQUAL(second_block, third_block->header.next);
        CHECK_NULL(third_block->header.previous);

        for (int i = 0; i < pool.allocations_per_block; i++)
        {
            return_last_allocated_to_pool(); // return last allocated by call to allocate_from_pool();
        }

        // verify that the second block has been removed from the block list
        // because all its allocations have been deallocated
        CHECK_EQUAL(third_block, pool.block_list);
        CHECK_NULL(third_block->header.previous);
        CHECK_EQUAL(first_block, third_block->header.next);
        CHECK_NULL(first_block->header.next);
        CHECK_EQUAL(third_block, first_block->header.previous);

        // now remove the last allocation from the pool causing the third block to be removed from the list.
        amqp_deallocate(&m_context, &pool, p);

        CHECK_EQUAL(first_block, pool.block_list);
        CHECK_NULL(first_block->header.previous);
        CHECK_NULL(first_block->header.next);

        // deallocate everything from the first block
        for (int i = 0; i < pool.allocations_per_block; i++)
        {
            return_last_allocated_to_pool(); // return last allocated by call to allocate_from_pool();
        }

        CHECK_NULL(pool.block_list); // assert that there are no blocks left
    }
#endif
}
