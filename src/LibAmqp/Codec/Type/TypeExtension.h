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
#include "Amqp/AmqpDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline
int amqp_type_is_null(amqp_type_t *type)
{
    return type->flags.is_null;
}

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
int amqp_type_convert_set_failed(amqp_type_t *type)
{
    return !(type->flags.convert_failed = true);
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

static inline
int amqp_type_is_short(amqp_type_t *type)
{
    return type->format_code == 0x61;
}

static inline
int16_t amqp_type_to_short(amqp_type_t *type)
{
    return amqp_type_convert_check_failed(type, amqp_type_is_short(type))
        ? type->value.b2._short
        : 0;
}

static inline
size_t amqp_type_copy_to(amqp_type_t *type, uint8_t *buffer, size_t amount)
{
    if (amqp_type_convert_check_failed(type, amqp_type_is_variable(type)))
    {
        // TODO - push into buffer and do block copy of fragments
        size_t i, j;
        for (i = 0, j = type->position.index; i < type->position.size && i < amount; i++, j++)
        {
            buffer[i] = amqp_unchecked_getc_at(type->value.variable.buffer, j);
        }
        return i;
    }
    return 0;
}

static inline
uint8_t amqp_type_get_byte_at(amqp_type_t *type, size_t index)
{
    assert(amqp_type_is_variable(type));
    return amqp_unchecked_getc_at(type->value.variable.buffer, type->position.index + index);
}

static inline
int amqp_type_is_ubyte(amqp_type_t *type)
{
    return type->format_code == 0x50;
}

static inline
uint8_t amqp_type_to_ubyte(amqp_type_t *type)
{
    return amqp_type_convert_check_failed(type, amqp_type_is_ubyte(type))
        ? type->value.b1._unsigned
        : 0;
}

static inline
int amqp_type_is_ushort(amqp_type_t *type)
{
    return type->format_code == 0x60;
}

static inline
uint8_t amqp_type_to_ushort(amqp_type_t *type)
{
    return amqp_type_convert_check_failed(type, amqp_type_is_ushort(type))
        ? type->value.b2._ushort
        : 0;
}

static inline
int amqp_type_is_uint(amqp_type_t *type)
{
    return type->format_code == 0x70 || type->format_code == 0x52 || type->format_code == 0x43;
}

static inline
uint8_t amqp_type_to_uint(amqp_type_t *type)
{
    return amqp_type_convert_check_failed(type, amqp_type_is_uint(type))
        ? type->value.b4._uint
        : 0;
}

static inline
int amqp_type_is_boolean(amqp_type_t *type)
{
    return type->format_code == 0x41 || type->format_code == 0x42 || type->format_code == 0x56;
}

static inline
uint8_t amqp_type_to_boolean(amqp_type_t *type)
{
    assert(amqp_type_is_boolean(type));
    not_implemented(todo);
}

#ifdef __cplusplus
}
#endif
#endif


