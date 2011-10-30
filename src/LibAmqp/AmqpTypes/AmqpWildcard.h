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
#include "AmqpTypes/AmqpHashTable.h"
#include "AmqpTypes/AmqpVariable.h"

#ifndef LIBAMQP_AMQP_WILDCARD_T
#define LIBAMQP_AMQP_WILDCARD_T
    typedef struct amqp_wildcard_t amqp_wildcard_t;
#endif

struct amqp_wildcard_t
{
    int ph;
};


#ifdef __cplusplus
}
#endif
#endif
