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

#ifndef LIBAMQP_TYPE_ENCODE_META_DATA_H
#define LIBAMQP_TYPE_ENCODE_META_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "Codec/Type/TypeMetaData.h"

extern amqp_type_encode_meta_data_t ampq_encode_meta_data_null;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_boolean;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_ubyte;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_byte;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_ushort;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_short;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_uint;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_int;

extern amqp_type_encode_meta_data_t ampq_encode_meta_data_small_uint;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_small_int;

extern amqp_type_encode_meta_data_t ampq_encode_meta_data_float;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_char;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_decimal32;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_ulong;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_long;

extern amqp_type_encode_meta_data_t ampq_encode_meta_data_small_ulong;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_small_long;

extern amqp_type_encode_meta_data_t ampq_encode_meta_data_double;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_timestamp;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_decimal64;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_uuid;

extern amqp_type_encode_meta_data_t ampq_encode_meta_data_binary;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_symbol;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_string_utf8;
extern amqp_type_encode_meta_data_t ampq_encode_meta_data_string_utf16;

#define amqp_type_d(t)  (&ampq_encode_meta_data_ ## t)

#ifdef __cplusplus
}
#endif
#endif


