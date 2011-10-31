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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_MESSAGING_TYPE_H
#define LIBAMQP_AMQP_TYPES_AMQP_MESSAGING_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "AmqpTypes/AmqpLeader.h"

typedef int (*amqp_messaging_type_decoder_t)(amqp_context_t *context, amqp_wildcard_t *wildcard);

struct amqp_messaging_type_t
{
    amqp_leader_t leader;
};

extern void amqp_messaging_type_cleanup(amqp_context_t *context, amqp_messaging_type_t *messaging_type);

#ifdef __cplusplus
}
#endif
#endif
