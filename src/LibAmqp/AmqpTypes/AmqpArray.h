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

#ifndef LIBAMQP_AMQP_TYPES_AMQP_ARRAY_H
#define LIBAMQP_AMQP_TYPES_AMQP_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"

struct amqp_array_t
{
    amqp_leader_t leader;
    amqp_type_t *type;
    size_t capacity;
    size_t size;
    void **elements;
};

extern void amqp_array_initialize(amqp_context_t *context, amqp_array_t *array, size_t size);
extern void amqp_array_initialize_from_type(amqp_context_t *context, amqp_array_t *array, amqp_type_t *type);

static inline
void amqp_array_cleanup(amqp_context_t *context, amqp_array_t *array)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) array);
}

#ifdef __cplusplus
}
#endif
#endif
