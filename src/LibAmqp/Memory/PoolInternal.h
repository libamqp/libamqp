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

#ifndef LIBAMQP_MEMORY_POOL_INTERNAL_H
#define LIBAMQP_MEMORY_POOL_INTERNAL_H

#include "Memory/Pool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct amqp_memory_allocation_t amqp_memory_allocation_t;

typedef struct amqp_memory_allocation_header_t
{
    amqp_memory_block_t *block;
    int index;
} amqp_memory_allocation_header_t;

struct amqp_memory_allocation_t
{
    amqp_memory_allocation_header_t header;
    unsigned char data[];
};

typedef struct amqp_memory_block_header_t
{
    amqp_memory_block_t *next;
    amqp_memory_block_t *previous;
    union
    {
        unsigned long bits;
        unsigned char bytes[LONG_BIT/8];
    } mask;
} amqp_memory_block_header_t;

struct amqp_memory_block_t
{
    amqp_memory_block_header_t header;
    unsigned char allocations_data[];      /* amqp_memory_allocation_t data[LONG_BIT] */
};


#ifdef __cplusplus
}
#endif
#endif
