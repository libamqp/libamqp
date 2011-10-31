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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_WILDCARD_H
#define LIBAMQP_AMQP_TYPES_AMQP_WILDCARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "AmqpTypes/AmqpLeader.h"
#include "AmqpTypes/AmqpMessagingType.h"

#ifndef LIBAMQP_AMQP_WILDCARD_T
#define LIBAMQP_AMQP_WILDCARD_T
    typedef struct amqp_wildcard_t amqp_wildcard_t;
#endif


struct amqp_wildcard_t
{
    amqp_leader_t leader;
    amqp_type_t *type;
    amqp_messaging_type_decoder_t message_type_decoder;

    // following members to be initialised by decode function
    amqp_messaging_type_t *messaging_type;
    void (*cleanup_messaging_type)(amqp_context_t *context, amqp_messaging_type_t *messaging_type);
};

extern void amqp_wildcard_initialize_as_null(amqp_context_t *context, amqp_wildcard_t *wildcard);

static inline
int amqp_wildcard_is_null(amqp_wildcard_t *wildcard)
{
    return wildcard->type == 0;
}

extern void amqp_wildcard_initialize(amqp_context_t *context, amqp_wildcard_t *wildcard, amqp_type_t *type, amqp_messaging_type_decoder_t message_type_decoder);

static inline
void amqp_wildcard_cleanup(amqp_context_t *context, amqp_wildcard_t *wildcard)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) wildcard);
}

#ifdef __cplusplus
}
#endif
#endif
