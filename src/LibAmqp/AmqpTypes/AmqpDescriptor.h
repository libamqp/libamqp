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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_DESCRIPTOR_H
#define LIBAMQP_AMQP_TYPES_AMQP_DESCRIPTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Context/Context.h"
#include "AmqpTypes/AmqpSymbol.h"

typedef struct amqp_descriptor_t
{
    uint32_t domain;
    uint32_t id;
} amqp_descriptor_t;

typedef struct amqp_symbolic_descriptor_t
{
    const char *symbolic;
    amqp_descriptor_t descriptor;
} amqp_symbolic_descriptor_t;

extern void amqp_load_descriptors(amqp_context_t *context, amqp_map_t *map);
extern void amqp_descriptors_cleannup(amqp_context_t *context, amqp_map_t *map);
extern amqp_descriptor_t *amqp_descriptor_lookup(amqp_map_t *map, amqp_symbol_t *symbol);

#ifdef __cplusplus
}
#endif
#endif
