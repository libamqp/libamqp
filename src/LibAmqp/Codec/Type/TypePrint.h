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

#ifndef LIBAMQP_CODEC_TYPE_TYPE_PRINT_H
#define LIBAMQP_CODEC_TYPE_TYPE_PRINT_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Codec/Type/Type.h"

typedef int amqp_debug_printc_t(int c);

extern void amqp_type_print(amqp_type_t *type);
extern void amqp_type_print_formatted(amqp_type_t *type);

extern void amqp_type_method_described_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_null_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_boolean_true_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_boolean_false_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_boolean_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_ubyte_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_byte_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_ushort_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_short_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_uint_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_uint0_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_int_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_uint_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_int_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_float_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_char_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_decimal32_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_decimal64_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_decimal128_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_ulong_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_ulong0_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_long_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_ulong_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_small_long_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_double_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_timestamp_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_uuid_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_binary_vbin8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_binary_vbin32_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_symbol_sym8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_symbol_sym32_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_string_str8_utf8_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_string_str32_utf8_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_list_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_map_print(amqp_context_t *context, amqp_type_t *type);
extern void amqp_type_method_array_print(amqp_context_t *context, amqp_type_t *type);

extern void amqp_type_method_extension_type_print(amqp_context_t *context, amqp_type_t *type);

#ifdef __cplusplus
}
#endif
#endif


