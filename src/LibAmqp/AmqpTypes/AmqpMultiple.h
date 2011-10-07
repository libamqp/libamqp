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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_MULTIPLE_H
#define LIBAMQP_AMQP_TYPES_AMQP_MULTIPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"

#define AMQP_MULTIPLE_DEFAULT_SIZE  8

struct amqp_multiple_symbol_t
{
    amqp_leader_t leader;
    int size;
    union
    {
        amqp_symbol_t *direct[AMQP_MULTIPLE_DEFAULT_SIZE];
        amqp_symbol_t **indirect;
    } array;
};

static inline
int amqp_multiple_symbol_size(amqp_multiple_symbol_t *multiple)
{
    return multiple->size;
}

static inline
amqp_symbol_t **amqp_multiple_symbol_array(amqp_multiple_symbol_t *multiple)
{
    return multiple->size > AMQP_MULTIPLE_DEFAULT_SIZE ? multiple->array.indirect : &multiple->array.direct[0];
}

static inline
amqp_symbol_t *amqp_multiple_symbol_get(amqp_multiple_symbol_t *multiple, int element)
{
    return amqp_multiple_symbol_array(multiple)[element];
}
extern void amqp_multiple_symbol_initialize(amqp_context_t *context, amqp_multiple_symbol_t *multiple, int size);
extern void amqp_multiple_symbol_cleanup(amqp_context_t *context, amqp_multiple_symbol_t *multiple);

extern amqp_multiple_symbol_t *amqp_multiple_symbol_create(amqp_context_t *context, int size);
extern void amqp_multiple_symbol_free(amqp_context_t *context, amqp_multiple_symbol_t *multiple);

#ifdef __cplusplus
}
#endif
#endif
