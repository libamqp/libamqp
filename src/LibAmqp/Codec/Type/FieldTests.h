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

#ifndef LIBAMQP_CODEC_TYPE_FIELD_TESTS_H
#define LIBAMQP_CODEC_TYPE_FIELD_TESTS_H

#include "Codec/Type/Type.h"
#include "Codec/Type/TypeExtension.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline
int amqp_field_is_role(amqp_type_t *type)
{
    return amqp_type_is_boolean(type);
}

static inline
int amqp_field_is_sender_settle_mode(amqp_type_t *type)
{
    return amqp_type_is_null(type) ||
        (amqp_type_is_ubyte(type) && amqp_type_to_ubyte(type) <= amqp_sender_settle_mode_mixed);
}

static inline
int amqp_field_is_receiver_settle_mode(amqp_type_t *type)
{
    return amqp_type_is_null(type) ||
        (amqp_type_is_ubyte(type) && amqp_type_to_ubyte(type) <= amqp_receiver_settle_mode_second);
}

static inline
int amqp_field_is_wildcard(amqp_type_t *type)
{
    return amqp_type_is_valid(type);
}

static inline
int amqp_field_is_boolean(amqp_type_t *type)
{
    return amqp_type_is_null(type) || amqp_type_is_boolean(type);
}

static inline
int amqp_field_is_sequence_no(amqp_type_t *type)
{
    return amqp_type_is_null(type) || amqp_type_is_uint(type);
}

static inline
int amqp_field_is_multiple_symbol(amqp_type_t *type)
{
    return amqp_type_is_null(type) || amqp_type_is_symbol(type) || amqp_type_is_array(type);
}

static inline
int amqp_field_is_message_format(amqp_type_t *type)
{
    return amqp_type_is_uint(type);
}
static inline
int amqp_field_is_delivery_number(amqp_type_t *type)
{
    return amqp_type_is_uint(type);
}
static inline
int amqp_field_is_delivery_tag(amqp_type_t *type)
{
    return amqp_type_is_binary(type);
}

#ifdef __cplusplus
}
#endif
#endif


