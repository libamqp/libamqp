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

#include <stdlib.h>
#include <string.h>

#include "Memory/Memory.h"
#include "Buffer/Buffer.h"
#include "TestData/TestData.h"

namespace test_data
{
    TestData::TestData(unsigned char * bytes, size_t n) : n_(n)
    {
        bytes_ = (unsigned char *) malloc(n);
        memcpy(bytes_, bytes, n);
    }

    TestData::~TestData()
    {
        free(bytes_);
    }

    void TestData::transfer_to(amqp_buffer_t *buffer) const
    {
        // TODO - throw exception
        amqp_buffer_puts(buffer, bytes_, n_);
    }
}
namespace t
{
    void amqp_buffer_dump(amqp_buffer_t *buffer, int (*putc)(int))
    {
        size_t i;

        for (i = 0; i < buffer->limit.size; i++)
        {
            int c = buffer->bytes[i];
            (*putc)(to_hex(c >> 4));
            (*putc)(to_hex(c & 0x0f));
            if (i > 0 && (i & 0x0f) == 0)
            {
                (*putc)('\n');
            }
        }
    }

    void amqp_buffer_dump_as_byte_array(amqp_buffer_t *buffer, int (*putc)(int))
    {
        size_t i;

        (*putc)('{');
        for (i = 0; i < buffer->limit.size; i++)
        {
            int c = buffer->bytes[i];
            (*putc)('0');
            (*putc)('x');
            (*putc)(to_hex(c >> 4));
            (*putc)(to_hex(c & 0x0f));
            (*putc)(',');
            (*putc)(' ');
            if (i > 0 && (i & 0x0f) == 0)
            {
                (*putc)('\n');
            }
        }
        (*putc)('}');
        (*putc)('\n');
    }

}
