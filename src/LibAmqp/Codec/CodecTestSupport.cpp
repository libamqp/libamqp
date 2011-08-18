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

#include "TestHarness.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "Buffer/BufferTestSupport.h"
#include "Codec/CodecTestSupport.h"
#include "Codec/Type/TypePrint.h"
#include "Context/Context.h"

namespace SuiteCodec
{
    CodecFixture::CodecFixture() : type(0) {}
    CodecFixture::~CodecFixture()
    {
        deallocate_type(type);
    }

    amqp_memory_pool_t *
    CodecFixture::type_pool()
    {
        return &context->pools.amqp_type_t_pool;
    }

    amqp_type_t *
    CodecFixture::allocate_type()
    {
        return (amqp_type_t *) amqp_allocate(context, type_pool());
    }

    void
    CodecFixture::deallocate_type(amqp_type_t *type)
    {

        amqp_deallocate(context, type_pool(), type);
    }

    bool
    CodecFixture::check_valid_array()
    {
        return type != null &&
            amqp_type_is_valid(type) &&
            amqp_type_is_container(type) &&
            amqp_type_is_array(type) &&
            !(amqp_type_is_map(type) ||
            amqp_type_is_list(type));
    }
}

namespace t
{
    static int dump_flag(amqp_context_t *context, int value, const char *text, int not_first)
    {
        if (value != 0)
        {
            amqp_context_printf(context, "%c %s", not_first ? ',' : ':', text);
            return true;
        }
        return not_first;
    }

    void dump_type_flags(amqp_type_t *type)
    {
        int not_first = false;
        amqp_context_printf(type->context, "flags");
        not_first = dump_flag(type->context, type->flags.is_null, "is_null", not_first);
        not_first = dump_flag(type->context, type->flags.is_invalid, "is_invalid", not_first);
        not_first = dump_flag(type->context, type->flags.is_encoded, "is_encoded", not_first);
        not_first = dump_flag(type->context, type->flags.is_incomplete, "is_incomplete", not_first);
        not_first = dump_flag(type->context, type->flags.is_contained, "is_contained", not_first);
        not_first = dump_flag(type->context, type->flags.is_descriptor, "is_descriptor", not_first);
        not_first = dump_flag(type->context, type->flags.has_descriptor, "has_descriptor", not_first);
        not_first = dump_flag(type->context, type->flags.container.type.is_array, "is_array", not_first);
        not_first = dump_flag(type->context, type->flags.container.type.is_list, "is_list", not_first);
        dump_flag(type->context, type->flags.container.type.is_map, "is_map", not_first);
        amqp_context_putc(type->context, '\n');
    }

    void dump_type(amqp_type_t *type)
    {
        amqp_context_t *context = type->context;
        amqp_buffer_t *buffer = type->flags.is_encoded ? context->encode.buffer : context->decode.buffer;
        int old_indent;

        amqp_context_printf(context, "format code: 0x%02x\n", type->format_code);
        old_indent = amqp_context_increase_print_indent(context, 4);
        if (type->meta_data != 0)
        {
            amqp_context_printf(context, "width: %d, name: %s", type->meta_data->width, type->meta_data->name);
            if (*type->meta_data->encoding_name)
            {
                amqp_context_printf(context, "-%s", type->meta_data->encoding_name);
            }
            amqp_context_putc(context, '\n');
        }

        dump_type_flags(type);

        amqp_buffer_dump_fragment(context, buffer, type->position.index, type->position.index + type->position.size);
        amqp_context_putc(context, '\n');
        amqp_type_print_formatted(type);
        amqp_context_set_print_indent(context, old_indent);
    }

    void dump_type_buffer(amqp_type_t *type)
    {
        amqp_context_t *context = type->context;
        amqp_buffer_dump(context, type->flags.is_encoded ? context->encode.buffer : context->decode.buffer);
        amqp_context_putc(context, '\n');
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
