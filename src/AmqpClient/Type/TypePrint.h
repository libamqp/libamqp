#ifndef STORMMQ_AMQP_TYPE_PRINT_H
#define STORMMQ_AMQP_TYPE_PRINT_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Type/Type.h"

typedef int amqp_debug_printc_t(int c);

extern void amqp_type_print(amqp_type_t *type);
extern void amqp_type_print_formatted(amqp_type_t *type);

extern void amqp_type_method_described_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_null_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_boolean_true_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_boolean_false_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_ubyte_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_byte_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_ushort_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_short_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_uint_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_int_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_uint_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_int_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_float_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_char_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_decimal32_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_ulong_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_long_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_ulong_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_long_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_double_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_timestamp_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_decimal64_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_uuid_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_binary_vbin8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_binary_vbin32_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_symbol_sym8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_symbol_sym32_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_string_str8_utf8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_string_str32_utf8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_string_str8_utf16_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_string_str32_utf16_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_list_8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_map_8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_list_32_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_map_32_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_array_8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_array_32_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_extension_type_print(amqp_context_t *context, amqp_type_t *type);

#ifdef __cplusplus
}
#endif
#endif

