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

#ifndef LIBAMQP_MEMORY_POOL_TEST_SUPPORT_H
#define LIBAMQP_MEMORY_POOL_TEST_SUPPORT_H

#include <stdlib.h>

#include "libamqp_common.h"
#include <TestHarness.h>
#include "Memory/MemoryTestSupport.h"
#include "Memory/Pool.h"
#include "Memory/PoolInternal.h"

namespace SuitePool
{
    typedef struct test_type_t
    {
        char important_stuff[32];
    } test_type_t;

    class PoolAllocator
    {
    public:
        PoolAllocator(amqp_memory_pool_t *pool_);
        ~PoolAllocator();
        test_type_t *allocate_one();
        void remove_one();

    private:
        int count, capacity;
        amqp_memory_pool_t *pool;
        test_type_t **allocations;
    };

    // TODO -  exposing too much from pool internal tests to other suites.
    class PoolFixture : public SuiteMemory::MemoryFixture
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
}

#endif

