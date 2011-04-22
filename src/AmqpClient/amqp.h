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


