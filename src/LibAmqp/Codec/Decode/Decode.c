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

#include "Codec/Type/Type.h"
#include "Buffer/Buffer.h"
#include "Codec/Decode/Decode.h"
#include "Context/Context.h"
#include "Codec/Type/TypeValidate.h"

static amqp_type_t *amqp_decode_array_element(amqp_context_t *context, amqp_type_t *array_element_type);


int amqp_decode_null(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    type->position.index = amqp_buffer_index(type->context->decode.buffer);
    type->position.size = 0;
    type->flags.is_null = 1;
    return true;
}

static int amqp_decode_fixed_zero_width(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    type->position.index = amqp_buffer_index(type->context->decode.buffer);
    type->position.size = 0;
    return true;
}

int amqp_decode_boolean_true(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_fixed_zero_width(meta_data, type);
}

int amqp_decode_boolean_false(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_fixed_zero_width(meta_data, type);
}

#define decode_error(type, code, ...) _decode_error(type, 1, __FILE__, __LINE__, #code, code, ""  __VA_ARGS__)
static void _decode_error(amqp_type_t *type, int level, const char *filename, int line_number, const char *error_mnemonic, int error_code, const char *format, ...)
{
    char description[128];
    char message[256];
    va_list args;

    amqp_mark_type_invalid(type, error_code);

    if (level < type->context->debug.level)
    {
        amqp_describe_type(description, sizeof(description), type);

        va_start(args, format);
        vsnprintf(message, sizeof(message), format, args);
        va_end(args);
    
        _amqp_error(type->context, level, filename, line_number, error_mnemonic, error_code, "Decode failure; %s; while decoding %s", message, description);
    }

//    if (type->context->debug.level) abort();

}

static inline
int check_available(amqp_type_t *type, size_t width)
{
    int rc = amqp_buffer_check_available(type->context->decode.buffer, width);
    if (!rc)
    {
        decode_error(type, AMQP_ERROR_BUFFER_WOULD_OVERRUN, "Type extends past end of buffer");
    }
    return rc;
}

static inline
int amqp_decode_fixed_width(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    int rc = check_available(type, meta_data->width);
    if (rc)
    {
        type->position.index = amqp_buffer_index(type->context->decode.buffer);
        type->position.size = meta_data->width;
        amqp_buffer_advance_read_index(type->context->decode.buffer, meta_data->width);
        switch (type->position.size)
        {
        case 1:
            type->value.b1 = amqp_ntoh_8(type->context->decode.buffer, type->position.index);
            break;
        case 2:
            type->value.b2 = amqp_ntoh_16(type->context->decode.buffer, type->position.index);
            break;
        case 4:
            type->value.b4 = amqp_ntoh_32(type->context->decode.buffer, type->position.index);
            break;
        case 8:
            amqp_ntoh_64(&type->value.b8, type->context->decode.buffer, type->position.index);
            break;
        case 16:
            type->value.uuid = amqp_buffer_pointer(type->context->decode.buffer, type->position.index);
            break;
            
        default:
            abort();
        }
    }
    return rc;
}

int amqp_decode_boolean(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_ubyte(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_byte(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_ushort(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_short(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_uint(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_uint0(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     int rc =  amqp_decode_fixed_zero_width(meta_data, type);
     if (rc)
     {
        type->value.b4._uint = 0U;
     }
     return rc;
 }

int amqp_decode_int(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_small_uint(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     int rc = amqp_decode_fixed_width(meta_data, type);
     if (rc)
     {
        type->value.b4._uint = type->value.b1._unsigned;
     }
     return rc;
}

int amqp_decode_small_int(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     int rc = amqp_decode_fixed_width(meta_data, type);
     if (rc)
     {
        type->value.b4._int = type->value.b1._signed;
     }
     return rc;
}

int amqp_decode_float(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_char(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    int rc = amqp_decode_fixed_width(meta_data, type);
    if (rc)
    {
        if (!amqp_validate_char(type->value.b4._wchar))
        {
            decode_error(type, AMQP_ERROR_INVALID_UTF32_CHARACTER, "0x%08x is an invalid utf_32 character", type->value.b4._wchar);
            return 0;
        }
    }
    return rc;
}

int amqp_decode_decimal32(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_ulong0(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     int rc =  amqp_decode_fixed_zero_width(meta_data, type);
     if (rc)
     {
        type->value.b8._ulong = 0U;
     }
     return rc;
 }

int amqp_decode_ulong(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_long(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_small_ulong(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     int rc = amqp_decode_fixed_width(meta_data, type);
     if (rc)
     {
        type->value.b8._ulong = type->value.b1._unsigned;
     }
     return rc;
}

int amqp_decode_small_long(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     int rc = amqp_decode_fixed_width(meta_data, type);
     if (rc)
     {
        type->value.b8._long = type->value.b1._signed;
     }
     return rc;
}

int amqp_decode_double(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_timestamp(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_decimal64(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

int amqp_decode_uuid(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
     return amqp_decode_fixed_width(meta_data, type);
}

// Variable types
static
uint32_t get_variable_type_size(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    uint32_t size = amqp_buffer_read_size(type->context->decode.buffer, meta_data->width);
    if (size == -1)
    {
        decode_error(type, AMQP_ERROR_BUFFER_WOULD_OVERRUN, "Cannot read size field for a variable/compound sized type");
        return -1;
    }
    if (!amqp_buffer_check_available(type->context->decode.buffer, size))
    {
        decode_error(type, AMQP_ERROR_BUFFER_WOULD_OVERRUN, "A variable/compound type size of %ul bytes would overrun buffer", (unsigned long) size);
        return -1;
    }
    return size;
}

// Variable types
static
uint32_t get_variable_type_count(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    uint32_t count = amqp_buffer_read_size(type->context->decode.buffer, meta_data->width);
    if (count == -1)
    {
        decode_error(type, AMQP_ERROR_VARIABLE_TYPE_MALFORMED, "Cannot read count field for a compound type");
    }
    return count;
}

static
int amqp_construct_variable_type(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    uint32_t size = get_variable_type_size(meta_data, type);
    if (size == -1)
    {
        return false;
    }

    type->position.index = amqp_buffer_index(type->context->decode.buffer);
    type->position.size = size;

    // advance the read index past the variable types data.
    // TODO - stop doing this, let type decode do the correct thing.
    amqp_buffer_advance_read_index(type->context->decode.buffer, size);
    return true;
}

int amqp_decode_binary_vbin8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_construct_variable_type(meta_data, type);
}

int amqp_decode_binary_vbin32(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_construct_variable_type(meta_data, type);
}

static
int amqp_decode_symbol(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    int rc = amqp_construct_variable_type(meta_data, type);
    if (rc)
    {
        size_t i;
        for (i = 0; i < type->position.size; i++)
        {
            int c = amqp_unchecked_getc_at(type->context->decode.buffer, type->position.index + i);
            if (c & 0x80)
            {
                decode_error(type, AMQP_ERROR_INVALID_SYMBOL_CHARACTER, "Symbol contains a non ASCII character 0x%02x", c);
                return 0;
            }
        }
    }
    return rc;
}

int amqp_decode_symbol_sym8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_symbol(meta_data, type);
}

int amqp_decode_symbol_sym32(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_symbol(meta_data, type);
}

static
int amqp_decode_string_utf8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    int rc = amqp_construct_variable_type(meta_data, type);
    if (rc)
    {
        size_t i;
        for (i = 0; i < type->position.size; i++)
        {
            int c = amqp_unchecked_getc_at(type->context->decode.buffer, type->position.index + i);

            // TODO - verify that characters are valid utf8
            if (c == 0)
            {
                decode_error(type, AMQP_ERROR_INVALID_UTF8_CHARACTER, "utf-8 string contains an invalid character");
                return 0;
            }
        }
    }
    return rc;
}

int amqp_decode_string_str8_utf8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_string_utf8(meta_data, type);
}

int amqp_decode_string_str32_utf8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_string_utf8(meta_data, type);
}

static
int amqp_construct_container_type(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    uint32_t size = get_variable_type_size(meta_data, type);
    if (size != -1)
    {
        type->position.index = amqp_buffer_index(type->context->decode.buffer);
        type->position.size = size;
        return true;
    }
    return false;
}

int amqp_decode_described_type(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{

    type->position.index = amqp_buffer_index(type->context->decode.buffer);
    type->position.size = 0;
    type->flags.structure.flags.is_described = true;

    type->value.described.count = 2;
    type->value.described.elements = amqp_allocate_amqp_type_t_array(2);

    amqp_type_t *descriptor = amqp_decode(type->context);
    if (descriptor)
    {
        type->value.described.elements[0] = descriptor;
        descriptor->flags.is_contained = true;
        descriptor->flags.is_descriptor = true;
    }
    else
    {
        decode_error(type, AMQP_ERROR_NO_DESCRIPTOR, "expected a descriptor for described type");
        return false;
    }

    amqp_type_t *described = amqp_decode(type->context);
    if (described)
    {
        type->value.described.elements[1] = described;
        described->flags.is_contained = true;
        described->flags.has_descriptor = true;
    }
    else
    {
        decode_error(type, AMQP_ERROR_NO_DESCRIBED_TYPE, "expected a described type");
        return false;
    }

    if (!amqp_type_is_valid(descriptor))
    {
        decode_error(type, AMQP_ERROR_DESCRIPTOR_INVALID, "descriptor for a described type is invalid");
        return false;
    }

    if (!amqp_type_is_valid(described))
    {
        decode_error(type, AMQP_ERROR_DESCRIBED_INVALID, "type for a described type is invalid");
        return false;
    }

    return true;
}

int amqp_decode_list_list(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    uint32_t i;
    int rc = amqp_construct_container_type(meta_data, type);
    if (rc)
    {
        uint32_t count = get_variable_type_count(meta_data, type);
        if (count == -1)
        {
            return 0;
        }

        type->flags.structure.flags.is_list = true;
        type->value.list.count = count;

        type->value.list.elements = amqp_allocate_amqp_type_t_array(count);
        for (i = 0; i < count; i++)
        {
            amqp_type_t *element = amqp_decode(type->context);
            if (element)
            {
                type->value.list.elements[i] = element;
                element->flags.is_contained = true;
            }
        }
    }
    return rc;
}

int amqp_decode_list_8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
   return amqp_decode_list_list(meta_data, type);
}

int amqp_decode_list_32(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
   return amqp_decode_list_list(meta_data, type);
}

inline static
int is_even(size_t v)
{
    return (v & 0x01) == 0;
}

static int amqp_decode_map_map(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    size_t i;
    int rc = amqp_construct_container_type(meta_data, type);
    if (rc)
    {
        uint32_t count = get_variable_type_count(meta_data, type);
        if (count == -1)
        {
            return 0;
        }

        type->flags.structure.flags.is_map = true;
        type->value.map.count = count;

        type->value.map.entries = amqp_allocate_amqp_type_t_array(count);
        for (i = 0; i < type->value.map.count; i++)
        {
            amqp_type_t *entry = amqp_decode(type->context);
            entry->flags.is_contained = true;
            type->value.map.entries[i] = entry;
        }
    }
    return rc;
}

int amqp_decode_map_8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_map_map(meta_data, type);
}

int amqp_decode_map_32(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_map_map(meta_data, type);
}

static int amqp_decode_array(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    int i;
    int rc = amqp_construct_container_type(meta_data, type);
    if (rc)
    {
        uint32_t count = get_variable_type_count(meta_data, type);
        if (count == -1)
        {
            return 0;
        }

        type->flags.structure.flags.is_array = true;
        type->value.array.count = count;
        type->value.array.elements = amqp_allocate_amqp_type_t_array(count);

        amqp_type_t *element_type = amqp_decode(type->context);
        element_type->flags.is_contained = true;

        type->value.array.elements[0] = element_type;

        for (i = 1; i < count; i++)
        {
            amqp_type_t *element = amqp_decode_array_element(type->context, element_type);
            element->flags.is_contained = true;
            type->value.array.elements[i] = element;
        }

        // TODO - check that the elements do not go past the array boundry
    }
    return rc;
}

int amqp_decode_array_8(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_array(meta_data, type);
}

int amqp_decode_array_32(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    return amqp_decode_array(meta_data, type);
}

int amqp_decode_extension_type(amqp_type_meta_data_t *meta_data, amqp_type_t *type)
{
    not_implemented(Unsupported format code);
}

static int
decode_type_constructor_into_result(amqp_context_t *context, amqp_type_t *type)
{
    amqp_type_meta_data_t *meta_data;

    type->format_code = amqp_buffer_getc(context->decode.buffer);

    if ((meta_data = amqp_type_meta_data_lookup(context, type->format_code)) == NULL)
    {
        decode_error(type, AMQP_ERROR_UNKNOWN_FORMAT_CODE, "unknown format code = 0x%02x", type->format_code);
        return 0;
    }

    type->meta_data = meta_data;

    return 1;
}

static int
decode_type_into_result(amqp_context_t *context, amqp_type_t *type)
{
    int rc;

    if (!(rc = (*type->meta_data->type_decoder)(type->meta_data, type)))
    {
        assert(type->flags.is_invalid);
        assert(type->invalid_cause != 0);
    }

    return rc;
}

static
amqp_type_t *amqp_decode_array_element(amqp_context_t *context, amqp_type_t *array_element_type)
{
    amqp_type_t *type;

    type = amqp_allocate_type(context);
    
    type->context = context;
    type->format_code = array_element_type->format_code;
    type->extension_type_code = array_element_type->extension_type_code;
    type->meta_data = array_element_type->meta_data;

    decode_type_into_result(context, type);

    return type;
}

amqp_type_t *amqp_decode(amqp_context_t *context)
{
    amqp_type_t *type;

    if (amqp_buffer_at_end(context->decode.buffer))
    {
        return NULL;
    }

    type = amqp_allocate_type(context);
    type->context = context;

    // amqp_type_convert_buffer(type);

    if (decode_type_constructor_into_result(context, type))
    {
        decode_type_into_result(context, type);
    }

    return type;
}

amqp_type_t *amqp_decode_supress_messages(amqp_context_t *context)
{
    amqp_type_t *result;
    int debug_level = context->debug.level;

    context->debug.level = 0;
    result = amqp_decode(context);
    context->debug.level = debug_level;
    
    return result;
}

amqp_type_t *amqp_list_element(amqp_type_t *type, int index)
{
    assert(amqp_type_is_list(type));
    assert(type->value.list.count > index);
    return type->value.list.elements[index];
}

amqp_type_t *amqp_map_element(amqp_type_t *type, int index)
{
    assert(amqp_type_is_map(type));
    assert(type->value.map.count > index);
    return type->value.map.entries[index];
}
