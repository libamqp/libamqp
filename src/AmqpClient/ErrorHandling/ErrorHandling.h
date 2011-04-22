#ifndef STORMMQ_AMQP_ERROR_HANDLING_H
#define STORMMQ_AMQP_ERROR_HANDLING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdio.h>

#ifndef AMQP_CONTEXT_T_TYPE
#define AMQP_CONTEXT_T_TYPE
typedef struct amqp_context_t amqp_context_t;
#endif

#define AMQP_ERROR_CODE_BASE    0x100

#define AMQP_ERROR_BUFFER_OVERRUN               (AMQP_ERROR_CODE_BASE + 0)
#define AMQP_ERROR_BUFFER_INDEX_OUT_OF_BOUNDS   (AMQP_ERROR_CODE_BASE + 1)
#define AMQP_ERROR_BUFFER_WOULD_OVERRUN         (AMQP_ERROR_CODE_BASE + 2)
#define AMQP_ERROR_UNKNOWN_FORMAT_CODE          (AMQP_ERROR_CODE_BASE + 3)
#define AMQP_ERROR_UNSUPPORTED_FORMAT_CODE      (AMQP_ERROR_CODE_BASE + 4)
#define AMQP_ERROR_BUFFER_POSITION_OUT_OF_BOUNDS    (AMQP_ERROR_CODE_BASE + 5)
#define AMQP_ERROR_INVALID_SYMBOL_CHARACTER     (AMQP_ERROR_CODE_BASE + 6)
#define AMQP_ERROR_INVALID_UTF8_CHARACTER       (AMQP_ERROR_CODE_BASE + 7)
#define AMQP_ERROR_INVALID_UTF16_CHARACTER      (AMQP_ERROR_CODE_BASE + 8)
#define AMQP_ERROR_INVALID_UTF32_CHARACTER      (AMQP_ERROR_CODE_BASE + 9)
#define AMQP_ERROR_VARIABLE_TYPE_MALFORMED      (AMQP_ERROR_CODE_BASE + 10)
#define AMQP_ERROR_INVALID_FRAME_HEADER         (AMQP_ERROR_CODE_BASE + 0x20)
#define AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT (AMQP_ERROR_CODE_BASE + 0x21)
#define AMQP_ERROR_ARRAY_ELEMENT_TYPE_MISMATCH  (AMQP_ERROR_CODE_BASE + 0x22)
#define AMQP_ERROR_DESCRIBED_TYPE_OVERFLOW      (AMQP_ERROR_CODE_BASE + 0x23)


#define AMQP_ERROR_NO_DESCRIPTOR                (AMQP_ERROR_CODE_BASE + 0x30)
#define AMQP_ERROR_NO_DESCRIBED_TYPE            (AMQP_ERROR_CODE_BASE + 0x31)
#define AMQP_ERROR_DESCRIPTOR_INVALID           (AMQP_ERROR_CODE_BASE + 0x32)
#define AMQP_ERROR_DESCRIBED_INVALID            (AMQP_ERROR_CODE_BASE + 0x33)

#define AMQP_ERROR_INVALID_FRAME_SIZE           (AMQP_ERROR_CODE_BASE + 0x33)
#define AMQP_ERROR_INCOMPLETE_WRITE             (AMQP_ERROR_CODE_BASE + 0x34)

#define AMQP_ERROR_INCOMPLETE_FRAME_HEADER      (AMQP_ERROR_CODE_BASE + 0x35)
#define AMQP_ERROR_INCOMPLETE_FRAME             (AMQP_ERROR_CODE_BASE + 0x36)

#define amqp_error(context, code, ...) _amqp_error(context, 1, __FILE__, __LINE__, #code, code, "" __VA_ARGS__);
extern void _amqp_error(amqp_context_t *context, int level, const char * filename, int line_number, const char *error_mnemonic, int error_code, const char *format, ...);

#define amqp_debug(context, ...) _amqp_debug(context, 2, __FILE__, __LINE__, __func__, "" __VA_ARGS__);
#define amqp_debug_level(context, level, ...) _amqp_debug(context, level, __FILE__, __LINE__, __func__, "" __VA_ARGS__);
extern void _amqp_debug(const amqp_context_t *context, int level, const char * filename, int line_number, const char *function, const char *format, ...);

#define amqp_io_error(context, ...) _amqp_io_error(context, 1, __FILE__, __LINE__, "" __VA_ARGS__);
extern void _amqp_io_error(amqp_context_t *context, int level, const char * filename, int line_number, const char *format, ...);

#define TODO(what)  (0)
#define _TODO(what)  assert(TODO(what))
#define unsupported_operation(what)  _TODO(unsupported_operation_ ## what)
#define not_implemented(what)  _TODO(not_implemented_ ## what)

#define SOUTC(v)    printf("%s:%d: soutc: %s = %c\n" , __FILE__, __LINE__, #v, v);
#define SOUTV(v)    printf("%s:%d: soutv: %s = %ld\n" , __FILE__, __LINE__, #v, (long) v);
#define SOUTVP(v)   printf("%s:%d: soutvp: %s = %p\n" , __FILE__, __LINE__, #v, v);
#define SOUTVX(v)   printf("%s:%d: soutvx: %s = %lx\n" , __FILE__, __LINE__, #v, (long) v);
#define SOUTS(v)    printf("%s:%d: souts: %s\n", __FILE__, __LINE__, v);

#define fatal_program_error()   abort()

#ifdef __cplusplus
}
#endif
#endif
