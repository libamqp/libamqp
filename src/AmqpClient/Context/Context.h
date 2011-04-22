#ifndef STORMMQ_AMQP_CONTEXT_H
#define STORMMQ_AMQP_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "Memory/Pool.h"
#include "Memory/Memory.h"
#include "ErrorHandling/ErrorHandling.h"

#ifndef AMQP_TYPE_T
#define AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

#ifndef AMQP_BUFFER_T_TYPE
#define AMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef AMQP_CONTEXT_T_TYPE
#define AMQP_CONTEXT_T_TYPE
typedef struct amqp_context_t amqp_context_t;
#endif

typedef int amqp_debug_print_c_t(int c);

struct amqp_context_t
{
    int error_code;
    struct
    {
        amqp_debug_print_c_t *putc;
        int formatted_print;
        int use_name_for_composite_type;
    } config;
    struct
    {
        FILE *stream;
        int level;
    } debug;
    struct
    {
        amqp_memory_pool_t amqp_buffer_t_pool;
        amqp_memory_pool_t amqp_type_t_pool;
        amqp_memory_pool_t amqp_frame_t_pool;
    } pools;
    struct {
        amqp_buffer_t *buffer;
    } decode;
    struct
    {
        amqp_buffer_t *buffer;
        amqp_type_t *container;
    } encode;
    amqp_buffer_t *convert_buffer;
};

extern amqp_context_t *amqp_new_amqp_context_t();
extern void amqp_delete_amqp_context_t(amqp_context_t *context);

extern int amqp_context_default_debug_putc(int c);
extern amqp_debug_print_c_t *amqp_context_define_putc_function(amqp_context_t *context, amqp_debug_print_c_t *putc);
extern int amqp_context_putc(amqp_context_t *context, int c);

#ifdef __cplusplus
}
#endif
#endif
