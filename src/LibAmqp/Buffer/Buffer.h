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

#ifndef LIBAMQP_BUFFER_H
#define LIBAMQP_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "libamqp_common.h"
#include "Memory/Pool.h"

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

/* Danger, Will Robinson. Keep these in sync. */
#define AMQP_BUFFER_MAX_SIZE        ((size_t) (64 * 1024))
#define AMQP_BUFFER_FRAGMENT_SIZE   ((size_t) (4 * 1024))
#define AMQP_BUFFER_FRAGMENTS       (AMQP_BUFFER_MAX_SIZE / AMQP_BUFFER_FRAGMENT_SIZE)
#define AMQP_FRAGMENT_INDEX_MASK    (AMQP_BUFFER_FRAGMENT_SIZE - 1)
#define AMQP_FRAGMENT_INDEX_BITS    12

struct amqp_block_header
{
    size_t capacity;
    size_t fragment_size;
    size_t n_fragments;
};

typedef struct amqp_memory_t
{
    struct amqp_block_header header;
    unsigned char *fragments[];
} amqp_memory_t;

extern void amqp_buffer_initialize_pool(amqp_memory_pool_t *pool);
extern size_t amqp_buffer_grow(amqp_context_t *context, amqp_buffer_t *buffer, size_t new_size);

extern int amqp_buffer_puts(amqp_buffer_t *, const unsigned char *source, size_t n);
extern int amqp_buffer_put_buffer_contents(amqp_buffer_t *desc, amqp_buffer_t *source);

extern uint32_t amqp_buffer_read_size_field(amqp_buffer_t *buffer, size_t width);

extern void amqp_buffer_backup(amqp_buffer_t *buffer, size_t amount);

/* get an iovec for writing into the buffer for use by the readv call */
extern struct iovec *amqp_buffer_write_io_vec(amqp_buffer_t *buffer, size_t n, int *iov_count);

/* get an iovec for reading from the buffer for use by the writev call */
extern struct iovec *amqp_buffer_read_io_vec(amqp_buffer_t *buffer, int *iov_count);

extern void amqp_buffer_reference(amqp_buffer_t *buffer);
extern size_t amqp_buffer_reference_count(amqp_buffer_t *buffer);
extern size_t amqp_buffer_release(amqp_buffer_t *buffer);

static inline
void amqp_block_header_initialize(struct amqp_block_header *header, size_t capacity, size_t fragment_size, size_t n_fragments)
{
    header->capacity = capacity;
    header->fragment_size = fragment_size;
    header->n_fragments = n_fragments;
}
extern int amqp_block_compare(amqp_memory_t *lhs, size_t lhs_offset, amqp_memory_t *rhs, size_t rhs_offset, size_t n);
extern void amqp_dump_buffer(amqp_context_t *context, amqp_buffer_t *buffer, int limit);
extern void amqp_buffer_block_copy(amqp_buffer_t *buffer, unsigned char *destination, size_t start, size_t size);
extern int amqp_buffer_write(int fd, amqp_buffer_t *buffer);

#include "Buffer/BufferInlines.h"

#ifdef __cplusplus
}
#endif
#endif
