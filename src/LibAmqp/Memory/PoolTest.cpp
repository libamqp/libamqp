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

#include "Memory/PoolTestSupport.h"
#include "debug_helper.h"

SUITE(Pool)
{
    TEST_FIXTURE(InitializedPoolFixture, callback_should_initialize_object)
    {
        test_type_t *p = (test_type_t *) amqp_allocate(&pool);
        CHECK_EQUAL('A', p->important_stuff[0]);
        CHECK_EQUAL('Z', p->important_stuff[25]);
        amqp_deallocate(&pool, p);
    }

#ifdef DISABLE_MEMORY_POOL
    TEST_FIXTURE(InitializedPoolFixture, verify_allocation_works_when_pool_disabled)
    {
        test_type_t *p, *q;
        p = (test_type_t *) amqp_allocate(&pool);
        q = (test_type_t *) amqp_allocate(&pool);
        amqp_deallocate(&pool, p);
        amqp_deallocate(&pool, q);
    }
#else
    TEST_FIXTURE(InitializedPoolFixture, first_allocate_from_pool_allocates_block)
    {
        CHECK_NULL(pool.block_list);

        amqp_memory_pool_t *p = (amqp_memory_pool_t *) amqp_allocate(&pool);
        CHECK_NOT_NULL(p);

        CHECK_NOT_NULL(pool.block_list);
        CHECK_NULL(pool.block_list->header.previous);
        CHECK_NULL(pool.block_list->header.next);

        amqp_deallocate(&pool, p);
    }

    TEST_FIXTURE(InitializedPoolFixture, allocate_from_pool)
    {
        test_type_t *p = (test_type_t *) amqp_allocate(&pool);

        CHECK_NOT_NULL(pool.block_list);
        CHECK_NULL(pool.block_list->header.previous);
        CHECK_NULL(pool.block_list->header.next);

        CHECK_EQUAL(0xfe, pool.block_list->header.mask.bytes[0]);
        CHECK_NOT_NULL(p);

        amqp_deallocate(&pool, p);
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
        allocate_from_pool(7);
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
        CHECK_EQUAL(0xffU, (unsigned) pool.block_list->header.mask.bytes[3]);

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
        break_one();
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
        test_type_t *p = (test_type_t *) amqp_allocate(&pool);
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
        amqp_deallocate(&pool, p);

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
