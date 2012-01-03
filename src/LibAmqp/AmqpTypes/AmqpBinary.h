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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_BINARY_H
#define LIBAMQP_AMQP_TYPES_AMQP_BINARY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"
#include "Codec/Type/TypeExtension.h"

#include "AmqpTypes/AmqpVariable.h"

struct amqp_binary_t
{
    amqp_leader_t leader;
    amqp_variable_t v;
};

extern void amqp_binary_initialize_as_null(amqp_context_t *context, amqp_binary_t *binary);
static inline
int amqp_binary_is_null(amqp_binary_t *binary)
{
    return binary->v.type == 0 && binary->v.data == 0;
}

extern void amqp_binary_initialize_from_type(amqp_context_t *context, amqp_binary_t *binary, amqp_type_t *type);
extern amqp_binary_t *amqp_binary_create_from_type(amqp_context_t *context,  amqp_type_t *type);

extern void amqp_binary_initialize(amqp_context_t *context, amqp_binary_t *binary, const uint8_t *data, size_t size);
extern amqp_binary_t *amqp_binary_create(amqp_context_t *context, const uint8_t *data, size_t size);
extern amqp_binary_t *amqp_binary_clone(amqp_context_t *context, amqp_binary_t *source);

static inline
void amqp_binary_cleanup(amqp_context_t *context, amqp_binary_t *binary)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) binary);
}

static inline
size_t amqp_binary_size(amqp_binary_t *binary)
{
    return binary->v.size;
}

extern int amqp_binary_to_bytes(amqp_binary_t *binary, uint8_t *buffer, size_t buffer_size);
extern int amqp_binary_compare(amqp_binary_t *lhs, amqp_binary_t *rhs);
extern int amqp_binary_compare_with_bytes(amqp_binary_t *lhs, const uint8_t *rhs, size_t size);
extern uint32_t amqp_binary_hash(amqp_binary_t *binary);
extern int amqp_binary_index_of(amqp_binary_t *binary, int c, int starting_position);

static inline
int amqp_binary_byte_get_at(amqp_binary_t *binary, size_t index)
{
    assert(binary && binary->v.type);
    return amqp_type_get_byte_at(binary->v.type, index);
}

#ifdef __cplusplus
}
#endif
#endif
