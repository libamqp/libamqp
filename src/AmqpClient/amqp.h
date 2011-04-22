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

#ifndef STORMMQ_AMQP_H
#define STORMMQ_AMQP_H

#ifdef __cplusplus
extern "C" {
#define null    0
#else
#define bool    int
#define true    1
#define false   0
#define null    ((void *) 0)
#endif

#include <stdint.h>
#include "amqp_misc.h"

#ifndef AMQP_TYPE_T
#define AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

typedef uint32_t amqp_decimal32_t;
typedef uint64_t amqp_decimal64_t;

typedef int64_t amqp_timestamp_t;
typedef uint8_t amqp_uuid_t[16];

#ifdef __cplusplus
}
#endif
#endif


