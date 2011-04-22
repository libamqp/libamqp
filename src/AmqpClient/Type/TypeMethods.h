#ifndef STORMMQ_AMQP_TYPE_TYPE_METHODS_H
#define STORMMQ_AMQP_TYPE_TYPE_METHODS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Type/TypeMetaData.h"

// :s/^\(.*amqp_methods_\)\([^ ]*\);
extern amqp_type_methods_t amqp_methods_described;
extern amqp_type_methods_t amqp_methods_null;
extern amqp_type_methods_t amqp_methods_boolean_true;
extern amqp_type_methods_t amqp_methods_boolean_false;
extern amqp_type_methods_t amqp_methods_ubyte;
extern amqp_type_methods_t amqp_methods_byte;
extern amqp_type_methods_t amqp_methods_ushort;
extern amqp_type_methods_t amqp_methods_short;
extern amqp_type_methods_t amqp_methods_uint;
extern amqp_type_methods_t amqp_methods_int;

extern amqp_type_methods_t amqp_methods_small_uint;
extern amqp_type_methods_t amqp_methods_small_int;

extern amqp_type_methods_t amqp_methods_float;
extern amqp_type_methods_t amqp_methods_char;
extern amqp_type_methods_t amqp_methods_decimal32;
extern amqp_type_methods_t amqp_methods_ulong;
extern amqp_type_methods_t amqp_methods_long;

extern amqp_type_methods_t amqp_methods_small_ulong;
extern amqp_type_methods_t amqp_methods_small_long;

extern amqp_type_methods_t amqp_methods_double;
extern amqp_type_methods_t amqp_methods_timestamp;
extern amqp_type_methods_t amqp_methods_decimal64;
extern amqp_type_methods_t amqp_methods_uuid;

extern amqp_type_methods_t amqp_methods_binary_vbin8;
extern amqp_type_methods_t amqp_methods_string_str8_utf8;
extern amqp_type_methods_t amqp_methods_string_str8_utf16;
extern amqp_type_methods_t amqp_methods_symbol_sym8;
extern amqp_type_methods_t amqp_methods_binary_vbin32;
extern amqp_type_methods_t amqp_methods_string_str32_utf8;
extern amqp_type_methods_t amqp_methods_string_str32_utf16;
extern amqp_type_methods_t amqp_methods_symbol_sym32;
extern amqp_type_methods_t amqp_methods_list_8;
extern amqp_type_methods_t amqp_methods_map_8;
extern amqp_type_methods_t amqp_methods_list_32;
extern amqp_type_methods_t amqp_methods_map_32;
extern amqp_type_methods_t amqp_methods_array_8;
extern amqp_type_methods_t amqp_methods_array_32;

extern amqp_type_methods_t amqp_methods_extension_type;

#ifdef __cplusplus
}
#endif
#endif
