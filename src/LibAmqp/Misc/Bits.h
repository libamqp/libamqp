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

#ifndef LIBAMQP_MISC_BITS_H
#define LIBAMQP_MISC_BITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "platform_limits.h"

#define AMQP_BIT_SIZEOF_SIZE_T  (sizeof(size_t) * CHAR_BIT)
#define AMQP_BIT_SIZEOF_VOIDP   (sizeof(void *) * CHAR_BIT)

extern const uint8_t amqp_mod_37_bit_position[];
extern const uint8_t amqp_mod_67_bit_position[];

static inline int amqp_trailing_zeros_64(uint64_t value)
{
    return amqp_mod_67_bit_position[value % 67];
}

static inline int amqp_trailing_zeros_32(uint32_t value)
{
    return amqp_mod_37_bit_position[value % 37];
}

#if defined(__LP__) || defined(_WIN64) || ULONG_MAX > 4294967295ULL
#define amqp_mask_t uint64_t
#define amqp_trailing_zeros amqp_trailing_zeros_64
#define amqp_next_power_two amqp_next_power_two_64
#else
#define amqp_mask_t uint32_t
#define amqp_trailing_zeros amqp_trailing_zeros_32
#define amqp_next_power_two amqp_next_power_two_32
#endif

static inline amqp_mask_t amqp_least_significant_1_bit(amqp_mask_t value)
{
    return value & -value;
}

static inline int amqp_is_power_of_two(amqp_mask_t value)
{
    return (value & (value - 1)) == 0;
}

extern uint32_t amqp_next_power_two_32(uint32_t n);
extern uint64_t amqp_next_power_two_64(uint64_t n);

#ifdef __cplusplus
}
#endif
#endif


