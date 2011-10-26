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

#ifndef LIBAMQP_TYPE_META_DATA_H
#define LIBAMQP_TYPE_META_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdlib.h>

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_TYPE_T
#define LIBAMQP_AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

typedef struct amqp_encoding_meta_data_t amqp_encoding_meta_data_t;
typedef int amqp_decoder_t(amqp_context_t *context, amqp_buffer_t *buffer, amqp_encoding_meta_data_t *, amqp_type_t *);

typedef struct amqp_type_methods_t amqp_type_methods_t;
typedef void amqp_type_print_method_t(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer);
// TODO -  remove unnecessary indirection. There is only the print method.

struct amqp_type_methods_t
{
    amqp_type_print_method_t *print;
};

typedef enum {
    amqp_is_invalid = 0x00000000,
    amqp_is_null = 0x00000001,
    amqp_is_boolean = 0x00000002,
    amqp_is_unsigned = 0x00000004,
    amqp_is_byte = 0x00000008,
    amqp_is_short = 0x00000010,
    amqp_is_int = 0x00000020,
    amqp_is_long = 0x00000040,
    amqp_is_float = 0x00000080,
    amqp_is_double = 0x00000100,
    amqp_is_timestamp = 0x00000200,
    amqp_is_uuid = 0x00000400,
    amqp_is_decimal32 = 0x00000800,
    amqp_is_decimal64 = 0x00001000,
    amqp_is_decimal128 = 0x00002000,
    amqp_is_char = 0x00004000,

    amqp_is_fixed_mask = 0x00007fff,

    amqp_is_binary = 0x00008000,
    amqp_is_string = 0x00010000,
    amqp_is_symbol = 0x00020000,
    amqp_is_variable_mask = 0x00038000,

    amqp_is_list = 0x00040000,
    amqp_is_map = 0x00080000,
    amqp_is_array = 0x00100000,
    amqp_is_compound_mask = 0x001c0000,

    amqp_is_composite = 0x00200000,
    amqp_is_descriptor = 0x00400000,
    amqp_is_described = 0x00800000,
    amqp_is_extension = 0x01000000,

    amqp_is_encoded = 0x02000000,
    amqp_is_incomplete = 0x04000000,
} amqp_typedef_flags_t;

struct amqp_encoding_meta_data_t
{
    unsigned char format_code;
    int width;
    amqp_decoder_t *type_decoder;
    amqp_type_methods_t *methods;
    const char *name;
    const char *encoding_name;
    amqp_typedef_flags_t flags;
    int category_name_index;
};

typedef struct amqp_type_description_t amqp_type_description_t;
struct amqp_type_description_t
{
    size_t size;
    size_t overhead;
    amqp_encoding_meta_data_t *meta_data;
};

typedef struct amqp_type_encode_meta_data_t amqp_type_encode_meta_data_t;
typedef int amqp_describe_type_t(amqp_context_t *context, amqp_type_description_t *description, va_list ap);
typedef amqp_type_t *amqp_encode_caller_t(amqp_context_t *context, amqp_type_description_t *description, va_list ap);

struct amqp_type_encode_meta_data_t
{
    amqp_describe_type_t *describe;
    amqp_encode_caller_t *encode;
};

#define AMQP_TYPE_CATEGORY_FIXED        0
#define AMQP_TYPE_CATEGORY_VARIABLE     1
#define AMQP_TYPE_CATEGORY_COMPOUND     2
#define AMQP_TYPE_CATEGORY_ARRAY        3

extern const char *amqp_category_names[];

extern amqp_encoding_meta_data_t amqp_type_meta_data_described;
extern amqp_encoding_meta_data_t amqp_type_meta_data_null;
extern amqp_encoding_meta_data_t amqp_type_meta_data_boolean_true;
extern amqp_encoding_meta_data_t amqp_type_meta_data_boolean_false;
extern amqp_encoding_meta_data_t amqp_type_meta_data_boolean;
extern amqp_encoding_meta_data_t amqp_type_meta_data_uint_uint0;
extern amqp_encoding_meta_data_t amqp_type_meta_data_ulong_ulong0;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_fixed_0;

extern amqp_encoding_meta_data_t amqp_type_meta_data_ubyte;
extern amqp_encoding_meta_data_t amqp_type_meta_data_byte;
extern amqp_encoding_meta_data_t amqp_type_meta_data_uint_small_uint;
extern amqp_encoding_meta_data_t amqp_type_meta_data_ulong_small_ulong;
extern amqp_encoding_meta_data_t amqp_type_meta_data_int_small_int;
extern amqp_encoding_meta_data_t amqp_type_meta_data_long_small_long;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_fixed_1;

extern amqp_encoding_meta_data_t amqp_type_meta_data_ushort;
extern amqp_encoding_meta_data_t amqp_type_meta_data_short;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_fixed_2;

extern amqp_encoding_meta_data_t amqp_type_meta_data_uint;
extern amqp_encoding_meta_data_t amqp_type_meta_data_int;
extern amqp_encoding_meta_data_t amqp_type_meta_data_float_ieee_754;
extern amqp_encoding_meta_data_t amqp_type_meta_data_char_utf32;
extern amqp_encoding_meta_data_t amqp_type_meta_data_decimal32_ieee_754;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_fixed_4;

extern amqp_encoding_meta_data_t amqp_type_meta_data_ulong;
extern amqp_encoding_meta_data_t amqp_type_meta_data_long;
extern amqp_encoding_meta_data_t amqp_type_meta_data_double_ieee_754;
extern amqp_encoding_meta_data_t amqp_type_meta_data_timestamp_ms64;
extern amqp_encoding_meta_data_t amqp_type_meta_data_decimal64_ieee_754;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_fixed_8;

extern amqp_encoding_meta_data_t amqp_type_meta_data_uuid;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_fixed_16;

extern amqp_encoding_meta_data_t amqp_type_meta_data_binary_vbin8;
extern amqp_encoding_meta_data_t amqp_type_meta_data_binary_vbin32;

extern amqp_encoding_meta_data_t amqp_type_meta_data_string_str8_utf8;
extern amqp_encoding_meta_data_t amqp_type_meta_data_symbol_sym8;
extern amqp_encoding_meta_data_t amqp_type_meta_data_symbol_sym32;

extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_variable_1;
extern amqp_encoding_meta_data_t amqp_type_meta_data_string_str32_utf8;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_variable_4;
extern amqp_encoding_meta_data_t amqp_type_meta_data_list_0;
extern amqp_encoding_meta_data_t amqp_type_meta_data_list_8;
extern amqp_encoding_meta_data_t amqp_type_meta_data_map_8;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_compound_1;
extern amqp_encoding_meta_data_t amqp_type_meta_data_list_32;
extern amqp_encoding_meta_data_t amqp_type_meta_data_map_32;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_compound_4;
extern amqp_encoding_meta_data_t amqp_type_meta_data_array_8;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_array_1;
extern amqp_encoding_meta_data_t amqp_type_meta_data_array_32;
extern amqp_encoding_meta_data_t amqp_type_meta_data_extension_array_4;

extern amqp_encoding_meta_data_t *amqp_type_meta_data_lookup(amqp_context_t *context, int format_code);

#ifdef __cplusplus
}
#endif
#endif


