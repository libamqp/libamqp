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

#include "Amqp/AmqpSecurity.h"
#include "Amqp/AmqpTransport.h"
#include "Amqp/AmqpMessaging.h"
#include "Amqp/AmqpTransactions.h"

#include "AmqpTypes/AmqpDescriptor.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_FRAME_TYPE_T
#define LIBAMQP_AMQP_FRAME_TYPE_T
typedef struct amqp_frame_t amqp_frame_t;
#endif

#ifndef LIBAMQP_AMQP_CONNECTION_TYPE_T
#define LIBAMQP_AMQP_CONNECTION_TYPE_T
typedef struct amqp_connection_t amqp_connection_t;
#endif

#define AMQP_FRAME_HEADER_DATA_OFFSET_MULTIPLIER 4

enum amqp_frame_types_t {
    AMQP_FRAME_TYPE = 0,
    AMQP_SASL_FRAME_TYPE = 1
};

struct amqp_frame_t
{
    uint32_t data_offset;
    uint8_t frame_type;
    union {
        uint16_t word;
        uint16_t channel;
    } type_specific;

    amqp_type_t *type;      // Useful for debugging but it uses extra memory.

    amqp_descriptor_t descriptor;

    union {
        union {
            amqp_security_sasl_mechanisms_t mechanisms;
            amqp_security_sasl_init_t init;
            amqp_security_sasl_challenge_t challenge;
            amqp_security_sasl_response_t response;
            amqp_security_sasl_outcome_t outcome;
        } sasl;
        union {
            amqp_transport_open_t open;
            amqp_transport_begin_t begin;
            amqp_transport_attach_t attach;
            amqp_transport_flow_t flow;
            amqp_transport_transfer_t transfer;
            amqp_transport_disposition_t disposition;
            amqp_transport_detach_t detach;
            amqp_transport_end_t end;
            amqp_transport_close_t close;
        } amqp;
    } frames;

    void (*dispatch)(amqp_connection_t *connection, amqp_frame_t *frame);
    void (*cleanup)(amqp_context_t *context, amqp_frame_t *frame);
};

extern amqp_frame_t *amqp_decode_sasl_frame(amqp_context_t *context, amqp_buffer_t *buffer);
extern amqp_frame_t *amqp_decode_amqp_frame(amqp_context_t *context, amqp_buffer_t *buffer);

extern void amqp_frame_dump(amqp_context_t *context, amqp_frame_t *frame);
extern void amqp_frame_cleanup(amqp_context_t *context, amqp_frame_t *frame);

#ifdef __cplusplus
}
#endif
#endif
