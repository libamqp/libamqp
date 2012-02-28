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

#ifndef LIBAMQP_BUFFER_BUFFER_TEST_FIXTURE_H
#define LIBAMQP_BUFFER_BUFFER_TEST_FIXTURE_H

#include "Memory/Pool.h"
#include "Buffer/Buffer.h"

namespace SuiteBuffer
{
    class BufferFixture
    {
    public:
        BufferFixture()
        {
            amqp_buffer_initialize_pool(&pool);
            buffer = (amqp_buffer_t *) amqp_allocate(0, &pool);
        }
        ~BufferFixture()
        {
            amqp_deallocate(0, &pool, buffer);
        }
        void load_buffer(unsigned char *p, size_t n)
        {
            amqp_buffer_puts(buffer, p, n);
        }
    public:
        amqp_memory_pool_t pool;
        amqp_buffer_t *buffer;
    };
}

#endif


