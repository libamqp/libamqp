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

#include <string.h>
#include "Codec/Type/TypeEncodeMetaData.h"
#include "Codec/Encode/Encode.h"

/**

    &amqp_type_meta_data_described,
    &amqp_type_meta_data_null,
    &amqp_type_meta_data_boolean_true,
    &amqp_type_meta_data_boolean_false,
    &amqp_type_meta_data_uint_uint0,
    &amqp_type_meta_data_extension_fixed_0,
    &amqp_type_meta_data_ubyte,
    &amqp_type_meta_data_byte,
    &amqp_type_meta_data_uint_small_uint,
    &amqp_type_meta_data_ulong_small_ulong,
    &amqp_type_meta_data_int_small_int,
    &amqp_type_meta_data_long_small_long,
    &amqp_type_meta_data_extension_fixed_1,
    &amqp_type_meta_data_ushort,
    &amqp_type_meta_data_short,
    &amqp_type_meta_data_extension_fixed_2,
    &amqp_type_meta_data_uint,
    &amqp_type_meta_data_int,
    &amqp_type_meta_data_float_ieee_754,
    &amqp_type_meta_data_char_utf32,
    &amqp_type_meta_data_decimal32_ieee_754,
    &amqp_type_meta_data_extension_fixed_4,
    &amqp_type_meta_data_ulong,
    &amqp_type_meta_data_long,
    &amqp_type_meta_data_double_ieee_754,
    &amqp_type_meta_data_timestamp_ms64,
    &amqp_type_meta_data_decimal64_ieee_754,
    &amqp_type_meta_data_extension_fixed_8,
    &amqp_type_meta_data_uuid,
    &amqp_type_meta_data_extension_fixed_16,
    &amqp_type_meta_data_binary_vbin8,

    &amqp_type_meta_data_extension_variable_1,
    &amqp_type_meta_data_binary_vbin32,
    &amqp_type_meta_data_extension_variable_4,
    &amqp_type_meta_data_list_8,
    &amqp_type_meta_data_map_8,
    &amqp_type_meta_data_extension_compound_1,
    &amqp_type_meta_data_list_32,
    &amqp_type_meta_data_map_32,
    &amqp_type_meta_data_extension_compound_4,
    &amqp_type_meta_data_array_8,
    &amqp_type_meta_data_extension_array_1,
    &amqp_type_meta_data_array_32,
    &amqp_type_meta_data_extension_array_4

    **/
int amqp_type_describe_null(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_boolean(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_ubyte(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_byte(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_ushort(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_short(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_uint(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_int(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_small_uint(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_small_int(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_float(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_char(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_decimal32(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_ulong(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_long(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}
int amqp_type_describe_small_ulong(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_small_long(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_double(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_timestamp(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_decimal64(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_uuid(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_binary(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

int amqp_type_describe_symbol(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    const char *p = va_arg(ap, const char *);
    description->size = strlen(p);
    description->meta_data = strlen(p) > 255 ? &amqp_type_meta_data_symbol_sym32 : &amqp_type_meta_data_symbol_sym8;
    description->overhead = description->meta_data->width + 1;
    return true;
}

int amqp_type_describe_string_utf8(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    const char *p = va_arg(ap, const char *);
    description->size = strlen(p);
    description->meta_data = description->size > 255 ? &amqp_type_meta_data_string_str32_utf8 : &amqp_type_meta_data_string_str8_utf8;
    description->overhead = description->meta_data->width + 1;
    return true;
}

amqp_type_t *amqp_call_encode_null(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_boolean(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_ubyte(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_byte(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_ushort(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_short(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_uint(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_int(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_small_uint(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_small_int(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_float(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_char(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_decimal32(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_ulong(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_long(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}
amqp_type_t *amqp_call_encode_small_ulong(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_small_long(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_double(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_timestamp(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_decimal64(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_uuid(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_binary(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    abort(); return 0;
}

amqp_type_t *amqp_call_encode_symbol(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    const char *p = va_arg(ap, const char *);
    return amqp_encode_symboln(context, p, description != 0 ? description->size : strlen(p));
}

amqp_type_t *amqp_call_encode_string_utf8(amqp_context_t *context, amqp_type_description_t *description, va_list ap)
{
    const char *p = va_arg(ap, const char *);
    return amqp_encode_string_utf8n(context, p, description != 0 ? description->size : strlen(p));
}

// s/^\(.*ampq_encode_meta_data_\)\([^ ]*\) = {/\1\2 = { amqp_type_describe_\2, /

amqp_type_encode_meta_data_t ampq_encode_meta_data_null = { amqp_type_describe_null,  amqp_call_encode_null,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_boolean = { amqp_type_describe_boolean,  amqp_call_encode_boolean,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_ubyte = { amqp_type_describe_ubyte,  amqp_call_encode_ubyte,  };
// TODO uint0 here
amqp_type_encode_meta_data_t ampq_encode_meta_data_byte = { amqp_type_describe_byte,  amqp_call_encode_byte,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_ushort = { amqp_type_describe_ushort,  amqp_call_encode_ushort,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_short = { amqp_type_describe_short,  amqp_call_encode_short,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_uint = { amqp_type_describe_uint,  amqp_call_encode_uint,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_int = { amqp_type_describe_int,  amqp_call_encode_int,  };

amqp_type_encode_meta_data_t ampq_encode_meta_data_small_uint = { amqp_type_describe_small_uint,  amqp_call_encode_small_uint,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_small_int = { amqp_type_describe_small_int,  amqp_call_encode_small_int,  };

amqp_type_encode_meta_data_t ampq_encode_meta_data_float = { amqp_type_describe_float,  amqp_call_encode_float,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_char = { amqp_type_describe_char,  amqp_call_encode_char,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_decimal32 = { amqp_type_describe_decimal32,  amqp_call_encode_decimal32,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_ulong = { amqp_type_describe_ulong,  amqp_call_encode_ulong,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_long = { amqp_type_describe_long,  amqp_call_encode_long,  };

amqp_type_encode_meta_data_t ampq_encode_meta_data_small_ulong = { amqp_type_describe_small_ulong,  amqp_call_encode_small_ulong,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_small_long = { amqp_type_describe_small_long,  amqp_call_encode_small_long,  };

amqp_type_encode_meta_data_t ampq_encode_meta_data_double = { amqp_type_describe_double,  amqp_call_encode_double,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_timestamp = { amqp_type_describe_timestamp,  amqp_call_encode_timestamp,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_decimal64 = { amqp_type_describe_decimal64,  amqp_call_encode_decimal64,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_uuid = { amqp_type_describe_uuid,  amqp_call_encode_uuid,  };

amqp_type_encode_meta_data_t ampq_encode_meta_data_binary = { amqp_type_describe_binary,  amqp_call_encode_binary,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_symbol = { amqp_type_describe_symbol,  amqp_call_encode_symbol,  };
amqp_type_encode_meta_data_t ampq_encode_meta_data_string_utf8 = { amqp_type_describe_string_utf8,  amqp_call_encode_string_utf8,  };

