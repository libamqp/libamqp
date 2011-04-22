#ifndef STORMMQ_AMQP_CONVERT_H
#define STORMMQ_AMQP_CONVERT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Buffer/Buffer.h"
#include "Type/Type.h"

extern char *amqp_convert_bytes_to_cstr(amqp_type_t *);

inline static int amqp_convert_bytes_to_boolean(amqp_type_t *type)
{
    return type->format_code == 0x41;
}

//inline static uint64_t amqp_convert_bytes_to_ulong(amqp_type_t *type)
//{
//    return amqp_ntoh_64(type->context->convert_buffer, type->position.index)._ulong;
//}

//inline static int64_t amqp_convert_bytes_to_long(amqp_type_t *type)
//{
//    return amqp_ntoh_64(type->context->convert_buffer, type->position.index)._long;
//}

//inline static uint32_t amqp_convert_bytes_to_uint(amqp_type_t *type)
//{
//    return amqp_ntoh_32(type->context->convert_buffer, type->position.index)._uint;
//}

//inline static int32_t amqp_convert_bytes_to_int(amqp_type_t *type)
//{
//    return amqp_ntoh_32(type->context->convert_buffer, type->position.index)._int;
//}

//inline static uint64_t amqp_convert_bytes_to_small_ulong(amqp_type_t *type)
//{
//    return (uint64_t) amqp_ntoh_8(type->context->convert_buffer, type->position.index)._unsigned;
//}

//inline static int64_t amqp_convert_bytes_to_small_long(amqp_type_t *type)
//{
//    return (int64_t) amqp_ntoh_8(type->context->convert_buffer, type->position.index)._signed;
//}

//inline static uint32_t amqp_convert_bytes_to_small_uint(amqp_type_t *type)
//{
//    return (uint32_t) amqp_ntoh_8(type->context->convert_buffer, type->position.index)._unsigned;
//}

//inline static int32_t amqp_convert_bytes_to_small_int(amqp_type_t *type)
//{
//    return (int32_t)  amqp_ntoh_8(type->context->convert_buffer, type->position.index)._signed;
//}

//inline static uint16_t amqp_convert_bytes_to_ushort(amqp_type_t *type)
//{
//    return amqp_ntoh_16(type->context->convert_buffer, type->position.index)._ushort;
//}

//inline static int16_t amqp_convert_bytes_to_short(amqp_type_t *type)
//{
//    return amqp_ntoh_16(type->context->convert_buffer, type->position.index)._short;
//}

//inline static uint8_t amqp_convert_bytes_to_ubyte(amqp_type_t *type)
//{
//    return amqp_ntoh_8(type->context->convert_buffer, type->position.index)._unsigned;
//}

//inline static int8_t amqp_convert_bytes_to_byte(amqp_type_t *type)
//{
//    return (int8_t) amqp_ntoh_8(type->context->convert_buffer, type->position.index)._signed;
//}
//
//inline static float amqp_convert_bytes_to_float(amqp_type_t *type)
//{
//    return amqp_ntoh_32(type->context->convert_buffer, type->position.index)._float;
//}

//inline static double amqp_convert_bytes_to_double(amqp_type_t *type)
//{
//    return amqp_ntoh_64(type->context->convert_buffer, type->position.index)._double;
//}

#ifdef __cplusplus
}
#endif
#endif


