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

#ifndef LIBAMQP_AMQP_TYPES_DECODE_AMQP_TYPES_DECODE_H
#define LIBAMQP_AMQP_TYPES_DECODE_AMQP_TYPES_DECODE_H

#include <stdlib.h>
#include "AmqpTypes/AmqpTypes.h"

int amqp_decode_type_symbol(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_symbol_t *symbol);
int amqp_decode_type_binary(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_binary_t *binary);
int amqp_decode_type_string(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_string_t *string);
int amqp_decode_type_map(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_map_t *map);

int amqp_decode_mandatory_type_symbol(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_symbol_t *symbol);
int amqp_decode_mandatory_type_binary(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_binary_t *binary);
int amqp_decode_mandatory_type_string(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_string_t *string);
int amqp_decode_mandatory_type_map(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_map_t *map);

int amqp_decode_primitive_byte(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int8_t *result, int8_t default_value);
int amqp_decode_primitive_ubyte(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint8_t *result, uint8_t default_value);
int amqp_decode_primitive_short(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int16_t *result, int16_t default_value);
int amqp_decode_primitive_ushort(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint16_t *result, uint16_t default_value);
int amqp_decode_primitive_int(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int32_t *result, int32_t default_value);
int amqp_decode_primitive_uint(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint32_t *result, uint32_t default_value);
int amqp_decode_primitive_long(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int64_t *result, int64_t default_value);
int amqp_decode_primitive_ulong(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint64_t *result, uint64_t default_value);
int amqp_decode_primitive_boolean(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int *result, int default_value);

int amqp_decode_mandatory_primitive_byte(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int8_t *result);
int amqp_decode_mandatory_primitive_ubyte(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint8_t *result);
int amqp_decode_mandatory_primitive_short(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int16_t *result);
int amqp_decode_mandatory_primitive_ushort(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint16_t *result);
int amqp_decode_mandatory_primitive_int(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int32_t *result);
int amqp_decode_mandatory_primitive_uint(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint32_t *result);
int amqp_decode_mandatory_primitive_long(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int64_t *result);
int amqp_decode_mandatory_primitive_ulong(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint64_t *result);
int amqp_decode_mandatory_primitive_boolean(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int *result);

#endif
