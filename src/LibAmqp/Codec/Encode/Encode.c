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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


#include "Memory/Pool.h"
#include "Buffer/Buffer.h"
#include "Codec/Encode/Encode.h"
#include "Context/Context.h"

static const amqp_type_type_flags_t not_a_compound_type = {0};

/*
    constructor is not emitted for array elements after the first
 */
static inline int emit_constructor(amqp_context_t *context)
{
    amqp_type_t *type = context->encode.container;
    return type == 0 || (amqp_type_is_compound(type) && !amqp_type_is_array(type)) || (amqp_type_is_array(type)  && type->value.array.count == 0);
}

static inline amqp_type_t **allocate_elements(amqp_type_t **elements, size_t count)
{
    return elements == 0 ? amqp_allocate_amqp_type_t_array(1) : amqp_realloc_amqp_type_t_array(elements, count);
}

static inline void amqp_add_element_to_container(amqp_context_t *context, amqp_type_t *type)
{
    if (context->encode.container)
    {
        size_t count = context->encode.container->value.compound.count++;

        // container is now responsible for deallocating this type
        type->flags.is_contained = true;

        if (amqp_type_is_array(context->encode.container) && count > 0)
        {
            // this is not the first element being added to an array. We must ensure that the type of this element
            // matches the type of the first element in the array
            if (!amqp_type_match(context->encode.container->value.compound.elements[0], type))
            {
                // marke both the element and container as invalid
                amqp_mark_type_invalid(type, AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT);
                amqp_mark_type_invalid(context->encode.container, AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT);
            }
        }
        if (amqp_type_is_described(context->encode.container))
        {
            switch (count)
            {
            case 0:
                type->flags.is_descriptor = true;
                break;

            case 1:
                type->flags.has_descriptor = true;
                break;
            }
        }
        context->encode.container->value.compound.elements = allocate_elements(context->encode.container->value.compound.elements, count + 1);
        context->encode.container->value.compound.elements[count] = type;
    }
}

static void push_container(amqp_context_t *context, amqp_type_t *type)
{
    type->flags.is_incomplete = true;
    type->value.compound.saved_container = context->encode.container;
    context->encode.container = type;
}

static amqp_type_t *pop_container(amqp_context_t *context, amqp_type_t *cc)
{
    amqp_type_t *result = context->encode.container;

    result->flags.is_incomplete = false;
    context->encode.container = result->value.compound.saved_container;
    result->value.compound.saved_container = 0;
    return result;
}

static amqp_type_t *initialize_type(amqp_context_t *context, const amqp_type_type_flags_t type_flags, amqp_type_meta_data_t *meta_data)
{
    amqp_type_t *type = (amqp_type_t *) amqp_allocate(&context->pools.amqp_type_t_pool);

    type->format_code = meta_data->format_code;
    type->context = context;
    type->flags.is_encoded = true;
    type->meta_data = meta_data;

    type->flags.structure.flags = type_flags;

    if (emit_constructor(context))
    {
        amqp_buffer_putc(context->encode.buffer, meta_data->format_code);
    }

    amqp_add_element_to_container(context, type);

    if (amqp_type_is_compound(type))
    {
        push_container(context, type);
    }

    return type;
}

static amqp_type_t *amqp_encode_fixed(amqp_context_t *context, amqp_type_meta_data_t *meta_data)
{
    amqp_type_t  *type;

    if (amqp_buffer_check_free_space(context->encode.buffer, meta_data->width + 1) == 0)
    {
        return NULL;
    }

    type = initialize_type(context, not_a_compound_type, meta_data);

    type->position.index = context->encode.buffer->limit.size;
    type->position.size = meta_data->width;

    return type;
}

static void amqp_hton_uint(amqp_context_t *context, uint32_t value)
{
    amqp_four_byte_t v;
    v._uint = value;
    amqp_hton_32(context->encode.buffer, v);
}

static amqp_type_t *amqp_encode_simple_variable(amqp_context_t *context, amqp_type_meta_data_t *meta_data, const unsigned char *value, size_t size)
{
    amqp_type_t  *type;

    if (amqp_buffer_check_free_space(context->encode.buffer, meta_data->width + 1 + size) == 0)
    {
        return NULL;
    }

    type = initialize_type(context, not_a_compound_type, meta_data);

    switch (meta_data->width)
    {
    case 1:
        amqp_unchecked_putc(context->encode.buffer, (unsigned char) (size & 0xff));
        break;

    case 4:
        amqp_hton_uint(context, (uint32_t) size);
        break;

    default:
        abort();
    }

    type->position.index = context->encode.buffer->limit.size;
    type->position.size = size;
    amqp_buffer_puts(context->encode.buffer, value, size);

    return type;
}

static amqp_type_t *encode_compound_type(amqp_context_t *context, amqp_type_type_flags_t type_flags, amqp_type_meta_data_t *meta_data)
{
    amqp_type_t  *type;

    // check minimum space requirement, not a lot of use really
    if (amqp_buffer_check_free_space(context->encode.buffer, meta_data->width * 2 + 2) == 0)
    {
        abort();
        return NULL;
    }

    type = initialize_type(context, type_flags, meta_data);

    switch (meta_data->width)
    {
    case 0:
        type->position.index = context->encode.buffer->limit.size;
        break;

    case 1:
        amqp_unchecked_putc(context->encode.buffer, (unsigned char) 1);
        type->position.index = context->encode.buffer->limit.size;
        amqp_unchecked_putc(context->encode.buffer, (unsigned char) 0);
        break;

    case 4:
        amqp_hton_uint(context, (uint32_t) 4);
        type->position.index = context->encode.buffer->limit.size;
        amqp_hton_uint(context, (uint32_t) 0);
        break;

    default:
        abort();
    }

    return type;
}

amqp_type_t *amqp_encode_null(amqp_context_t *context)
{
    amqp_type_t *type = amqp_encode_fixed(context, &amqp_type_meta_data_null);

    if (type)
    {
        type->flags.is_null = 1;
    }

    return type;
}

amqp_type_t *amqp_encode_boolean(amqp_context_t *context, int value)
{
    amqp_type_meta_data_t *meta_data = value ? &amqp_type_meta_data_boolean_true : &amqp_type_meta_data_boolean_false;
    return amqp_encode_fixed(context, meta_data);
}

static amqp_type_t *amqp_encode_fixed_one_byte(amqp_context_t *context, amqp_type_meta_data_t *meta_data, unsigned char b)
{
    amqp_type_t *type = amqp_encode_fixed(context, meta_data);

    if (type)
    {
        amqp_unchecked_putc(context->encode.buffer, b);
    }

    return type;
}

amqp_type_t *amqp_encode_ubyte(amqp_context_t *context, uint8_t value)
{
    return amqp_encode_fixed_one_byte(context, &amqp_type_meta_data_ubyte, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_byte(amqp_context_t *context, int8_t value)
{
    return amqp_encode_fixed_one_byte(context, &amqp_type_meta_data_byte, (unsigned char) (value & 0xff));
}

static amqp_type_t *amqp_encode_fixed_two_byte(amqp_context_t *context, amqp_type_meta_data_t *meta_data, amqp_two_byte_t value)
{
    amqp_type_t *type = amqp_encode_fixed(context, meta_data);

    if (type)
    {
        amqp_hton_16(context->encode.buffer, value);
    }

    return type;
}

amqp_type_t *amqp_encode_ushort(amqp_context_t *context, uint16_t value)
{
    amqp_two_byte_t v;
    v._ushort = value;
    return amqp_encode_fixed_two_byte(context, &amqp_type_meta_data_ushort, v);
}

amqp_type_t *amqp_encode_short(amqp_context_t *context, int16_t value)
{
    amqp_two_byte_t v;
    v._short = value;
    return amqp_encode_fixed_two_byte(context, &amqp_type_meta_data_short, v);
}

static amqp_type_t *amqp_encode_fixed_four_byte(amqp_context_t *context, amqp_type_meta_data_t *meta_data, amqp_four_byte_t value)
{
    amqp_type_t *type = amqp_encode_fixed(context, meta_data);

    if (type)
    {
        amqp_hton_32(context->encode.buffer, value);
    }

    return type;
}

amqp_type_t *amqp_encode_small_int(amqp_context_t *context, int32_t value)
{
    return amqp_encode_fixed_one_byte(context, &amqp_type_meta_data_int_small_int, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_int(amqp_context_t *context, int32_t value)
{
    if (value == (signed char) (value & 0xff))
    {
        return amqp_encode_small_int(context, value);
    }
    else
    {
        amqp_four_byte_t v;
        v._int = value;
        return amqp_encode_fixed_four_byte(context, &amqp_type_meta_data_int, v);
    }
}

amqp_type_t *amqp_encode_small_uint(amqp_context_t *context, uint32_t value)
{
    return amqp_encode_fixed_one_byte(context, &amqp_type_meta_data_uint_small_uint, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_uint0(amqp_context_t *context)
{
    amqp_type_t *type = amqp_encode_fixed(context, &amqp_type_meta_data_uint_uint0);
    return type;
}

amqp_type_t *amqp_encode_uint(amqp_context_t *context, uint32_t value)
{
    if (value == 0)
    {
        return amqp_encode_uint0(context);
    }
    else if (value < 255)
    {
        return amqp_encode_small_uint(context, value);
    }
    else
    {
        amqp_four_byte_t v;
        v._uint = value;
        return amqp_encode_fixed_four_byte(context, &amqp_type_meta_data_uint, v);
    }
}

static amqp_type_t *amqp_encode_fixed_eight_byte(amqp_context_t *context, amqp_type_meta_data_t *meta_data, amqp_eight_byte_t value)
{
    amqp_type_t *type = amqp_encode_fixed(context, meta_data);

    if (type)
    {
        amqp_hton_64(context->encode.buffer, value);
    }

    return type;
}

amqp_type_t *amqp_encode_ulong0(amqp_context_t *context)
{
    amqp_type_t *type = amqp_encode_fixed(context, &amqp_type_meta_data_ulong_ulong0);
    return type;
}

amqp_type_t *amqp_encode_small_ulong(amqp_context_t *context, uint64_t value)
{
    return amqp_encode_fixed_one_byte(context, &amqp_type_meta_data_ulong_small_ulong, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_ulong(amqp_context_t *context, uint64_t value)
{
    if (value == 0)
    {
        return amqp_encode_ulong0(context);
    }
    else if (value < 255)
    {
        return amqp_encode_small_ulong(context, value);
    }
    else
    {
        amqp_eight_byte_t v;
        v._ulong = value;
        return amqp_encode_fixed_eight_byte(context, &amqp_type_meta_data_ulong, v);
    }
}

amqp_type_t *amqp_encode_small_long(amqp_context_t *context, int64_t value)
{
    return amqp_encode_fixed_one_byte(context, &amqp_type_meta_data_long_small_long, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_long(amqp_context_t *context, int64_t value)
{
    if (value == (signed char) (value & 0xff))
    {
        return amqp_encode_small_long(context, value);
    }
    else
    {
        amqp_eight_byte_t v;
        v._long = value;
        return amqp_encode_fixed_eight_byte(context, &amqp_type_meta_data_long, v);
    }
}

amqp_type_t *amqp_encode_timestamp(amqp_context_t *context, amqp_timestamp_t value)
{
    amqp_eight_byte_t v;
    v._timestamp = value;
    return amqp_encode_fixed_eight_byte(context, &amqp_type_meta_data_timestamp_ms64, v);
}

amqp_type_t *amqp_encode_float(amqp_context_t *context, float value)
{
    amqp_four_byte_t v;
    v._float = value;
    return amqp_encode_fixed_four_byte(context, &amqp_type_meta_data_float_ieee_754, v);
}

amqp_type_t *amqp_encode_double(amqp_context_t *context, double value)
{
    amqp_eight_byte_t v;
    v._double = value;
    return amqp_encode_fixed_eight_byte(context, &amqp_type_meta_data_double_ieee_754, v);
}

amqp_type_t *amqp_encode_char(amqp_context_t *context, wchar_t value)
{
   return 0; // TODO - implement me
}

amqp_type_t *amqp_encode_decimal32(amqp_context_t *context, amqp_decimal32_t value)
{
   return 0; // TODO - implement me
}

amqp_type_t *amqp_encode_decimal64(amqp_context_t *context, amqp_decimal64_t value)
{
   return 0; // TODO - implement me
}

amqp_type_t *amqp_encode_uuid(amqp_context_t *context, amqp_uuid_t *value)
{
    amqp_type_t *type = amqp_encode_fixed(context, &amqp_type_meta_data_uuid);
    if (type)
    {
        amqp_buffer_puts(context->encode.buffer, (unsigned char *) value, sizeof(amqp_uuid_t));
    }
    return type;
}


amqp_type_t *amqp_encode_binary(amqp_context_t *context, const unsigned char *value, size_t size)
{
   return size < 256 ? amqp_encode_binary_vbin8(context, value, size) : amqp_encode_binary_vbin32(context, value, size);
}

amqp_type_t *amqp_encode_binary_vbin8(amqp_context_t *context, const unsigned char *value, size_t size)
{
    return amqp_encode_simple_variable(context, &amqp_type_meta_data_binary_vbin8, value, size);
}

amqp_type_t *amqp_encode_binary_vbin32(amqp_context_t *context, const unsigned char *value, size_t size)
{
    return amqp_encode_simple_variable(context, &amqp_type_meta_data_binary_vbin32, value, size);
}

amqp_type_t *amqp_encode_symbol_sym8n(amqp_context_t *context, const char *value, size_t size)
{
    return amqp_encode_simple_variable(context, &amqp_type_meta_data_symbol_sym8, (const unsigned char *) value, size);
}

amqp_type_t *amqp_encode_symbol_sym32n(amqp_context_t *context, const char *value, size_t size)
{
    return amqp_encode_simple_variable(context, &amqp_type_meta_data_symbol_sym32, (const unsigned char *) value, size);
}

amqp_type_t *amqp_encode_symboln(amqp_context_t *context, const char *value, size_t size)
{
   return size < 256 ? amqp_encode_symbol_sym8n(context, value, size) : amqp_encode_symbol_sym32n(context, value, size);
}

amqp_type_t *amqp_encode_symbol_sym8(amqp_context_t *context, const char *value)
{
   return amqp_encode_symbol_sym8n(context, value, strlen(value));
}

amqp_type_t *amqp_encode_symbol_sym32(amqp_context_t *context, const char *value)
{
   return amqp_encode_symbol_sym32n(context, value, strlen(value));
}

amqp_type_t *amqp_encode_symbol(amqp_context_t *context, const char *value)
{
   return amqp_encode_symboln(context, value, strlen(value));
}

static inline
amqp_type_t *encode_string_utf8(amqp_context_t *context, const char *value, size_t size)
{
    // TODO - validate
    return amqp_encode_simple_variable(context, size < 256 ? &amqp_type_meta_data_string_str8_utf8 : &amqp_type_meta_data_string_str32_utf8, (const unsigned char *) value, size);
}

amqp_type_t *amqp_encode_string_utf8(amqp_context_t *context, const char *value)
{
   return encode_string_utf8(context, value, strlen(value));
}

amqp_type_t *amqp_encode_string_utf8n(amqp_context_t *context, const char *value, size_t size)
{
   return encode_string_utf8(context, value, size);
}

amqp_type_t *amqp_encode_list_8(amqp_context_t *context)
{
    amqp_type_type_flags_t type_flags = {0};
    type_flags.is_list = true;
    return encode_compound_type(context, type_flags, &amqp_type_meta_data_list_8);
}

amqp_type_t *amqp_encode_list_32(amqp_context_t *context)
{
    amqp_type_type_flags_t type_flags = {0};
    type_flags.is_list = true;
    return encode_compound_type(context, type_flags, &amqp_type_meta_data_list_32);
}

amqp_type_t *amqp_encode_array_8(amqp_context_t *context)
{
    amqp_type_type_flags_t type_flags = {0};
    type_flags.is_array = true;
    return encode_compound_type(context, type_flags, &amqp_type_meta_data_array_8);
}

amqp_type_t *amqp_encode_array_32(amqp_context_t *context)
{
    amqp_type_type_flags_t type_flags = {0};
    type_flags.is_array = true;
    return encode_compound_type(context, type_flags, &amqp_type_meta_data_array_32);
}

amqp_type_t *amqp_encode_map_8(amqp_context_t *context)
{
    amqp_type_type_flags_t type_flags = {0};
    type_flags.is_map = true;
    return encode_compound_type(context, type_flags, &amqp_type_meta_data_map_8);
}

amqp_type_t *amqp_encode_map_32(amqp_context_t *context)
{
    amqp_type_type_flags_t type_flags = {0};
    type_flags.is_map = true;
    return encode_compound_type(context, type_flags, &amqp_type_meta_data_map_32);
}

amqp_type_t *amqp_start_encode_described_type(amqp_context_t *context)
{
    amqp_type_type_flags_t type_flags = {0};
    type_flags.is_described = true;
    return encode_compound_type(context, type_flags, &amqp_type_meta_data_described);
}

amqp_type_t *amqp_complete_type(amqp_context_t *context, amqp_type_t *type)
{
    size_t saved_buffer_write_position = context->encode.buffer->limit.size;

        amqp_type_t *top = pop_container(context, type);
        if (top != type)
        {
            abort();
        }

        type->position.size = saved_buffer_write_position - type->position.index;

        switch (type->meta_data->width)
        {
        case 0:
            if (type->value.described.count > 2)
            {
                amqp_mark_type_invalid(type, AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT);
                amqp_mark_type_invalid(context->encode.container, AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT);
            }
            break;

        case 1:
            // TODO test for size/count overflow
            context->encode.buffer->limit.size = type->position.index - 1;
            amqp_unchecked_putc(context->encode.buffer, (unsigned char) type->position.size);
            amqp_unchecked_putc(context->encode.buffer, (unsigned char) type->value.compound.count);
            break;

        case 4:
            context->encode.buffer->limit.size = type->position.index - 4;
            amqp_hton_uint(context, (uint32_t) type->position.size);
            amqp_hton_uint(context, (uint32_t) type->value.compound.count);
            break;

        default:
            abort();
        }

        context->encode.buffer->limit.size = saved_buffer_write_position;

    return type;
}
