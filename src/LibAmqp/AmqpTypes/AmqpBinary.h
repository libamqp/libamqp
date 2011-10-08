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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_BINARY_H
#define LIBAMQP_AMQP_TYPES_AMQP_BINARY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"
#include "Codec/Type/TypeExtension.h"

struct amqp_binary_t
{
    amqp_leader_t leader;
    amqp_type_t *type;
    size_t size;
    uint8_t *bytes;
};

extern void amqp_binary_initialize_from_type(amqp_context_t *context, amqp_binary_t *binary, amqp_type_t *type);
extern amqp_binary_t *amqp_binary_create_from_type(amqp_context_t *context,  amqp_type_t *type);

static inline
void amqp_binary_cleanup(amqp_context_t *context, amqp_binary_t *binary)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) binary);
}

static inline
size_t amqp_binary_size(amqp_binary_t *binary)
{
    return binary->size;
}

static inline
size_t amqp_binary_copy_to(amqp_binary_t *binary, uint8_t *buffer, size_t amount)
{
    return amqp_type_copy_to(binary->type, buffer, amount);
}

#ifdef __cplusplus
}
#endif
#endif
