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

#ifndef LIBAMQP_COMMON_H
#define LIBAMQP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#define null    0
#else
#define bool    int
#define true    1
#define false   0
#define null    ((void *) 0)
#endif

/* If we're not using GNU C, elide __attribute__ */
#ifndef __GNUC__
#define __attribute__(x)  /* NOTHING */
#endif

#ifdef _MSC_VER
#include "libamqp_win32.h"
#define not_reached()   abort(); return 0;
#else
#include <stdint.h>
#define not_reached()   abort();
#endif

#include <wchar.h>

#ifndef LIBAMQP_AMQP_TYPE_T
#define LIBAMQP_AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

typedef uint32_t amqp_decimal32_t;
typedef uint64_t amqp_decimal64_t;
typedef int64_t amqp_timestamp_t;
typedef uint8_t amqp_uuid_t[16];

typedef union amqp_four_byte_t
{
    unsigned char b[4];
    uint32_t _uint;
    int32_t _int;
    float _float;
    wchar_t _wchar;
} amqp_four_byte_t;

typedef union amqp_eight_byte_t
{
    unsigned char b[8];
    uint64_t _ulong;
    int64_t _long;
    double _double;
    amqp_timestamp_t _timestamp;
} amqp_eight_byte_t;

typedef union amqp_two_byte_t
{
    unsigned char b[2];
    uint16_t _ushort;
    int16_t _short;
} amqp_two_byte_t;

/* TODO - review - This is kind of silly but it's use helps prevent sign bit extension errors */
typedef union amqp_one_byte_t
{
    unsigned char b[1];
    uint8_t _unsigned;
    int8_t _signed;
} amqp_one_byte_t;

#ifdef __cplusplus
}
#endif
#endif
