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

#ifndef LIBAMQP_TRANSPORT_FRAME_FRAME_H
#define LIBAMQP_TRANSPORT_FRAME_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Context/Context.h"
#include "Codec/Type/TypeExtension.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_FRAME_TYPE_T
#define LIBAMQP_AMQP_FRAME_TYPE_T
typedef struct amqp_frame_t amqp_frame_t;
#endif

#define AMQP_FRAME_HEADER_DATA_OFFSET_MULTIPLIER 4

enum amqp_frame_types_t {
    AMQP_FRAME_TYPE = 0,
    AMQP_SASL_FRAME_TYPE = 1
};

typedef enum amqp_frame_type_type_t
{
    amqp_empty_frame = 1,
    amqp_sasl_mechanism_frame,
} amqp_frame_type_type_t;

struct amqp_frame_t
{
    uint32_t data_offset;
    uint8_t frame_type;
    union {
        uint16_t word;
        uint16_t channel;
    } type_specific;
    amqp_frame_type_type_t selector;
    union {
        int pad;
    } frames;
};

extern amqp_frame_t *amqp_decode_frame(amqp_context_t *context, amqp_buffer_t *buffer);

#ifdef __cplusplus
}
#endif
#endif
