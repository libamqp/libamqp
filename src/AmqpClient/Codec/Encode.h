#ifndef STORMMQ_AMQP_ENCODE_H
#define STORMMQ_AMQP_ENCODE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include "amqp.h"
#include "Type/Type.h"
#include "Codec/Convert.h"

#include "types/binary/binary.h"

#ifndef AMQP_CONTEXT_T_TYPE
#define AMQP_CONTEXT_T_TYPE
typedef struct amqp_context_t amqp_context_t;
#endif

amqp_type_t *amqp_encode_null(amqp_context_t *context);
amqp_type_t *amqp_encode_boolean(amqp_context_t *context, int value);

amqp_type_t *amqp_encode_ubyte(amqp_context_t *context, uint8_t value);
amqp_type_t *amqp_encode_byte(amqp_context_t *context, int8_t value);

amqp_type_t *amqp_encode_ushort(amqp_context_t *context, uint16_t value);
amqp_type_t *amqp_encode_short(amqp_context_t *context, int16_t value);

amqp_type_t *amqp_encode_uint(amqp_context_t *context, uint32_t value);
amqp_type_t *amqp_encode_int(amqp_context_t *context, int32_t value);

amqp_type_t *amqp_encode_small_uint(amqp_context_t *context, uint32_t value);
amqp_type_t *amqp_encode_small_int(amqp_context_t *context, int32_t value);

amqp_type_t *amqp_encode_float(amqp_context_t *context, float value);
amqp_type_t *amqp_encode_char(amqp_context_t *context, wchar_t value);

amqp_type_t *amqp_encode_decimal32(amqp_context_t *context, amqp_decimal32_t value);
amqp_type_t *amqp_encode_decimal64(amqp_context_t *context, amqp_decimal64_t value);

amqp_type_t *amqp_encode_ulong(amqp_context_t *context, uint64_t value);
amqp_type_t *amqp_encode_long(amqp_context_t *context, int64_t value);
amqp_type_t *amqp_encode_small_ulong(amqp_context_t *context, uint64_t value);
amqp_type_t *amqp_encode_small_long(amqp_context_t *context, int64_t value);

amqp_type_t *amqp_encode_double(amqp_context_t *context, double value);
amqp_type_t *amqp_encode_timestamp(amqp_context_t *context, amqp_timestamp_t value);

amqp_type_t *amqp_encode_uuid(amqp_context_t *context, amqp_uuid_t *value);

amqp_type_t *amqp_encode_binary_vbin8(amqp_context_t *context, const unsigned char *value, size_t size);
amqp_type_t *amqp_encode_binary_vbin32(amqp_context_t *context, const unsigned char *value, size_t size);
amqp_type_t *amqp_encode_binary(amqp_context_t *context, const unsigned char *value, size_t size);

amqp_type_t *amqp_encode_symbol(amqp_context_t *context, const char *value);
amqp_type_t *amqp_encode_symbol_sym8(amqp_context_t *context, const char *value);
amqp_type_t *amqp_encode_symbol_sym32(amqp_context_t *context, const char *value);
amqp_type_t *amqp_encode_symboln(amqp_context_t *context, const char *value, size_t size);
amqp_type_t *amqp_encode_symbol_sym8n(amqp_context_t *context, const char *value, size_t size);
amqp_type_t *amqp_encode_symbol_sym32n(amqp_context_t *context, const char *value, size_t size);

amqp_type_t *amqp_encode_string_utf8(amqp_context_t *context, const char *value);
amqp_type_t *amqp_encode_string_utf8n(amqp_context_t *context, const char *value, size_t size);

amqp_type_t *amqp_encode_string_utf16(amqp_context_t *context);
amqp_type_t *amqp_encode_string_utf16n(amqp_context_t *context, size_t size);

amqp_type_t *amqp_encode_list_8(amqp_context_t *context);
amqp_type_t *amqp_encode_list_32(amqp_context_t *context);

amqp_type_t *amqp_encode_map_8(amqp_context_t *context);
amqp_type_t *amqp_encode_map_32(amqp_context_t *context);

amqp_type_t *amqp_encode_array_8(amqp_context_t *context);
amqp_type_t *amqp_encode_array_32(amqp_context_t *context);

amqp_type_t *amqp_complete_type(amqp_context_t *context, amqp_type_t *type);
amqp_type_t *amqp_start_encode_described_type(amqp_context_t *context);


#ifdef __cplusplus
}
#endif
#endif


