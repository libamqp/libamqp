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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_FRAME_DISPATCH_H
#define LIBAMQP_TRANSPORT_CONNECTION_FRAME_DISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"

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

extern void amqp_dispatch_sasl_mechanisms(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_sasl_init(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_sasl_challenge(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_sasl_response(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_sasl_outcome(amqp_connection_t *connection, amqp_frame_t *frame);

extern void amqp_dispatch_amqp_open(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_begin(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_attach(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_flow(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_transfer(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_disposition(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_detach(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_end(amqp_connection_t *connection, amqp_frame_t *frame);
extern void amqp_dispatch_amqp_close(amqp_connection_t *connection, amqp_frame_t *frame);

#ifdef __cplusplus
}
#endif
#endif
