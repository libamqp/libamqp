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

#include <string.h>

#include "Memory/PoolTestSupport.h"

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

namespace SuitePool
{
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
            amqp_initialize_pool_suggesting_block_size(&pool, sizeof(test_type_t), n);
            amqp_pool_specify_initialization_callbacks(&pool, (amqp_pool_callback_t) allocate_callback, deallocate_callback);
        }
    }

    void PoolFixture::initialize_pool()
    {
        if (!pool.initialized)
        {
            amqp_initialize_pool(&pool, sizeof(test_type_t));
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
}

