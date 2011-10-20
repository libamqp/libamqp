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

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "libamqp_common.h"

#include "Buffer/Buffer.h"
#include "Memory/Memory.h"
#include "Codec/Type/TypePrint.h"
#include "Context/ErrorHandling.h"
#include "Codec/Convert.h"

static void str_print(amqp_context_t *context, const char *s)
{
    while (*s)
    {
        amqp_context_putc(context, *s++);
    }
}

static void printf_type_value(amqp_context_t *context, const char *format, ...)
{
    char buffer[64];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    str_print(context, buffer);
}

static int amqp_type_print_ascii(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    size_t i;
    for (i = 0; i < type->position.size; i++)
    {
        int c = amqp_unchecked_getc_at(buffer, type->position.index + i);
        amqp_context_putc(context, isprint(c) ? c : '?');
    }

    return 1;
}

static int amqp_type_raw_dump(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    size_t i, j;
    for (i = 0, j = 0; i < type->position.size; i++, j++)
    {
        if (j == 32)
        {
            amqp_context_putc(context, '\n');
            j = 0;
        }
        else if (j > 0)
        {
            amqp_context_putc(context, ' ');
        }
        int c = amqp_unchecked_getc_at(buffer, type->position.index + i);
        amqp_context_putc(context, to_hex(c >> 4));
        amqp_context_putc(context, to_hex(c & 0x0f));
    }

    return 1;
}

static int amqp_type_print_raw_data_line(amqp_context_t *context, size_t index, amqp_type_t *type, amqp_buffer_t *buffer)
{
    size_t j = 0;
    size_t i = index;

    while (i < type->position.size && j < 32)
    {
        int c = amqp_unchecked_getc_at(buffer, type->position.index + i);
        amqp_context_putc(context, to_hex(c >> 4));
        amqp_context_putc(context, to_hex(c & 0x0f));
        amqp_context_putc(context, ' ');
        i++;
        j++;
    }
    while (j < 32)
    {
        amqp_context_putc(context, ' ');
        amqp_context_putc(context, ' ');
        amqp_context_putc(context, ' ');
        j++;
    }
    amqp_context_putc(context, ' ');
    j = 0;
    while (index < type->position.size && j < 32)
    {
        int c = amqp_unchecked_getc_at(buffer, type->position.index + index);
        amqp_context_putc(context, isprint(c) ? c : '.');
        index++;
        j++;
    }

    return 1;
}

static int amqp_type_print_raw_data(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    size_t i = 0;

    if (type->position.size == 0)
    {
        return 1;
    }

    amqp_type_print_raw_data_line(context, i, type, buffer);
    i += 32;

    while (i < type->position.size)
    {
        amqp_context_putc(context, '\n');
        amqp_type_print_raw_data_line(context, i, type, buffer);
        i += 32;
    }

    return 1;
}

static inline void print_invalid_type(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    str_print(context, "invalid: ");
    amqp_type_raw_dump(context, type, buffer);
}

// TODO - change return type
static int amqp_nested_type_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_method_t *method = type->meta_data->methods->print;
    assert(method != NULL);

    if (type->flags.is_invalid)
    {
        print_invalid_type(context, type, buffer);
    }
    else
    {
        (*method)(context, type, buffer);
    }

    return 0;
}

static void amqp_type_print_formatted_leader(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    if (type && context->config.formatted_print)
    {
        str_print(context, type->meta_data->name);
        if (type->meta_data->encoding_name[0])
        {
            amqp_context_putc(context, '-');
            str_print(context, type->meta_data->encoding_name);
        }
        amqp_context_putc(context, ':');
        amqp_context_putc(context, ' ');
    }
}

void amqp_type_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    int old_auto_indent = amqp_context_set_auto_indent(context, 4);
    amqp_type_print_formatted_leader(context, type, buffer);

    amqp_nested_type_print(context, type, buffer);

    if (context->config.formatted_print)
    {
        amqp_context_putc(context, ';');
    }
    amqp_context_set_auto_indent(context, old_auto_indent);
}

void amqp_type_print_formatted(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    int old_formatted_print_flag = context->config.formatted_print;

    context->config.formatted_print = 1;

    amqp_type_print(context, type, buffer);
    amqp_context_putc(context, '\n');

    context->config.formatted_print = old_formatted_print_flag;
}

void amqp_type_method_described_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    str_print(context, "{descriptor={");
    amqp_type_print(context, type->value.described.elements[0], buffer);
    str_print(context, "}, type={");
    amqp_type_print(context, type->value.described.elements[1], buffer);
    str_print(context, "}}");
}

void amqp_type_method_null_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    str_print(context, "null");
}

void amqp_type_method_boolean_true_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    str_print(context, "true");
}

void amqp_type_method_boolean_false_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    str_print(context, "false");
}

void amqp_type_method_boolean_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    type->value.b1._unsigned == 0x00 ?
            amqp_type_method_boolean_true_print(context, type, buffer) :
            amqp_type_method_boolean_false_print(context, type, buffer);
}

void amqp_type_method_ubyte_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%u", type->value.b1._unsigned);
}

void amqp_type_method_byte_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%d", type->value.b1._signed);
}

void amqp_type_method_ushort_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%u", type->value.b2._ushort);
}

void amqp_type_method_short_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%d", type->value.b2._short);
}

void amqp_type_method_uint_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%u", type->value.b4._uint);
}

void amqp_type_method_uint0_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_method_uint_print(context, type, buffer);
}

void amqp_type_method_int_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%d", type->value.b4._int);
}

void amqp_type_method_ulong_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%lu", type->value.b8._ulong);
}

void amqp_type_method_ulong0_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_method_ulong_print(context, type, buffer);
}

void amqp_type_method_long_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%ld", type->value.b8._long);
}

void amqp_type_method_small_uint_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%u", type->value.b4._uint);
}

void amqp_type_method_small_int_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%d", type->value.b4._int);
}

void amqp_type_method_small_ulong_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%lu", type->value.b8._ulong);
}

void amqp_type_method_small_long_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%ld", type->value.b8._ulong);
}

void amqp_type_method_float_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%f", type->value.b4._float);
}

void amqp_type_method_double_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "%f", type->value.b8._double);
}

void amqp_type_method_char_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    printf_type_value(context, "U%60x", type->value.b4._wchar);
}

void amqp_type_method_decimal32_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_raw_data(context, type, buffer);
}

void amqp_type_method_timestamp_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    int64_t milliseconds_since_epoc = type->value.b8._timestamp;
    time_t seconds_since_epoc = milliseconds_since_epoc / 1000;
    int milliseconds = milliseconds_since_epoc % 1000;
    struct tm *utc_time = gmtime(&seconds_since_epoc);
    char print_buffer[80];

    strftime (print_buffer, 80, "%Y-%m-%dT%H:%M:%S", utc_time);

    printf_type_value(context, "%s.%03dZ", print_buffer, milliseconds);
}

void amqp_type_method_decimal64_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_raw_data(context, type, buffer);
}

void amqp_type_method_decimal128_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_raw_data(context, type, buffer);
}

void amqp_type_method_uuid_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    static char separator[16] = {0, 0, 0, '-', 0, '-', 0, '-', 0, '-', 0 };
    size_t i;
    for (i = 0; i < type->position.size; i++)
    {
        int c = amqp_unchecked_getc_at(buffer, type->position.index + i);
        amqp_context_putc(context, to_hex(c >> 4));
        amqp_context_putc(context, to_hex(c & 0x0f));
        if ((c = separator[i]) != 0)
        {
            amqp_context_putc(context, c);
        }
    }
}

void amqp_type_method_binary_vbin8_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_raw_data(context, type, buffer);
}

void amqp_type_method_binary_vbin32_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_raw_data(context, type, buffer);
}

void amqp_type_method_symbol_sym8_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_ascii(context, type, buffer);
}

void amqp_type_method_symbol_sym32_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_ascii(context, type, buffer);
}

void amqp_type_method_string_str8_utf8_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_ascii(context, type, buffer);
}

void amqp_type_method_string_str32_utf8_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    amqp_type_print_ascii(context, type, buffer);
}

void amqp_type_method_list_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    size_t i;
    size_t count = type->value.list.count;

    amqp_context_putc(context, '{');
    amqp_context_putc(context, '\n');
    for (i = 0; i < count; i++)
    {
        if (i > 0)
        {
            amqp_context_putc(context, ',');
            amqp_context_putc(context, '\n');
        }
        amqp_nested_type_print(context, type->value.list.elements[i], buffer);
    }
    amqp_context_putc(context, '\n');
    amqp_context_putc(context, '}');
}

void amqp_type_method_map_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    size_t i;
    size_t count = type->value.list.count;

    amqp_context_putc(context, '{');
    amqp_context_putc(context, '\n');
    for (i = 0; i < count; i += 2)
    {
        if (i > 0)
        {
            str_print(context, ",");
            amqp_context_putc(context, '\n');
        }
        amqp_nested_type_print(context, type->value.map.entries[i], buffer);
        amqp_context_putc(context, '-');
        amqp_context_putc(context, '>');
        amqp_nested_type_print(context, type->value.map.entries[i + 1], buffer);
    }
    amqp_context_putc(context, '\n');
    amqp_context_putc(context, '}');
}

void amqp_type_method_array_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    size_t i;

//    if (type->value.compound.elements)
//    {
//        amqp_type_print_formatted_leader(context, type->value.array.elements[0], buffer);
//    }
//    else
//    {
//        str_print(context, "EMPTY ARRAY!");
//    }

    amqp_context_putc(context, '[');
//    amqp_context_putc(context, '\n');
    for (i = 0; i < type->value.array.count; i++)
    {
        if (i > 0)
        {
            amqp_context_putc(context, ',');
            amqp_context_putc(context, ' ');
        }
        amqp_nested_type_print(context, type->value.array.elements[i], buffer);
    }
//    amqp_context_putc(context, '\n');
    amqp_context_putc(context, ']');
}

void amqp_type_method_extension_type_print(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer)
{
    not_implemented(method);
}
