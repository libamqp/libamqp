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

#define AMQP_DESCRIPTOR_DOMAIN  0x00

typedef enum amqp_frame_descriptor_id_t
{
    AMQP_FRAME_ID_OPEN_LIST = 0x00000010,
    AMQP_FRAME_ID_BEGIN_LIST = 0x00000011,
    AMQP_FRAME_ID_ATTACH_LIST = 0x00000012,
    AMQP_FRAME_ID_FLOW_LIST = 0x00000013,
    AMQP_FRAME_ID_TRANSFER_LIST = 0x00000014,
    AMQP_FRAME_ID_DISPOSITION_LIST = 0x00000015,
    AMQP_FRAME_ID_DETACH_LIST = 0x00000016,
    AMQP_FRAME_ID_END_LIST = 0x00000017,
    AMQP_FRAME_ID_CLOSE_LIST = 0x00000018,
    AMQP_FRAME_ID_ERROR_LIST = 0x0000001d,
    AMQP_FRAME_ID_SASL_MECHANISMS_LIST = 0x00000040,
    AMQP_FRAME_ID_SASL_INIT_LIST = 0x00000041,
    AMQP_FRAME_ID_SASL_CHALLENGE_LIST = 0x00000042,
    AMQP_FRAME_ID_SASL_RESPONSE_LIST = 0x00000043,
    AMQP_FRAME_ID_SASL_OUTCOME_LIST = 0x00000044
} amqp_frame_descriptor_id_t;

typedef enum amqp_frame_descriptor_type_t
{
    amqp_empty_frame = 1,
    amqp_unsupported_descriptor,
    amqp_amqp_descriptor,
    amqp_sasl_descriptor,
} amqp_frame_type_type_t;

typedef struct amqp_descriptor_t
{
    uint32_t domain;
    uint32_t id;
    amqp_frame_type_type_t group;
} amqp_descriptor_t;

typedef struct amqp_symbolic_descriptor_t
{
    const char *symbolic;
    amqp_descriptor_t descriptor;
} amqp_symbolic_descriptor_t;

extern amqp_map_t *amqp_load_descriptors(amqp_context_t *context);
extern void amqp_descriptors_cleanup(amqp_context_t *context, amqp_map_t *map);
extern amqp_descriptor_t *amqp_descriptor_lookup(amqp_map_t *map, amqp_symbol_t *symbol);

static inline
amqp_descriptor_t *amqp_context_descriptor_lookup(amqp_context_t *context, amqp_symbol_t *symbol)
{
    return amqp_descriptor_lookup(context->reference.amqp_descriptors, symbol);
}

#ifdef __cplusplus
}
#endif
#endif
