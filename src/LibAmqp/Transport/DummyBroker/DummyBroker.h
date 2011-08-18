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

#ifndef LIBAMQP_TRANSPORT_DUMMY_BROKER_DUMMY_BROKER_H
#define LIBAMQP_TRANSPORT_DUMMY_BROKER_DUMMY_BROKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
#define LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
typedef struct amqp_event_thread_t amqp_event_thread_t;
#endif

typedef struct amqp_dummy_broker_t
{
    amqp_event_thread_t *thread;
} amqp_dummy_broker_t;

extern amqp_dummy_broker_t *amqp_dummy_broker_initialize(amqp_context_t *context);
extern void amqp_dummy_broker_destroy(amqp_context_t *context, amqp_dummy_broker_t *broker);

#ifdef __cplusplus
}
#endif
#endif
