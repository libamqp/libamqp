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

#define NO_EXTRA_TYPEDEF_FLAGS  0

/*
    constructor is not emitted for array elements after the first
 */
static inline int emit_constructor(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_type_t *type = context->encode.container;
    return type == 0 || (amqp_type_is_container(type) && !amqp_type_is_array(type)) || (amqp_type_is_array(type)  && type->value.array.count == 0);
}

static inline amqp_type_t **allocate_elements(amqp_context_t *c, amqp_type_t **elements, size_t count)
{
    return elements == 0 ? amqp_allocate_amqp_type_t_array(c, 1) : amqp_realloc_amqp_type_t_array(c, elements, count);
}

static inline void amqp_add_element_to_container(amqp_context_t *context, amqp_buffer_t *buffer, amqp_type_t *type)
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
                // mark both the element and container as invalid
                amqp_mark_type_invalid(type, AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT);
                amqp_mark_type_invalid(context->encode.container, AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT);
            }
        }

        if (amqp_type_is_composite(context->encode.container))
        {
            switch (count)
            {
            case 0:
                type->typedef_flags |= amqp_is_descriptor;
                type->flags.is_descriptor = true;
                break;

            case 1:
                type->typedef_flags |= amqp_is_described;
                type->flags.has_descriptor = true;
                break;
            }
        }
        
        context->encode.container->value.compound.elements = allocate_elements(context, context->encode.container->value.compound.elements, count + 1);
        context->encode.container->value.compound.elements[count] = type;
    }
}

static inline bool is_i_contained_within_array(amqp_context_t *context)
{
    return context->encode.container && amqp_type_is_array(context->encode.container);
}

static void push_container(amqp_context_t *context, amqp_buffer_t *buffer, amqp_type_t *type)
{
    type->flags.is_incomplete = true;
    type->value.compound.saved_container = context->encode.container;
    context->encode.container = type;
}

static amqp_type_t *pop_container(amqp_context_t *context)
{
    amqp_type_t *result = context->encode.container;

    result->flags.is_incomplete = false;
    context->encode.container = result->value.compound.saved_container;
    result->value.compound.saved_container = 0;
    return result;
}

static amqp_type_t *initialize_type(amqp_context_t *context, amqp_buffer_t *buffer, const amqp_typedef_flags_t extra_typedef_flags, amqp_encoding_meta_data_t *meta_data)
{
    amqp_type_t *type = (amqp_type_t *) amqp_allocate(context, &context->memory.amqp_type_t_pool);

    type->format_code = meta_data->format_code;
    type->meta_data = meta_data;
    type->typedef_flags = meta_data->typedef_flags | amqp_is_encoded | extra_typedef_flags;

    if (emit_constructor(context, buffer))
    {
        amqp_buffer_putc(buffer, meta_data->format_code);
    }

    amqp_add_element_to_container(context, buffer, type);

    if (amqp_type_is_container(type))
    {
        push_container(context, buffer, type);
    }

    return type;
}

static amqp_type_t *amqp_encode_fixed(amqp_context_t *context, amqp_buffer_t *buffer, amqp_encoding_meta_data_t *meta_data)
{
    amqp_type_t  *type;

    if (amqp_buffer_check_free_space(buffer, meta_data->width + 1) == 0)
    {
        return NULL;
    }

    type = initialize_type(context, buffer, NO_EXTRA_TYPEDEF_FLAGS, meta_data);

    type->position.index = amqp_buffer_size(buffer);
    type->position.size = meta_data->width;

    return type;
}

static void amqp_hton_uint(amqp_context_t *context, amqp_buffer_t *buffer, uint32_t value)
{
    amqp_four_byte_t v;
    v._uint = value;
    amqp_hton_32(buffer, v);
}

static amqp_type_t *amqp_encode_simple_variable(amqp_context_t *context, amqp_buffer_t *buffer, amqp_encoding_meta_data_t *meta_data, const unsigned char *value, size_t size)
{
    amqp_type_t  *type;

    if (amqp_buffer_check_free_space(buffer, meta_data->width + 1 + size) == 0)
    {
        return NULL;
    }

    type = initialize_type(context, buffer, NO_EXTRA_TYPEDEF_FLAGS, meta_data);
    type->value.variable.buffer = buffer;
    switch (meta_data->width)
    {
    case 1:
        amqp_unchecked_putc(buffer, (unsigned char) (size & 0xff));
        break;

    case 4:
        amqp_hton_uint(context, buffer, (uint32_t) size);
        break;

    default:
        abort();
    }

    type->position.index = amqp_buffer_size(buffer);
    type->position.size = size;
    amqp_buffer_puts(buffer, value, size);

    return type;
}

static amqp_type_t *encode_compound_type(amqp_context_t *context, amqp_buffer_t *buffer, amqp_typedef_flags_t extra_typedef_flags, amqp_encoding_meta_data_t *meta_data)
{
    amqp_type_t  *type;

    // check minimum space requirement, not a lot of use really
    if (amqp_buffer_check_free_space(buffer, meta_data->width * 2 + 2) == 0)
    {
        abort();
        return NULL;
    }

    type = initialize_type(context, buffer, extra_typedef_flags, meta_data);

    switch (meta_data->width)
    {
    case 0:
        type->position.index = amqp_buffer_size(buffer);
        break;

    case 1:
        amqp_unchecked_putc(buffer, (unsigned char) 1);
        type->position.index = amqp_buffer_size(buffer);
        amqp_unchecked_putc(buffer, (unsigned char) 0);
        break;

    case 4:
        amqp_hton_uint(context, buffer, (uint32_t) 4);
        type->position.index = amqp_buffer_size(buffer);
        amqp_hton_uint(context, buffer, (uint32_t) 0);
        break;

    default:
        abort();
    }

    return type;
}

amqp_type_t *amqp_encode_null(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, &amqp_type_meta_data_null);

    if (type)
    {
        type->flags.is_null = 1;
    }

    return type;
}

static amqp_type_t *amqp_encode_fixed_one_byte(amqp_context_t *context, amqp_buffer_t *buffer, amqp_encoding_meta_data_t *meta_data, unsigned char b)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, meta_data);

    if (type)
    {
        amqp_unchecked_putc(buffer, b);
    }

    return type;
}

amqp_type_t *amqp_encode_boolean(amqp_context_t *context, amqp_buffer_t *buffer, int value)
{
    amqp_type_t *result;
    if (is_i_contained_within_array(context))
    {
        unsigned char boolean_value = value != 0 ? 1 : 0;     // True == 1, false == 0
        result = amqp_encode_fixed_one_byte(context, buffer, &amqp_type_meta_data_boolean, boolean_value);
    }
    else
    {
        amqp_encoding_meta_data_t *meta_data = value ? &amqp_type_meta_data_boolean_true : &amqp_type_meta_data_boolean_false;
        result = amqp_encode_fixed(context, buffer, meta_data);
    }
    result->value.b1._unsigned = value;
    return result;
}

amqp_type_t *amqp_encode_ubyte(amqp_context_t *context, amqp_buffer_t *buffer, uint8_t value)
{
    return amqp_encode_fixed_one_byte(context, buffer, &amqp_type_meta_data_ubyte, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_byte(amqp_context_t *context, amqp_buffer_t *buffer, int8_t value)
{
    return amqp_encode_fixed_one_byte(context, buffer, &amqp_type_meta_data_byte, (unsigned char) (value & 0xff));
}

static amqp_type_t *amqp_encode_fixed_two_byte(amqp_context_t *context, amqp_buffer_t *buffer, amqp_encoding_meta_data_t *meta_data, amqp_two_byte_t value)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, meta_data);

    if (type)
    {
        amqp_hton_16(buffer, value);
    }

    return type;
}

amqp_type_t *amqp_encode_ushort(amqp_context_t *context, amqp_buffer_t *buffer, uint16_t value)
{
    amqp_two_byte_t v;
    v._ushort = value;
    return amqp_encode_fixed_two_byte(context, buffer, &amqp_type_meta_data_ushort, v);
}

amqp_type_t *amqp_encode_short(amqp_context_t *context, amqp_buffer_t *buffer, int16_t value)
{
    amqp_two_byte_t v;
    v._short = value;
    return amqp_encode_fixed_two_byte(context, buffer, &amqp_type_meta_data_short, v);
}

static amqp_type_t *amqp_encode_fixed_four_byte(amqp_context_t *context, amqp_buffer_t *buffer, amqp_encoding_meta_data_t *meta_data, amqp_four_byte_t value)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, meta_data);

    if (type)
    {
        amqp_hton_32(buffer, value);
    }

    return type;
}

amqp_type_t *amqp_encode_small_int(amqp_context_t *context, amqp_buffer_t *buffer, int32_t value)
{
    return amqp_encode_fixed_one_byte(context, buffer, &amqp_type_meta_data_int_small_int, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_int(amqp_context_t *context, amqp_buffer_t *buffer, int32_t value)
{
    if (value == (signed char) (value & 0xff))
    {
        return amqp_encode_small_int(context, buffer, value);
    }
    else
    {
        amqp_four_byte_t v;
        v._int = value;
        return amqp_encode_fixed_four_byte(context, buffer, &amqp_type_meta_data_int, v);
    }
}

amqp_type_t *amqp_encode_small_uint(amqp_context_t *context, amqp_buffer_t *buffer, uint32_t value)
{
    return amqp_encode_fixed_one_byte(context, buffer, &amqp_type_meta_data_uint_small_uint, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_uint0(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, &amqp_type_meta_data_uint_uint0);
    return type;
}

amqp_type_t *amqp_encode_uint(amqp_context_t *context, amqp_buffer_t *buffer, uint32_t value)
{
    if (value == 0)
    {
        return amqp_encode_uint0(context, buffer);
    }
    else if (value < 255)
    {
        return amqp_encode_small_uint(context, buffer, value);
    }
    else
    {
        amqp_four_byte_t v;
        v._uint = value;
        return amqp_encode_fixed_four_byte(context, buffer, &amqp_type_meta_data_uint, v);
    }
}

static amqp_type_t *amqp_encode_fixed_eight_byte(amqp_context_t *context, amqp_buffer_t *buffer, amqp_encoding_meta_data_t *meta_data, amqp_eight_byte_t value)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, meta_data);

    if (type)
    {
        amqp_hton_64(buffer, value);
    }

    return type;
}

amqp_type_t *amqp_encode_ulong0(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, &amqp_type_meta_data_ulong_ulong0);
    return type;
}

amqp_type_t *amqp_encode_small_ulong(amqp_context_t *context, amqp_buffer_t *buffer, uint64_t value)
{
    return amqp_encode_fixed_one_byte(context, buffer, &amqp_type_meta_data_ulong_small_ulong, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_ulong(amqp_context_t *context, amqp_buffer_t *buffer, uint64_t value)
{
    if (value == 0)
    {
        return amqp_encode_ulong0(context, buffer);
    }
    else if (value < 255)
    {
        return amqp_encode_small_ulong(context, buffer, value);
    }
    else
    {
        amqp_eight_byte_t v;
        v._ulong = value;
        return amqp_encode_fixed_eight_byte(context, buffer, &amqp_type_meta_data_ulong, v);
    }
}

amqp_type_t *amqp_encode_small_long(amqp_context_t *context, amqp_buffer_t *buffer, int64_t value)
{
    return amqp_encode_fixed_one_byte(context, buffer, &amqp_type_meta_data_long_small_long, (unsigned char) (value & 0xff));
}

amqp_type_t *amqp_encode_long(amqp_context_t *context, amqp_buffer_t *buffer, int64_t value)
{
    if (value == (signed char) (value & 0xff))
    {
        return amqp_encode_small_long(context, buffer, value);
    }
    else
    {
        amqp_eight_byte_t v;
        v._long = value;
        return amqp_encode_fixed_eight_byte(context, buffer, &amqp_type_meta_data_long, v);
    }
}

amqp_type_t *amqp_encode_timestamp(amqp_context_t *context, amqp_buffer_t *buffer, amqp_timestamp_t value)
{
    amqp_eight_byte_t v;
    v._timestamp = value;
    return amqp_encode_fixed_eight_byte(context, buffer, &amqp_type_meta_data_timestamp_ms64, v);
}

amqp_type_t *amqp_encode_float(amqp_context_t *context, amqp_buffer_t *buffer, float value)
{
    amqp_four_byte_t v;
    v._float = value;
    return amqp_encode_fixed_four_byte(context, buffer, &amqp_type_meta_data_float_ieee_754, v);
}

amqp_type_t *amqp_encode_double(amqp_context_t *context, amqp_buffer_t *buffer, double value)
{
    amqp_eight_byte_t v;
    v._double = value;
    return amqp_encode_fixed_eight_byte(context, buffer, &amqp_type_meta_data_double_ieee_754, v);
}

amqp_type_t *amqp_encode_char(amqp_context_t *context, amqp_buffer_t *buffer, wchar_t value)
{
   return 0; // TODO - implement me
}

amqp_type_t *amqp_encode_decimal32(amqp_context_t *context, amqp_buffer_t *buffer, amqp_decimal32_t value)
{
   return 0; // TODO - implement me
}

amqp_type_t *amqp_encode_decimal64(amqp_context_t *context, amqp_buffer_t *buffer, amqp_decimal64_t value)
{
   return 0; // TODO - implement me
}

amqp_type_t *amqp_encode_uuid(amqp_context_t *context, amqp_buffer_t *buffer, amqp_uuid_t *value)
{
    amqp_type_t *type = amqp_encode_fixed(context, buffer, &amqp_type_meta_data_uuid);
    if (type)
    {
        amqp_buffer_puts(buffer, (unsigned char *) value, sizeof(amqp_uuid_t));
    }
    return type;
}

amqp_type_t *amqp_encode_binary(amqp_context_t *context, amqp_buffer_t *buffer, const unsigned char *value, size_t size)
{
   return size < 256 ? amqp_encode_binary_vbin8(context, buffer, value, size) : amqp_encode_binary_vbin32(context, buffer, value, size);
}

amqp_type_t *amqp_encode_binary_vbin8(amqp_context_t *context, amqp_buffer_t *buffer, const unsigned char *value, size_t size)
{
    amqp_type_t *result = amqp_encode_simple_variable(context, buffer, &amqp_type_meta_data_binary_vbin8, value, size);
    result->flags.is_binary = true;
    return result;
}

amqp_type_t *amqp_encode_binary_vbin32(amqp_context_t *context, amqp_buffer_t *buffer, const unsigned char *value, size_t size)
{
    amqp_type_t *result = amqp_encode_simple_variable(context, buffer, &amqp_type_meta_data_binary_vbin32, value, size);
    result->flags.is_binary = true;
    return result;
}

amqp_type_t *amqp_encode_symbol_sym8n(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size)
{
    amqp_type_t *result = amqp_encode_simple_variable(context, buffer, &amqp_type_meta_data_symbol_sym8, (const unsigned char *) value, size);
    result->flags.is_symbol = true;
    return result;
}

amqp_type_t *amqp_encode_symbol_sym32n(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size)
{
    amqp_type_t *result = amqp_encode_simple_variable(context, buffer, &amqp_type_meta_data_symbol_sym32, (const unsigned char *) value, size);
    result->flags.is_symbol = true;
    return result;
}

amqp_type_t *amqp_encode_symboln(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size)
{
   return size < 256 ? amqp_encode_symbol_sym8n(context, buffer, value, size) : amqp_encode_symbol_sym32n(context, buffer, value, size);
}

amqp_type_t *amqp_encode_symbol_sym8(amqp_context_t *context, amqp_buffer_t *buffer, const char *value)
{
   return amqp_encode_symbol_sym8n(context, buffer, value, strlen(value));
}

amqp_type_t *amqp_encode_symbol_sym32(amqp_context_t *context, amqp_buffer_t *buffer, const char *value)
{
   return amqp_encode_symbol_sym32n(context, buffer, value, strlen(value));
}

amqp_type_t *amqp_encode_symbol(amqp_context_t *context, amqp_buffer_t *buffer, const char *value)
{
   return amqp_encode_symboln(context, buffer, value, strlen(value));
}

static inline
amqp_type_t *encode_string_utf8(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size)
{
    // TODO - validate
    amqp_type_t *result = amqp_encode_simple_variable(context, buffer, size < 256 ? &amqp_type_meta_data_string_str8_utf8 : &amqp_type_meta_data_string_str32_utf8, (const unsigned char *) value, size);
    result->flags.is_string = true;
    return result;
}

amqp_type_t *amqp_encode_string_utf8(amqp_context_t *context, amqp_buffer_t *buffer, const char *value)
{
   return encode_string_utf8(context, buffer, value, strlen(value));
}

amqp_type_t *amqp_encode_string_utf8n(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size)
{
   return encode_string_utf8(context, buffer, value, size);
}

amqp_type_t *amqp_encode_list_8(amqp_context_t *context, amqp_buffer_t *buffer)
{
    return encode_compound_type(context, buffer, amqp_is_list, &amqp_type_meta_data_list_8);
}

amqp_type_t *amqp_encode_list_32(amqp_context_t *context, amqp_buffer_t *buffer)
{
    return encode_compound_type(context, buffer, amqp_is_list, &amqp_type_meta_data_list_32);
}

amqp_type_t *amqp_encode_array_8(amqp_context_t *context, amqp_buffer_t *buffer)
{
    return encode_compound_type(context, buffer, amqp_is_array, &amqp_type_meta_data_array_8);
}

amqp_type_t *amqp_encode_array_32(amqp_context_t *context, amqp_buffer_t *buffer)
{
    return encode_compound_type(context, buffer, amqp_is_array, &amqp_type_meta_data_array_32);
}

amqp_type_t *amqp_encode_map_8(amqp_context_t *context, amqp_buffer_t *buffer)
{
    return encode_compound_type(context, buffer, amqp_is_map, &amqp_type_meta_data_map_8);
}

amqp_type_t *amqp_encode_map_32(amqp_context_t *context, amqp_buffer_t *buffer)
{
    return encode_compound_type(context, buffer, amqp_is_map, &amqp_type_meta_data_map_32);
}

amqp_type_t *amqp_start_encode_described_type(amqp_context_t *context, amqp_buffer_t *buffer)
{
    return encode_compound_type(context, buffer, amqp_is_composite, &amqp_type_meta_data_described);
}

static
amqp_type_t *complete_container_type(amqp_context_t *context, amqp_buffer_t *buffer, amqp_type_t *type)
{
    size_t saved_buffer_write_position = amqp_buffer_size(buffer);

    type->position.size = saved_buffer_write_position - type->position.index;
    switch (type->meta_data->width)
    {
    case 0:
        if (type->value.described.count > 2)
        {
            amqp_mark_type_invalid(type, AMQP_ERROR_DESCRIBED_TYPE_OVERFLOW);
            amqp_mark_type_invalid(context->encode.container, AMQP_ERROR_DESCRIBED_TYPE_OVERFLOW);
        }
        break;

    case 1:
        // TODO test for size/count overflow
        amqp_buffer_set_write_index(buffer, type->position.index - 1);
        amqp_unchecked_putc(buffer, (unsigned char) type->position.size);
        amqp_unchecked_putc(buffer, (unsigned char) type->value.compound.count);
        break;

    case 4:
        amqp_buffer_set_write_index(buffer, type->position.index - 4);
        amqp_hton_uint(context, buffer, (uint32_t) type->position.size);
        amqp_hton_uint(context, buffer, (uint32_t) type->value.compound.count);
        break;

    default:
        amqp_fatal_program_error("Invalid size/count field width.");
    }

    amqp_buffer_set_write_index(buffer, saved_buffer_write_position);

    return type;
}

static
amqp_type_t *complete_empty_list(amqp_context_t *context, amqp_buffer_t *buffer, amqp_type_t *type)
{
    amqp_encoding_meta_data_t *old_meta_data = type->meta_data;
    amqp_encoding_meta_data_t *meta_data = &amqp_type_meta_data_list_0;

    type->meta_data = &amqp_type_meta_data_list_0;
    type->format_code = type->meta_data->format_code;

    // backup over the old list type format code and size field
    amqp_buffer_backup(buffer, old_meta_data->width * 2 + 1);

    // write the zero list0 format code
    amqp_buffer_putc(buffer, meta_data->format_code);

    type->position.index = amqp_buffer_size(buffer);
    type->position.size = meta_data->width;

    return type;
}

amqp_type_t *amqp_complete_type(amqp_context_t *context, amqp_buffer_t *buffer, amqp_type_t *type)
{
    amqp_type_t *top = pop_container(context);
    // TODO - check for completion of a nonexistent container

    if (type && top != type)
    {
        amqp_api_usage_error(context, "Can only complete inner most map/list/array.");
        amqp_mark_type_invalid(type, AMQP_ERROR_COMPLETE_WRONG_CONTAINER);
        amqp_mark_type_invalid(top, AMQP_ERROR_COMPLETE_WRONG_CONTAINER);
        return 0;
    }

    return (is_i_contained_within_array(context) || !amqp_type_is_empty_list(top))
            ? complete_container_type(context, buffer, top)
            : complete_empty_list(context, buffer, top);
}

amqp_type_t *amqp_encode_list_0(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_type_t *type = amqp_encode_list_8(context, buffer);
    if (type)
    {
        amqp_complete_type(context, buffer, type);
    }
    return type;
}
