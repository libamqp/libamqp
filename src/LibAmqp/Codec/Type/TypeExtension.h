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

#ifndef LIBAMQP_CODEC_TYPE_TYPE_EXTENSION_H
#define LIBAMQP_CODEC_TYPE_TYPE_EXTENSION_H

#include "Codec/Type/Type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
static inline
bool amqp_type_is_convert_ok(amqp_type_t *type)
{
    return type->flags.convert_failed == 0;
}
*/

static inline
bool amqp_type_is_convert_failed(amqp_type_t *type)
{
    return type->flags.convert_failed;
}

static inline
int amqp_type_convert_check_failed(amqp_type_t *type, int convert_result)
{
    return !(type->flags.convert_failed = type->flags.convert_failed || !convert_result);
}

static inline
amqp_type_t *amqp_type_get_descriptor(amqp_type_t *type)
{
    return amqp_type_convert_check_failed(type, amqp_type_is_described(type))
        ? type->value.described.elements[0]
        : type;
}

static inline
amqp_type_t *amqp_type_get_described(amqp_type_t *type)
{
    return amqp_type_convert_check_failed(type, amqp_type_is_described(type))
        ? type->value.described.elements[1]
        : type;
}

static inline
int amqp_type_is_symbol(amqp_type_t *type)
{
    return type->format_code == 0xa3 || type->format_code == 0xa3;
}

static inline
int amqp_type_is_ulong(amqp_type_t *type)
{
    return type->format_code == 0x53 || type->format_code == 0x80 || type->format_code == 0x44;
}

static inline
uint64_t amqp_type_to_ulong(amqp_type_t *type)
{
    return amqp_type_convert_check_failed(type, amqp_type_is_ulong(type))
        ? type->value.b8._ulong
        : 0;
}

#ifdef __cplusplus
}
#endif
#endif


