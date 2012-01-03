/*
   Copyright 2011-2012 StormMQ Limited

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

#ifndef LIBAMQP_CODEC_ENCODE_ENCODE_H
#define LIBAMQP_CODEC_ENCODE_ENCODE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include "libamqp_common.h"
#include "Codec/Type/Type.h"
#include "Codec/Convert.h"

//#include "types/binary/binary.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

amqp_type_t *amqp_encode_null(amqp_context_t *context, amqp_buffer_t *buffer);
amqp_type_t *amqp_encode_boolean(amqp_context_t *context, amqp_buffer_t *buffer, int value);

amqp_type_t *amqp_encode_ubyte(amqp_context_t *context, amqp_buffer_t *buffer, uint8_t value);
amqp_type_t *amqp_encode_byte(amqp_context_t *context, amqp_buffer_t *buffer, int8_t value);

amqp_type_t *amqp_encode_ushort(amqp_context_t *context, amqp_buffer_t *buffer, uint16_t value);
amqp_type_t *amqp_encode_short(amqp_context_t *context, amqp_buffer_t *buffer, int16_t value);

amqp_type_t *amqp_encode_uint0(amqp_context_t *context, amqp_buffer_t *buffer);
amqp_type_t *amqp_encode_uint(amqp_context_t *context, amqp_buffer_t *buffer, uint32_t value);
amqp_type_t *amqp_encode_int(amqp_context_t *context, amqp_buffer_t *buffer, int32_t value);

amqp_type_t *amqp_encode_small_uint(amqp_context_t *context, amqp_buffer_t *buffer, uint32_t value);
amqp_type_t *amqp_encode_small_int(amqp_context_t *context, amqp_buffer_t *buffer, int32_t value);

amqp_type_t *amqp_encode_float(amqp_context_t *context, amqp_buffer_t *buffer, float value);
amqp_type_t *amqp_encode_char(amqp_context_t *context, amqp_buffer_t *buffer, wchar_t value);

amqp_type_t *amqp_encode_decimal32(amqp_context_t *context, amqp_buffer_t *buffer, amqp_decimal32_t value);
amqp_type_t *amqp_encode_decimal64(amqp_context_t *context, amqp_buffer_t *buffer, amqp_decimal64_t value);

amqp_type_t *amqp_encode_ulong0(amqp_context_t *context, amqp_buffer_t *buffer);
amqp_type_t *amqp_encode_ulong(amqp_context_t *context, amqp_buffer_t *buffer, uint64_t value);
amqp_type_t *amqp_encode_long(amqp_context_t *context, amqp_buffer_t *buffer, int64_t value);
amqp_type_t *amqp_encode_small_ulong(amqp_context_t *context, amqp_buffer_t *buffer, uint64_t value);
amqp_type_t *amqp_encode_small_long(amqp_context_t *context, amqp_buffer_t *buffer, int64_t value);

amqp_type_t *amqp_encode_double(amqp_context_t *context, amqp_buffer_t *buffer, double value);
amqp_type_t *amqp_encode_timestamp(amqp_context_t *context, amqp_buffer_t *buffer, amqp_timestamp_t value);

amqp_type_t *amqp_encode_uuid(amqp_context_t *context, amqp_buffer_t *buffer, amqp_uuid_t *value);

amqp_type_t *amqp_encode_binary_vbin8(amqp_context_t *context, amqp_buffer_t *buffer, const unsigned char *value, size_t size);
amqp_type_t *amqp_encode_binary_vbin32(amqp_context_t *context, amqp_buffer_t *buffer, const unsigned char *value, size_t size);
amqp_type_t *amqp_encode_binary(amqp_context_t *context, amqp_buffer_t *buffer, const unsigned char *value, size_t size);

amqp_type_t *amqp_encode_symbol(amqp_context_t *context, amqp_buffer_t *buffer, const char *value);
amqp_type_t *amqp_encode_symbol_sym8(amqp_context_t *context, amqp_buffer_t *buffer, const char *value);
amqp_type_t *amqp_encode_symbol_sym32(amqp_context_t *context, amqp_buffer_t *buffer, const char *value);
amqp_type_t *amqp_encode_symboln(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size);
amqp_type_t *amqp_encode_symbol_sym8n(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size);
amqp_type_t *amqp_encode_symbol_sym32n(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size);

amqp_type_t *amqp_encode_string(amqp_context_t *context, amqp_buffer_t *buffer, const char *value);
amqp_type_t *amqp_encode_string(amqp_context_t *context, amqp_buffer_t *buffer, const char *value);
amqp_type_t *amqp_encode_stringn(amqp_context_t *context, amqp_buffer_t *buffer, const char *value, size_t size);

amqp_type_t *amqp_encode_list_8(amqp_context_t *context, amqp_buffer_t *buffer);
amqp_type_t *amqp_encode_list_32(amqp_context_t *context, amqp_buffer_t *buffer);

amqp_type_t *amqp_encode_map_8(amqp_context_t *context, amqp_buffer_t *buffer);
amqp_type_t *amqp_encode_map_32(amqp_context_t *context, amqp_buffer_t *buffer);

amqp_type_t *amqp_encode_array_8(amqp_context_t *context, amqp_buffer_t *buffer);
amqp_type_t *amqp_encode_array_32(amqp_context_t *context, amqp_buffer_t *buffer);

amqp_type_t *amqp_complete_type(amqp_context_t *context, amqp_buffer_t *buffer, amqp_type_t *type);
amqp_type_t *amqp_start_encode_described_type(amqp_context_t *context, amqp_buffer_t *buffer);

#ifdef __cplusplus
}
#endif
#endif


