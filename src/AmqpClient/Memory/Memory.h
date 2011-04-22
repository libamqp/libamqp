#ifndef STORMMQ_AMQP_MEMORY_H
#define STORMMQ_AMQP_MEMORY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "Memory/Pool.h"

#ifdef TRACE_ALLOCATIONS
#define TRACE_ARGS    , __FILE__, __LINE__
#define TRACE_PARAMS    , const char *fileName, int lineNumber
#else
#define TRACE_ARGS
#define TRACE_PARAMS
#endif

#define AMQP_POOL_REF(context, type)    (&context->pools.type ## _pool)

#define AMQP_ALLOC(type, ...)        amqp_alloc_ ## type(__VA_ARGS__)
#define AMQP_DEALLOC(type, context, p)   (amqp_dealloc_ ## type((context), (p)), (p) = (type *) 0)

#define AMQP_NEW(type)              amqp_new_ ## type()
#define AMQP_DELETE(type, p)   	    {amqp_delete_ ## type(p); (p) = (type *) 0;}

#define AMQP_MALLOC(type)     	    ((type *) amqp_malloc(sizeof(type) TRACE_ARGS))
#define AMQP_FREE(p)                (amqp_free(p TRACE_ARGS), p = 0)

extern void *amqp_malloc(size_t n TRACE_PARAMS);
extern void *amqp_realloc(void *p, size_t n TRACE_PARAMS);
extern void amqp_free(void *p TRACE_PARAMS);

extern void amqp_reset_malloc_allocation_stats();
extern amqp_allocation_stats_t amqp_malloc_stats;
extern volatile int amqp_memory_trace_enabled;

#ifdef __cplusplus
}
#endif
#endif
