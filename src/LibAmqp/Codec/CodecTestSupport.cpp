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

#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "Codec/CodecTestSupport.h"

namespace t
{
    void dump_type_buffer(amqp_type_t *type)
    {
        amqp_context_t *context = type->context;
        amqp_buffer_dump_as_byte_array(type->flags.is_encoded ? context->encode.buffer : context->decode.buffer, context->config.putc);
        (*context->config.putc)('\n');
    }

    // lhs is left hand param passed to macro
    int compare_buffers(const unsigned char *expect, size_t expect_size, const unsigned char *actual, size_t actual_size)
    {
        if (expect_size != actual_size)
        {
            printf("Buffer sizes differ: expect size: %lu, actual size: %lu\n", (unsigned long) expect_size, (unsigned long) actual_size);
            return 0;
        }

        for (size_t i = 0; i < expect_size; i++)
        {
            if (expect[i] != actual[i])
            {
                printf("Buffers differ: expect[%d] = 0x%02x, '%c'; actual[%d] = 0x%02x, '%c'\n",
                    (int) i, expect[i], isprint(expect[i]) ? expect[i] : '-', (int) i, actual[i], isprint(actual[i]) ? actual[i] : '-');
                return 0;
            }
        }
        return 1;
    }
}
