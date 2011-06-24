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

#define CHECK_ALLOCATIONS() \
    do \
    { \
        try { \
            if (!UnitTest::Check(amqp_assert_that_calls_to_free_match_calls_to_malloc())) \
                UnitTest::CurrentTest::Results()->OnTestFailure(*UnitTest::CurrentTest::Details(), "Number of calls to amqp_free() does not match number of calls to amqp_malloc()."); \
        } \
        catch (...) { \
            UnitTest::CurrentTest::Results()->OnTestFailure(*UnitTest::CurrentTest::Details(), "Unhandled exception in CHECK_ALLOCATIONS()"); \
        } \
    } while (0)

namespace SuiteMemory
{
    MemoryFixture::MemoryFixture()
    {
        amqp_reset_malloc_allocation_stats();
    }
    MemoryFixture::~MemoryFixture()
    {
        CHECK_ALLOCATIONS();
    }
}