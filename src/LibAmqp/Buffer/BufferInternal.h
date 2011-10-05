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

#ifndef LIBAMQP_BUFFER_INTERNAL_H
#define LIBAMQP_BUFFER_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include "libamqp_common.h"

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

typedef struct amqp_buffer_limit_t
{
    size_t index;
    size_t size;
} amqp_buffer_limit_t;

struct amqp_buffer_t
{
    size_t capacity;
    amqp_buffer_limit_t limit;
    size_t reference_count;
    unsigned char *fragments[AMQP_BUFFER_FRAGMENTS];
#ifndef _MSC_VER
    struct iovec io_vectors[AMQP_BUFFER_FRAGMENTS];
#endif
};

#ifdef __cplusplus
}
#endif
#endif
