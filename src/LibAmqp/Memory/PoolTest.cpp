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
#include "ExtraChecks.h"

#include "debug_helper.h"

SUITE(Pool)
{
    TEST_FIXTURE(PoolFixture, a_test)
    {
    }

    TEST_FIXTURE(PoolFixture, allocation_with_unitialized_pool_should_fail)
    {
        void *p = amqp_allocate(&pool);
        CHECK_NULL(p);
    }

    TEST_FIXTURE(PoolFixture, pool_initialization)
    {
        amqp_initialize_pool(&pool, sizeof(test_type_t));

        CHECK_EQUAL(LONG_BIT, pool.allocations_per_block);
        SOUTV(pool.allocations_per_block);

        CHECK_EQUAL(-1UL, pool.allocations_mask);
        SOUTVX(pool.allocations_mask);

        CHECK(pool.safe_alignment == 4 || pool.safe_alignment == 16);
        SOUTV(pool.safe_alignment);

        CHECK_EQUAL(0U, (pool.allocation_data_padding + sizeof(amqp_memory_allocation_header_t)) % pool.safe_alignment);
        SOUTV(pool.allocation_data_padding);

        CHECK_EQUAL(0U, (pool.block_data_padding + sizeof(amqp_memory_block_header_t)) % pool.safe_alignment);
        SOUTV(pool.block_data_padding);

        CHECK_EQUAL(sizeof(test_type_t), pool.object_size);

        size_t allocation_size = sizeof(test_type_t) + pool.allocation_data_padding + sizeof(amqp_memory_allocation_header_t);
        CHECK_EQUAL(allocation_size, pool.allocation_size);
    }

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

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_64)
    {
        initialize_pool();
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_8)
    {
        initialize_pool(8);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_16)
    {
        initialize_pool(16);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_32)
    {
        initialize_pool(32);
        test();
    }
    
    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_2)
    {
        initialize_pool(2);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_7)
    {
        initialize_pool(7);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_9)
    {
        initialize_pool(9);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_11)
    {
        initialize_pool(11);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_15)
    {
        initialize_pool(15);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_17)
    {
        initialize_pool(17);
        test();
    }
    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_31)
    {
        initialize_pool(31);
        test();
    }
    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_33)
    {
        initialize_pool(33);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, allocation_beyond_block_capacity_should_allocate_another_block_63)
    {
        initialize_pool(63);
        test();
    }

    TEST_FIXTURE(BlockChainingTestFixture, free_allocation_in_block_should_result_in_block_removal_from_chain)
    {
        initialize_pool(2);

        allocate_from_pool(2);
        amqp_memory_block_t *first_block = pool.block_list;

        test_type_t *p = (test_type_t *) amqp_allocate(&pool);
        test_type_t *q = (test_type_t *) amqp_allocate(&pool);

        amqp_memory_block_t *second_block = pool.block_list;

        allocate_from_pool(2);
        amqp_memory_block_t *third_block = pool.block_list;

        CHECK_NULL(first_block->header.next);
        CHECK_EQUAL(second_block, first_block->header.previous);
        CHECK_EQUAL(first_block, second_block->header.next);
        CHECK_EQUAL(third_block, second_block->header.previous);
        CHECK_EQUAL(second_block, third_block->header.next);
        CHECK_NULL(third_block->header.previous);

        amqp_deallocate(&pool, p);
        amqp_deallocate(&pool, q);

        // verify that the second block has been removed from the block list
        // because all its allocation have been deallocated
        CHECK_EQUAL(third_block, pool.block_list);
        CHECK_NULL(third_block->header.previous);
        CHECK_EQUAL(first_block, third_block->header.next);
        CHECK_NULL(first_block->header.next);
        CHECK_EQUAL(third_block, first_block->header.previous);

        // now remove the last allocations from the pool causing the third block to be removed from the list.
        return_last_allocated_to_pool();
        return_last_allocated_to_pool();
        CHECK_EQUAL(first_block, pool.block_list);
        CHECK_NULL(first_block->header.previous);
        CHECK_NULL(first_block->header.next);
    }

    void allocate_callback(amqp_memory_pool_t *pool, test_type_t *object)
    {
        for (int i = 0; i < 32; i++)
        {
            object->important_stuff[i] = 'A' + i;
        }
    }
    void deallocate_callback(amqp_memory_pool_t *pool, void *object)
    {
    }

    TEST_FIXTURE(InitializedPoolFixture, callback_should_initialize_onject)
    {
        amqp_pool_specify_initialization_callbacks(&pool, (amqp_pool_callback_t) allocate_callback, deallocate_callback);
        test_type_t *t = (test_type_t *) allocate_from_pool();
        CHECK_EQUAL('A', t->important_stuff[0]);
        CHECK_EQUAL('Z', t->important_stuff[25]);
    }
}
