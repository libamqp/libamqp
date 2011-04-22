#include <stdio.h>

#include "Type/TypeMetaData.h"
#include "Type/TypeMethods.h"
#include "Codec/Decode.h"
#include "Type/Type.h"

const char *amqp_category_names[] = {
    "fixed",
    "variable",
    "compound",
    "array"
};

amqp_type_meta_data_t amqp_type_meta_data_described = {
    0x00,
    0,
    amqp_decode_described_type,
    &amqp_methods_described,
    "described",
    "",
    AMQP_TYPE_CATEGORY_COMPOUND,
};
amqp_type_meta_data_t amqp_type_meta_data_null = {
    0x40,
    0,
    amqp_decode_null,
    &amqp_methods_null,
    "null",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_boolean_true = {
    0x41,
    0,
    amqp_decode_boolean_true,
    &amqp_methods_boolean_true,
    "boolean",
    "true",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_boolean_false = {
    0x42,
    0,
    amqp_decode_boolean_false,
    &amqp_methods_boolean_false,
    "boolean",
    "false",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_fixed_0 = {
    0x4F,
    0,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "fixed-0",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_ubyte = {
    0x50,
    1,
    amqp_decode_ubyte,
    &amqp_methods_ubyte,
    "ubyte",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_byte = {
    0x51,
    1,
    amqp_decode_byte,
    &amqp_methods_byte,
    "byte",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_uint_small_uint = {
    0x52,
    1,
    amqp_decode_small_uint,
    &amqp_methods_small_uint,
    "uint",
    "smalluint",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_ulong_small_ulong = {
    0x53,
    1,
    amqp_decode_small_ulong,
    &amqp_methods_small_ulong,
    "ulong",
    "smallulong",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_int_small_int = {
    0x54,
    1,
    amqp_decode_small_int,
    &amqp_methods_small_int,
    "int",
    "smallint",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_long_small_long = {
    0x55,
    1,
    amqp_decode_small_long,
    &amqp_methods_small_long,
    "long",
    "smalllong",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_fixed_1 = {
    0x5F,
    1,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "fixed-1",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_ushort = {
    0x60,
    2,
    amqp_decode_ushort,
    &amqp_methods_ushort,
    "ushort",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_short = {
    0x61,
    2,
    amqp_decode_short,
    &amqp_methods_short,
    "short",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_fixed_2 = {
    0x6F,
    2,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "fixed-2",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_uint = {
    0x70,
    4,
    amqp_decode_uint,
    &amqp_methods_uint,
    "uint",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_int = {
    0x71,
    4,
    amqp_decode_int,
    &amqp_methods_int,
    "int",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_float_ieee_754 = {
    0x72,
    4,
    amqp_decode_float,
    &amqp_methods_float,
    "float",
    "ieee-754",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_char_utf32 = {
    0x73,
    4,
    amqp_decode_char,
    &amqp_methods_char,
    "char",
    "utf32",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_decimal32_ieee_754 = {
    0x74,
    4,
    amqp_decode_decimal32,
    &amqp_methods_decimal32,
    "decimal32",
    "ieee-754",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_fixed_4 = {
    0x7F,
    4,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "fixed-4",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_ulong = {
    0x80,
    8,
    amqp_decode_ulong,
    &amqp_methods_ulong,
    "ulong",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_long = {
    0x81,
    8,
    amqp_decode_long,
    &amqp_methods_long,
    "long",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_double_ieee_754 = {
    0x82,
    8,
    amqp_decode_double,
    &amqp_methods_double,
    "double",
    "ieee-754",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_timestamp_ms64 = {
    0x83,
    8,
    amqp_decode_timestamp,
    &amqp_methods_timestamp,
    "timestamp",
    "ms64",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_decimal64_ieee_754 = {
    0x84,
    8,
    amqp_decode_decimal64,
    &amqp_methods_decimal64,
    "decimal64",
    "ieee-754",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_fixed_8 = {
    0x8F,
    8,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "fixed-8",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_uuid = {
    0x98,
    16,
    amqp_decode_uuid,
    &amqp_methods_uuid,
    "uuid",
    "",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_fixed_16 = {
    0x9F,
    16,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "fixed-16",
    AMQP_TYPE_CATEGORY_FIXED,
};
amqp_type_meta_data_t amqp_type_meta_data_binary_vbin8 = {
    0xA0,
    1,
    amqp_decode_binary_vbin8,
    &amqp_methods_binary_vbin8,
    "binary",
    "vbin8",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_string_str8_utf8 = {
    0xA1,
    1,
    amqp_decode_string_str8_utf8,
    &amqp_methods_string_str8_utf8,
    "string",
    "str8-utf8",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_string_str8_utf16 = {
    0xA2,
    1,
    amqp_decode_string_str8_utf16,
    &amqp_methods_string_str8_utf16,
    "string",
    "str8-utf16",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_symbol_sym8 = {
    0xA3,
    1,
    amqp_decode_symbol_sym8,
    &amqp_methods_symbol_sym8,
    "symbol",
    "sym8",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_variable_1 = {
    0xAF,
    1,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "variable-1",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_binary_vbin32 = {
    0xB0,
    4,
    amqp_decode_binary_vbin32,
    &amqp_methods_binary_vbin32,
    "binary",
    "vbin32",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_string_str32_utf8 = {
    0xB1,
    4,
    amqp_decode_string_str32_utf8,
    &amqp_methods_string_str32_utf8,
    "string",
    "str32-utf8",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_string_str32_utf16 = {
    0xB2,
    4,
    amqp_decode_string_str32_utf16,
    &amqp_methods_string_str32_utf16,
    "string",
    "str32-utf16",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_symbol_sym32 = {
    0xB3,
    4,
    amqp_decode_symbol_sym32,
    &amqp_methods_symbol_sym32,
    "symbol",
    "sym32",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_variable_4 = {
    0xBF,
    4,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "variable-4",
    AMQP_TYPE_CATEGORY_VARIABLE,
};
amqp_type_meta_data_t amqp_type_meta_data_list_8 = {
    0xC0,
    1,
    amqp_decode_list_8,
    &amqp_methods_list_8,
    "list",
    "list8",
    AMQP_TYPE_CATEGORY_COMPOUND,
};
amqp_type_meta_data_t amqp_type_meta_data_map_8 = {
    0xC1,
    1,
    amqp_decode_map_8,
    &amqp_methods_map_8,
    "map",
    "map8",
    AMQP_TYPE_CATEGORY_COMPOUND,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_compound_1 = {
    0xCF,
    1,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "compound-1",
    AMQP_TYPE_CATEGORY_COMPOUND,
};
amqp_type_meta_data_t amqp_type_meta_data_list_32 = {
    0xD0,
    4,
    amqp_decode_list_32,
    &amqp_methods_list_32,
    "list",
    "list32",
    AMQP_TYPE_CATEGORY_COMPOUND,
};
amqp_type_meta_data_t amqp_type_meta_data_map_32 = {
    0xD1,
    4,
    amqp_decode_map_32,
    &amqp_methods_map_32,
    "map",
    "map32",
    AMQP_TYPE_CATEGORY_COMPOUND,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_compound_4 = {
    0xDF,
    4,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "compound-4",
    AMQP_TYPE_CATEGORY_COMPOUND,
};
amqp_type_meta_data_t amqp_type_meta_data_array_8 = {
    0xE0,
    1,
    amqp_decode_array_8,
    &amqp_methods_array_8,
    "list",
    "array8",
    AMQP_TYPE_CATEGORY_ARRAY,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_array_1 = {
    0xEF,
    1,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "array-1",
    AMQP_TYPE_CATEGORY_ARRAY,
};
amqp_type_meta_data_t amqp_type_meta_data_array_32 = {
    0xF0,
    4,
    amqp_decode_array_32,
    &amqp_methods_array_32,
    "list",
    "array32",
    AMQP_TYPE_CATEGORY_ARRAY,
};
amqp_type_meta_data_t amqp_type_meta_data_extension_array_4 = {
    0xFF,
    4,
    amqp_decode_extension_type,
    &amqp_methods_extension_type,
    "extension",
    "array-4",
    AMQP_TYPE_CATEGORY_ARRAY,
};

amqp_type_meta_data_t *type_lookup_table[] = {
    &amqp_type_meta_data_described,
    &amqp_type_meta_data_null,
    &amqp_type_meta_data_boolean_true,
    &amqp_type_meta_data_boolean_false,
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
    &amqp_type_meta_data_string_str8_utf8,
    &amqp_type_meta_data_string_str8_utf16,
    &amqp_type_meta_data_symbol_sym8,
    &amqp_type_meta_data_extension_variable_1,
    &amqp_type_meta_data_binary_vbin32,
    &amqp_type_meta_data_string_str32_utf8,
    &amqp_type_meta_data_string_str32_utf16,
    &amqp_type_meta_data_symbol_sym32,
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
};

static const int ntypes = sizeof(type_lookup_table) / sizeof(amqp_type_meta_data_t *);


// TODO - this binary search can take 5 compares to find a value, do lookup table for fast version
amqp_type_meta_data_t *
amqp_type_meta_data_lookup(amqp_context_t *context, int format_code)
{
    int s = 0;
    int e = ntypes;
    int m;
    amqp_type_meta_data_t *candidate;

    while (s < e)
    {
        m = s + (e - s) / 2;

        candidate = type_lookup_table[m];

        if (candidate->format_code == format_code)
        {
            return candidate;
        }
        else if (format_code < candidate->format_code)
        {
            e = m;
        }
        else
        {
            s = m + 1;
        }
    }

    return 0;
}
