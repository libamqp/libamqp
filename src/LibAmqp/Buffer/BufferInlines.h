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

#ifndef LIBAMQP_BUFFER_INLINES_H
#define LIBAMQP_BUFFER_INLINES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "libamqp_common.h"

#ifndef LIBAMQP_DISABLE_BUFFER_INLINE
#define AMQP_BUFFER_INLINE_DECL inline static
#define AMQP_BUFFER_INLINE_DEFN inline static
#define AMQP_BUFFER_ALWAYS_INLINE  __attribute__((always_inline))
#else
#ifndef AMQP_BUFFER_INLINE_DECL
#define AMQP_BUFFER_INLINE_DECL extern
#endif
#define AMQP_BUFFER_ALWAYS_INLINE
#endif /* LIBAMQP_DISABLE_BUFFER_INLINE */

#ifdef AMQP_BUFFER_INLINE_DECL

AMQP_BUFFER_INLINE_DECL
size_t amqp_buffer_capacity(amqp_buffer_t *buffer);

AMQP_BUFFER_INLINE_DECL
int amqp_buffer_available(amqp_buffer_t *buffer);

AMQP_BUFFER_INLINE_DECL
int amqp_buffer_check_available(amqp_buffer_t *buffer, size_t desired);

AMQP_BUFFER_INLINE_DECL
int amqp_buffer_check_free_space(amqp_buffer_t *buffer, size_t needed);

AMQP_BUFFER_INLINE_DECL
int amqp_unchecked_putc_at(amqp_buffer_t *buffer, int c, size_t index);

AMQP_BUFFER_INLINE_DECL
int amqp_unchecked_putc(amqp_buffer_t *buffer, int c);

AMQP_BUFFER_INLINE_DECL
int amqp_buffer_putc(amqp_buffer_t *buffer, int c);

AMQP_BUFFER_INLINE_DECL
void amqp_buffer_advance_read_index(amqp_buffer_t *buffer, size_t amount);
AMQP_BUFFER_INLINE_DECL
void amqp_buffer_advance_write_point(amqp_buffer_t *buffer, size_t amount);
AMQP_BUFFER_INLINE_DECL
size_t amqp_buffer_set_write_point(amqp_buffer_t *buffer, size_t new_value);

AMQP_BUFFER_INLINE_DECL
size_t amqp_buffer_index(amqp_buffer_t *buffer);

AMQP_BUFFER_INLINE_DECL
size_t amqp_buffer_size(amqp_buffer_t *buffer);

AMQP_BUFFER_INLINE_DECL
void amqp_buffer_set_write_index(amqp_buffer_t *buffer, size_t value);

AMQP_BUFFER_INLINE_DECL
size_t amqp_buffer_write_point(amqp_buffer_t *buffer);

// at end check is relative to the current limit
AMQP_BUFFER_INLINE_DECL
int amqp_buffer_at_end(amqp_buffer_t *buffer);

AMQP_BUFFER_INLINE_DECL
void amqp_buffer_reset_index(amqp_buffer_t *buffer);

AMQP_BUFFER_INLINE_DECL
void amqp_buffer_reset(amqp_buffer_t *buffer);

AMQP_BUFFER_INLINE_DECL
int amqp_buffer_unchecked_getc(amqp_buffer_t *buffer);

// getc is checked against the current limit
AMQP_BUFFER_INLINE_DECL
int amqp_buffer_getc(amqp_buffer_t *buffer);

// Unchecked get is relative to the front of the buffer, not the current limit.
AMQP_BUFFER_INLINE_DECL
int amqp_unchecked_getc_at(amqp_buffer_t *buffer, size_t index);

AMQP_BUFFER_INLINE_DECL
amqp_one_byte_t amqp_ntoh_8(amqp_buffer_t *buffer, size_t offset) AMQP_BUFFER_ALWAYS_INLINE;
AMQP_BUFFER_INLINE_DECL
amqp_two_byte_t amqp_ntoh_16(amqp_buffer_t *buffer, size_t offset) AMQP_BUFFER_ALWAYS_INLINE;
AMQP_BUFFER_INLINE_DECL
amqp_four_byte_t amqp_ntoh_32(amqp_buffer_t *buffer, size_t offset) AMQP_BUFFER_ALWAYS_INLINE;
AMQP_BUFFER_INLINE_DECL
void amqp_ntoh_64(amqp_eight_byte_t *value, amqp_buffer_t *buffer, size_t offset);

AMQP_BUFFER_INLINE_DECL
void amqp_hton_16(amqp_buffer_t *buffer, amqp_two_byte_t value) AMQP_BUFFER_ALWAYS_INLINE;
AMQP_BUFFER_INLINE_DECL
void amqp_hton_32(amqp_buffer_t *buffer, amqp_four_byte_t value) AMQP_BUFFER_ALWAYS_INLINE;
AMQP_BUFFER_INLINE_DECL
void amqp_hton_64(amqp_buffer_t *buffer, amqp_eight_byte_t value) AMQP_BUFFER_ALWAYS_INLINE;


/*
The next three functions are intended for use by the frame header decoding and
assume that at least 8 bytes are in the buffer.
*/
AMQP_BUFFER_INLINE_DECL
uint32_t amqp_buffer_read_uint32(amqp_buffer_t *buffer, size_t offset);
AMQP_BUFFER_INLINE_DECL
uint16_t amqp_buffer_read_uint16(amqp_buffer_t *buffer, size_t offset);
AMQP_BUFFER_INLINE_DECL
uint8_t amqp_buffer_read_uint8(amqp_buffer_t *buffer, size_t offset);

/*
Write functions intended for use by the frame header writers
The assume they are writing to the start of a buffer and do no bounds checking.
*/
AMQP_BUFFER_INLINE_DECL void amqp_buffer_write_uint32(amqp_buffer_t *buffer, uint32_t value);
AMQP_BUFFER_INLINE_DECL void amqp_buffer_write_uint16(amqp_buffer_t *buffer, uint16_t value);
AMQP_BUFFER_INLINE_DECL void amqp_buffer_write_uint8(amqp_buffer_t *buffer, uint8_t value);

AMQP_BUFFER_INLINE_DECL int to_hex(int c);

AMQP_BUFFER_INLINE_DECL
const unsigned char *amqp_buffer_pointer(amqp_buffer_t *buffer, size_t index);

// TODO - introduced because of a transport test - move test code into buffer
AMQP_BUFFER_INLINE_DECL
unsigned char *amqp_buffer_write_pointer(amqp_buffer_t *buffer);

#endif /* AMQP_BUFFER_INLINE_DECL */

#ifdef AMQP_BUFFER_INLINE_DEFN
#include "Buffer/BufferInternal.h"

AMQP_BUFFER_INLINE_DEFN
size_t amqp_buffer_capacity(amqp_buffer_t *buffer)
{
    return buffer->capacity;
}

// TODO - rename to amqp_buffer_available_for_reading
AMQP_BUFFER_INLINE_DEFN
int amqp_buffer_available(amqp_buffer_t *buffer)
{
    return buffer->limit.size - buffer->limit.index;
}

AMQP_BUFFER_INLINE_DEFN
int amqp_buffer_check_available(amqp_buffer_t *buffer, size_t desired)
{
    // TODO - can simplify this
    return buffer->limit.index < buffer->limit.size && (buffer->limit.size - buffer->limit.index) >= desired;
}

// check on available space is relative to the current limit.
AMQP_BUFFER_INLINE_DEFN
int amqp_buffer_check_free_space(amqp_buffer_t *buffer, size_t needed)
{
    return ((buffer->limit.size + needed) <= buffer->capacity);
}

AMQP_BUFFER_INLINE_DEFN
int amqp_unchecked_putc_at(amqp_buffer_t *buffer, int c, size_t index)
{
    size_t block = index >> AMQP_FRAGMENT_INDEX_BITS;
    size_t offset = index & AMQP_FRAGMENT_INDEX_MASK;

    return buffer->fragments[block][offset] = c;
}

AMQP_BUFFER_INLINE_DEFN
int amqp_unchecked_putc(amqp_buffer_t *buffer, int c)
{
    return amqp_unchecked_putc_at(buffer, c, buffer->limit.size++);
}

AMQP_BUFFER_INLINE_DEFN
int amqp_buffer_putc(amqp_buffer_t *buffer, int c)
{
    return (buffer->limit.size < buffer->capacity) ? amqp_unchecked_putc(buffer, c) : -1;
}

AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_advance_read_index(amqp_buffer_t *buffer, size_t amount)
{
    buffer->limit.index += amount;
}

AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_advance_write_point(amqp_buffer_t *buffer, size_t amount)
{
    buffer->limit.size += amount;
}

AMQP_BUFFER_INLINE_DEFN
size_t amqp_buffer_set_write_point(amqp_buffer_t *buffer, size_t new_value)
{
    size_t old_value = buffer->limit.size;
    buffer->limit.size = new_value;
    return old_value;
}

AMQP_BUFFER_INLINE_DEFN
size_t amqp_buffer_index(amqp_buffer_t *buffer)
{
    return buffer->limit.index;
}

AMQP_BUFFER_INLINE_DEFN
size_t amqp_buffer_size(amqp_buffer_t *buffer)
{
    return buffer->limit.size;
}

// TODO - rename
AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_set_write_index(amqp_buffer_t *buffer, size_t value)
{
    buffer->limit.size = value;
}

AMQP_BUFFER_INLINE_DEFN
size_t amqp_buffer_write_point(amqp_buffer_t *buffer)
{
    return amqp_buffer_size(buffer);
}

// at end check is relative to the current limit
AMQP_BUFFER_INLINE_DEFN
int amqp_buffer_at_end(amqp_buffer_t *buffer)
{
    return buffer->limit.index == buffer->limit.size;
}

AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_reset_index(amqp_buffer_t *buffer)
{
    buffer->limit.index = 0;
}

AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_reset(amqp_buffer_t *buffer)
{
    buffer->limit.index = buffer->limit.size = 0;
}

// Unchecked get is relative to the front of the buffer, not the current limit.
AMQP_BUFFER_INLINE_DEFN
int amqp_unchecked_getc_at(amqp_buffer_t *buffer, size_t index)
{
    size_t block = index >> AMQP_FRAGMENT_INDEX_BITS;
    size_t offset = index & AMQP_FRAGMENT_INDEX_MASK;

    return buffer->fragments[block][offset];
}

AMQP_BUFFER_INLINE_DEFN
int amqp_buffer_unchecked_getc(amqp_buffer_t *buffer)
{
    return amqp_unchecked_getc_at(buffer, buffer->limit.index++);
}

// getc is checked against the current limit
AMQP_BUFFER_INLINE_DEFN
int amqp_buffer_getc(amqp_buffer_t *buffer)
{
    return buffer->limit.index < buffer->limit.size ? amqp_buffer_unchecked_getc(buffer) : -1;
}

#ifdef WORDS_BIGENDIAN
#error BIG ENDIAN not supported
#else

AMQP_BUFFER_INLINE_DEFN
amqp_one_byte_t amqp_ntoh_8(amqp_buffer_t *buffer, size_t offset)
{
    amqp_one_byte_t result;
    result._unsigned = amqp_unchecked_getc_at(buffer, offset);
    return result;
}

AMQP_BUFFER_INLINE_DEFN
amqp_two_byte_t amqp_ntoh_16(amqp_buffer_t *buffer, size_t offset)
{
    amqp_two_byte_t result;
    result.b[1] = amqp_unchecked_getc_at(buffer, offset++);
    result.b[0] = amqp_unchecked_getc_at(buffer, offset);
    return result;
}

AMQP_BUFFER_INLINE_DEFN
amqp_four_byte_t amqp_ntoh_32(amqp_buffer_t *buffer, size_t offset)
{
    amqp_four_byte_t result;
    result.b[3] = amqp_unchecked_getc_at(buffer, offset++);
    result.b[2] = amqp_unchecked_getc_at(buffer, offset++);
    result.b[1] = amqp_unchecked_getc_at(buffer, offset++);
    result.b[0] = amqp_unchecked_getc_at(buffer, offset);
    return result;
}

AMQP_BUFFER_INLINE_DEFN
void amqp_ntoh_64(amqp_eight_byte_t *value, amqp_buffer_t *buffer, size_t offset)
{
    value->b[7] = amqp_unchecked_getc_at(buffer, offset++);
    value->b[6] = amqp_unchecked_getc_at(buffer, offset++);
    value->b[5] = amqp_unchecked_getc_at(buffer, offset++);
    value->b[4] = amqp_unchecked_getc_at(buffer, offset++);
    value->b[3] = amqp_unchecked_getc_at(buffer, offset++);
    value->b[2] = amqp_unchecked_getc_at(buffer, offset++);
    value->b[1] = amqp_unchecked_getc_at(buffer, offset++);
    value->b[0] = amqp_unchecked_getc_at(buffer, offset);
}

AMQP_BUFFER_INLINE_DEFN
void amqp_hton_16(amqp_buffer_t *buffer, amqp_two_byte_t value)
{
    amqp_unchecked_putc(buffer, value.b[1]);
    amqp_unchecked_putc(buffer, value.b[0]);
}

AMQP_BUFFER_INLINE_DEFN
void amqp_hton_32(amqp_buffer_t *buffer, amqp_four_byte_t value)
{
    amqp_unchecked_putc(buffer, value.b[3]);
    amqp_unchecked_putc(buffer, value.b[2]);
    amqp_unchecked_putc(buffer, value.b[1]);
    amqp_unchecked_putc(buffer, value.b[0]);
}

AMQP_BUFFER_INLINE_DEFN
void amqp_hton_64(amqp_buffer_t *buffer, amqp_eight_byte_t value)
{
    amqp_unchecked_putc(buffer, value.b[7]);
    amqp_unchecked_putc(buffer, value.b[6]);
    amqp_unchecked_putc(buffer, value.b[5]);
    amqp_unchecked_putc(buffer, value.b[4]);
    amqp_unchecked_putc(buffer, value.b[3]);
    amqp_unchecked_putc(buffer, value.b[2]);
    amqp_unchecked_putc(buffer, value.b[1]);
    amqp_unchecked_putc(buffer, value.b[0]);
}
#endif

/*
The next three functions are intended for use by the frame header decoding and
assume that at least 8 bytes are in the buffer.
*/
AMQP_BUFFER_INLINE_DEFN
uint32_t amqp_buffer_read_uint32(amqp_buffer_t *buffer, size_t offset)
{
    return amqp_ntoh_32(buffer, offset)._uint;
}

AMQP_BUFFER_INLINE_DEFN
uint16_t amqp_buffer_read_uint16(amqp_buffer_t *buffer, size_t offset)
{
    return amqp_ntoh_16(buffer, offset)._ushort;
}

AMQP_BUFFER_INLINE_DEFN
uint8_t amqp_buffer_read_uint8(amqp_buffer_t *buffer, size_t offset)
{
    return amqp_unchecked_getc_at(buffer, offset);
}

/*
Write functions intended for use by the frame header writers
The assume they are writing to the start of a buffer and do no bounds checking.
*/
AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_write_uint32(amqp_buffer_t *buffer, uint32_t value)
{
    amqp_four_byte_t v;
    v._uint = value;
    amqp_hton_32(buffer, v);
}

AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_write_uint16(amqp_buffer_t *buffer, uint16_t value)
{
    amqp_two_byte_t v;
    v._ushort = value;
    amqp_hton_16(buffer, v);
}

AMQP_BUFFER_INLINE_DEFN
void amqp_buffer_write_uint8(amqp_buffer_t *buffer, uint8_t value)
{
    amqp_unchecked_putc(buffer, value);
}

AMQP_BUFFER_INLINE_DEFN
int to_hex(int c)
{
    return c + (c < 10 ? '0' : ('a' - 10));
}

AMQP_BUFFER_INLINE_DEFN
const unsigned char *amqp_buffer_pointer(amqp_buffer_t *buffer, size_t index)
{
    size_t block = index >> AMQP_FRAGMENT_INDEX_BITS;
    size_t offset = index & AMQP_FRAGMENT_INDEX_MASK;
    return &buffer->fragments[block][offset];
}

AMQP_BUFFER_INLINE_DEFN
unsigned char *amqp_buffer_write_pointer(amqp_buffer_t *buffer)
{
    return (unsigned char *) amqp_buffer_pointer(buffer, buffer->limit.size);
}

#endif /* AMQP_BUFFER_INLINE_DEFN */
#ifdef __cplusplus
}
#endif
#endif
