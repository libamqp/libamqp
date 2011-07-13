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

#ifndef LIBAMQP_CODEC_DECODE_DECODE_H
#define LIBAMQP_CODEC_DECODE_DECODE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Codec/Convert.h"
#include "Codec/Type/Type.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

extern amqp_type_t *amqp_decode(amqp_context_t *);
extern amqp_type_t *amqp_decode_supress_messages(amqp_context_t *context);

// validate functions
int amqp_decode_described_type(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_null(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_boolean_true(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_boolean_false(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_boolean(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_ubyte(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_byte(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_ushort(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_short(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_uint(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_uint0(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_int(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_small_uint(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_small_int(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_float(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_char(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_decimal32(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_ulong0(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_ulong(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_long(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_small_ulong(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_small_long(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_double(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_timestamp(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_decimal64(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_uuid(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_binary_vbin8(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_string_str8_utf8(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_symbol_sym8(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_binary_vbin32(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_string_str32_utf8(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_symbol_sym32(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_list_8(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_map_8(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_list_32(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_map_32(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_array_8(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);
int amqp_decode_array_32(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);

int amqp_decode_extension_type(amqp_encoding_meta_data_t *meta_data, amqp_type_t *type);

extern amqp_type_t *amqp_list_element(amqp_type_t *type, int index);
extern amqp_type_t *amqp_map_element(amqp_type_t *type, int index);

#ifdef __cplusplus
}
#endif
#endif


