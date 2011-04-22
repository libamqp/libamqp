#ifndef STORMMQ_AMQP_ENCODE_FRAMING_TYPES_H
#define STORMMQ_AMQP_ENCODE_FRAMING_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Codec/Encode.h"
#include "Type/TypeEncodeMetaData.h"

static inline
amqp_type_t *amqp_encode_seconds(amqp_context_t *context, uint32_t seconds)
{
    return amqp_encode_uint(context, seconds);
}

amqp_type_t *amqp_encode_multiple_true_va(amqp_context_t *context, amqp_type_encode_meta_data_t *mata_data, ...);
#define amqp_encode_multiple_true(c, ...) amqp_encode_multiple_true_va(c, __VA_ARGS__, 0);

#ifdef __cplusplus
}
#endif
#endif


