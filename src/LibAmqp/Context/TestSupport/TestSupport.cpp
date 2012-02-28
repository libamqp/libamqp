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

#include "TestHarness.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"

#include "Codec/Type/TypePrint.h"
#include "Buffer/BufferTestSupport.h"
#include "Context/TestSupport/TestSupport.h"

namespace t
{
    static int dump_flag(amqp_context_t *context, int value, const char *text, int not_first)
    {
        if (value != 0)
        {
            amqp_context_printf(context, 3, "%c %s", not_first ? ',' : ':', text);
            return true;
        }
        return not_first;
    }

    void dump_type_flags(amqp_context_t *context, amqp_type_t *type)
    {
        int not_first = false;
        amqp_context_printf(context, 3, "flags");
        not_first = dump_flag(context, amqp_type_is_null(type), "is_null", not_first);
        not_first = dump_flag(context, amqp_type_is_invalid(type), "is_invalid", not_first);
        not_first = dump_flag(context, amqp_type_is_encoded(type), "is_encoded", not_first);
        not_first = dump_flag(context, amqp_type_is_incomplete(type), "is_incomplete", not_first);
        not_first = dump_flag(context, amqp_type_is_contained(type), "is_contained", not_first);
        not_first = dump_flag(context, amqp_type_is_descriptor(type), "is_descriptor", not_first);
        not_first = dump_flag(context, amqp_type_is_described(type), "is_described", not_first);
        not_first = dump_flag(context, amqp_type_is_composite(type), "is_composite", not_first);
        not_first = dump_flag(context, amqp_type_is_array(type), "is_array", not_first);
        not_first = dump_flag(context, amqp_type_is_list(type), "is_list", not_first);
        dump_flag(context, amqp_type_is_map(type), "is_map", not_first);
        amqp_context_putc(context, '\n');
    }

    void dump_type(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
    {
        int old_indent;

        amqp_outputter_lock(context);
        amqp_context_printf(context, 3, "format code: 0x%02x\n", type->constructor.format_code);
        old_indent = amqp_context_increase_print_indent(context, 4);
        if (type->constructor.meta_data != 0)
        {
            amqp_context_printf(context, 3, "width: %d, name: %s", type->constructor.meta_data->width, type->constructor.meta_data->name);
            if (*type->constructor.meta_data->encoding_name)
            {
                amqp_context_printf(context, 3, "-%s", type->constructor.meta_data->encoding_name);
            }
            amqp_context_putc(context, '\n');
        }

        dump_type_flags(context, type);

        amqp_buffer_dump_fragment(context, buffer, type->position.index, type->position.index + type->position.size);
        amqp_context_putc(context, '\n');
        amqp_type_print_formatted(context, type);
        amqp_context_set_print_indent(context, old_indent);
        amqp_outputter_unlock(context);
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

    int compare_buffers(amqp_context_t *context, const unsigned char *expect, size_t expect_size, amqp_buffer_t *buffer)
    {
        int rc = compare_buffers(expect, expect_size, amqp_buffer_pointer(buffer, 0), amqp_buffer_size(buffer));
        if (!rc)
        {
            amqp_buffer_dump(context, buffer);
            amqp_context_putc(context, '\n');
        }
        return rc;
    }
}
