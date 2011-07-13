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
    va_list args;
    va_start(args, format);

    char buffer[64];
    vsnprintf(buffer, sizeof(buffer), format, args);

    str_print(context, buffer);
}

static int amqp_type_print_ascii(amqp_context_t *context, amqp_type_t *type)
{
    size_t i;
    for (i = 0; i < type->position.size; i++)
    {
        int c = amqp_unchecked_getc_at(amqp_type_convert_buffer(type), type->position.index + i);
        amqp_context_putc(context, isprint(c) ? c : '?');
    }

    return 1;
}

static int amqp_type_print_raw_data(amqp_context_t *context, amqp_type_t *type)
{
    size_t i;
    for (i = 0; i < type->position.size; i++)
    {
        int c = amqp_unchecked_getc_at(amqp_type_convert_buffer(type), type->position.index + i);
        amqp_context_putc(context, to_hex(c >> 4));
        amqp_context_putc(context, to_hex(c & 0x0f));
    }

    return 1;
}

static inline void print_invalid_type(amqp_type_t *type)
{
    str_print(type->context, "invalid: ");
    amqp_type_print_raw_data(type->context, type);
}

// TODO - change return type
static int amqp_nested_type_print(amqp_type_t *type)
{
    amqp_type_print_method_t *method = type->meta_data->methods->print;
    assert(method != NULL);

    if (type->flags.is_invalid)
    {
        print_invalid_type(type);
    }
    else
    {
        (*method)(type->context, type);
    }

    return 0;
}

static void amqp_type_print_formatted_leader(amqp_type_t *type)
{
    if (type && type->context->config.formatted_print)
    {
        str_print(type->context, type->meta_data->name);
        if (type->meta_data->encoding_name[0])
        {
            amqp_context_putc(type->context, '-');
            str_print(type->context, type->meta_data->encoding_name);
        }
        amqp_context_putc(type->context, ':');
        amqp_context_putc(type->context, ' ');
    }
}


void amqp_type_print(amqp_type_t *type)
{
    amqp_type_print_formatted_leader(type);

    amqp_nested_type_print(type);

    if (type->context->config.formatted_print)
    {
        amqp_context_putc(type->context, ';');
    }
}

void amqp_type_print_formatted(amqp_type_t *type)
{
    int old_formatted_print_flag = type->context->config.formatted_print;

    type->context->config.formatted_print = 1;

    amqp_type_print(type);
    amqp_context_putc(type->context, '\n');

    type->context->config.formatted_print = old_formatted_print_flag;
}

void amqp_type_method_described_print(amqp_context_t *context, amqp_type_t *type)
{
    str_print(context, "{descriptor={");
    amqp_type_print(type->value.described.elements[0]);
    str_print(context, "}, type={");
    amqp_type_print(type->value.described.elements[1]);
    str_print(context, "}}");
}

void amqp_type_method_null_print(amqp_context_t *context, amqp_type_t *type)
{
    str_print(context, "null");
}

void amqp_type_method_boolean_true_print(amqp_context_t *context, amqp_type_t *type)
{
    str_print(context, "true");
}

void amqp_type_method_boolean_false_print(amqp_context_t *context, amqp_type_t *type)
{
    str_print(context, "false");
}

void amqp_type_method_boolean_print(amqp_context_t *context, amqp_type_t *type)
{
    type->value.b1._unsigned == 0x00 ?
            amqp_type_method_boolean_true_print(context, type) :
            amqp_type_method_boolean_false_print(context, type);
}

void amqp_type_method_ubyte_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%u", type->value.b1._unsigned);
}

void amqp_type_method_byte_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%d", type->value.b1._signed);
}

void amqp_type_method_ushort_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%u", type->value.b2._ushort);
}

void amqp_type_method_short_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%d", type->value.b2._short);
}

void amqp_type_method_uint_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%u", type->value.b4._uint);
}

void amqp_type_method_uint0_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_uint_print(context, type);
}

void amqp_type_method_int_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%d", type->value.b4._int);
}

void amqp_type_method_ulong_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%lu", type->value.b8._ulong);
}

void amqp_type_method_ulong0_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_ulong_print(context, type);
}

void amqp_type_method_long_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%ld", type->value.b8._long);
}

void amqp_type_method_small_uint_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%u", type->value.b4._uint);
}

void amqp_type_method_small_int_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%d", type->value.b4._int);
}

void amqp_type_method_small_ulong_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%lu", type->value.b8._ulong);
}

void amqp_type_method_small_long_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%ld", type->value.b8._ulong);
}

void amqp_type_method_float_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%f", type->value.b4._float);
}

void amqp_type_method_double_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "%f", type->value.b8._double);
}

void amqp_type_method_char_print(amqp_context_t *context, amqp_type_t *type)
{
    printf_type_value(context, "U%60x", type->value.b4._wchar);
}

void amqp_type_method_decimal32_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_raw_data(context, type);
}

void amqp_type_method_timestamp_print(amqp_context_t *context, amqp_type_t *type)
{
    int64_t milliseconds_since_epoc = type->value.b8._timestamp;
    
    time_t seconds_since_epoc = milliseconds_since_epoc / 1000;
    int milliseconds = milliseconds_since_epoc % 1000;
    struct tm *utc_time = gmtime(&seconds_since_epoc);
    char buffer [80];

    strftime (buffer,80,"%Y-%m-%dT%H:%M:%S", utc_time);

    printf_type_value(context, "%s.%03dZ", buffer, milliseconds);
}

void amqp_type_method_decimal64_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_raw_data(context, type);
}

void amqp_type_method_uuid_print(amqp_context_t *context, amqp_type_t *type)
{
    static char separator[16] = {0, 0, 0, '-', 0, '-', 0, '-', 0, '-', 0 };
    size_t i;
    for (i = 0; i < type->position.size; i++)
    {
        int c = amqp_unchecked_getc_at(amqp_type_convert_buffer(type), type->position.index + i);
        amqp_context_putc(context, to_hex(c >> 4));
        amqp_context_putc(context, to_hex(c & 0x0f));
        if ((c = separator[i]) != 0)
        {
            amqp_context_putc(context, c);
        }
    }
}

void amqp_type_method_binary_vbin8_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_raw_data(context, type);
}

void amqp_type_method_binary_vbin32_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_raw_data(context, type);
}

void amqp_type_method_symbol_sym8_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_ascii(context, type);
}

void amqp_type_method_symbol_sym32_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_ascii(context, type);
}

void amqp_type_method_string_str8_utf8_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_ascii(context, type);
}

void amqp_type_method_string_str32_utf8_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_print_ascii(context, type);
}

static void amqp_type_method_list_print(amqp_context_t *context, amqp_type_t *type)
{
    size_t i;
    size_t count = type->value.list.count;

    amqp_type_print_formatted_leader(type);

    amqp_context_putc(context, '{');
    for (i = 0; i < count; i++)
    {
        if (i > 0)
        {
            amqp_context_putc(context, ';');
            amqp_context_putc(context, ' ');
        }
        amqp_nested_type_print(type->value.list.elements[i]);
    }
    amqp_context_putc(context, '}');
}


// TODO -  remove me
void amqp_type_method_list_8_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_list_print(context, type);
}

// TODO -  remove me
void amqp_type_method_list_32_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_list_print(context, type);
}

static void amqp_type_method_map_print(amqp_context_t *context, amqp_type_t *type)
{
    size_t i;
    size_t count = type->value.list.count;

    amqp_type_print_formatted_leader(type);

    amqp_context_putc(context, '{');
    for (i = 0; i < count; i += 2)
    {
        if (i > 0)
        {
            str_print(context, ", ");
        }

        amqp_context_putc(context, '{');

        amqp_nested_type_print(type->value.map.entries[i]);
        amqp_context_putc(context, ':');
        amqp_context_putc(context, ':');
        amqp_nested_type_print(type->value.map.entries[i + 1]);

        amqp_context_putc(context, '}');
    }
    amqp_context_putc(context, '}');
}

void amqp_type_method_map_8_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_map_print(context, type);
}

void amqp_type_method_map_32_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_map_print(context, type);
}

static void amqp_type_method_list_array_print(amqp_context_t *context, amqp_type_t *type)
{
    size_t i;

    if (type->value.compound.elements)
    {
        amqp_type_print_formatted_leader(type->value.array.elements[0]);
    }
    else
    {
        str_print(context, "EMPTY ARRAY!");
    }

    amqp_context_putc(context, '[');
    for (i = 0; i < type->value.array.count; i++)
    {
        if (i > 0)
        {
            amqp_context_putc(context, ',');
            amqp_context_putc(context, ' ');
        }
        amqp_nested_type_print(type->value.array.elements[i]);
    }
    amqp_context_putc(context, ']');
}

void amqp_type_method_array_8_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_list_array_print(context, type);
}

void amqp_type_method_array_32_print(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_method_list_array_print(context, type);
}

void amqp_type_method_extension_type_print(amqp_context_t *context, amqp_type_t *type)
{
    not_implemented(method);
}
