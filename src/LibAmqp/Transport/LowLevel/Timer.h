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

#ifndef LIBAMQP_TRANSPORT_LOW_LEVEL_TIMER_H
#define LIBAMQP_TRANSPORT_LOW_LEVEL_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Transport/LowLevel/EventLoop.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

typedef struct amqp_timer_t amqp_timer_t;
typedef void (*amqp_timer_expiry_handler_t)(amqp_context_t *context, amqp_timer_t *timer);

struct amqp_timer_t
{
    ev_timer timer;
    amqp_context_t *context;
    amqp_timer_expiry_handler_t expiry_handler;
    void *argument;
};

extern amqp_timer_t *amqp_timer_initialize(amqp_context_t *context, amqp_timer_expiry_handler_t expiry_handler, void *argument);
extern void amqp_timer_destroy(amqp_context_t *context, amqp_timer_t *timer);
extern void amqp_timer_start(amqp_context_t *context, amqp_timer_t *timer, ev_tstamp delay);
extern void amqp_timer_stop(amqp_context_t *context, amqp_timer_t *timer);

#ifdef __cplusplus
}
#endif
#endif
