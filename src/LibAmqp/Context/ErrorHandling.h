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

#ifndef LIBAMQP_CONTEXT_ERROR_HANDLING_H
#define LIBAMQP_CONTEXT_ERROR_HANDLING_H

#include <assert.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#define AMQP_ERROR_CODE_BASE    0x1000

enum amqp_error_code_t {
    AMQP_ERROR_BUFFER_OVERRUN = AMQP_ERROR_CODE_BASE,
    AMQP_ERROR_BUFFER_INDEX_OUT_OF_BOUNDS,
    AMQP_ERROR_BUFFER_WOULD_OVERRUN,
    AMQP_ERROR_UNKNOWN_FORMAT_CODE,
    AMQP_ERROR_UNSUPPORTED_FORMAT_CODE,
    AMQP_ERROR_BUFFER_POSITION_OUT_OF_BOUNDS,
    AMQP_ERROR_INVALID_SYMBOL_CHARACTER,
    AMQP_ERROR_INVALID_UTF8_CHARACTER,
    AMQP_ERROR_INVALID_UTF32_CHARACTER,
    AMQP_ERROR_VARIABLE_TYPE_MALFORMED,
    AMQP_ERROR_INVALID_FRAME_HEADER,
    AMQP_ERROR_ARRAY_ELEMENT_TYPE_INCORRECT,
    AMQP_ERROR_ARRAY_ELEMENT_TYPE_MISMATCH,
    AMQP_ERROR_DESCRIBED_TYPE_OVERFLOW,
    //TODO -  add test for error,
    AMQP_ERROR_COMPLETE_WRONG_CONTAINER,
    AMQP_ERROR_NO_DESCRIPTOR,
    AMQP_ERROR_NO_DESCRIBED_TYPE,
    AMQP_ERROR_DESCRIPTOR_INVALID,
    AMQP_ERROR_DESCRIBED_INVALID,
    AMQP_ERROR_INVALID_FRAME_SIZE,
    AMQP_ERROR_INCOMPLETE_WRITE,
    AMQP_ERROR_INCOMPLETE_FRAME_HEADER,
    AMQP_ERROR_INCOMPLETE_FRAME,
    AMQP_ERROR_MEMORY_ERROR,
    AMQP_ERROR_ILLEGAL_STATE,
    AMQP_ERROR_NEGOTIATION_FAILED,
    AMQP_ERROR_NO_PROTOCOL,
    AMQP_ERROR_UNKNOWN_PROTOCOL,
    AMQP_ERROR_INVALID_HEADER,
    AMQP_ERROR_UNITIALIZED_ACTION,
    AMQP_ERROR_PARTIAL_READ,
    AMQP_ERROR_CLIENT_DISCONNECT,
    AMQP_ERROR_BROKER_DISCONNECT,
    AMQP_ERROR_WRITE_DISCHARGE_TIMEOUT,
    AMQP_ERROR_INPUT_DRAIN_TIMEOUT,

};


#define amqp_error(context, code, ...) _amqp_error(context, 1, __FILE__, __LINE__, 0, #code, code, "" __VA_ARGS__)
extern void _amqp_error(amqp_context_t *context, int level, const char * filename, int line_number, const char *source, const char *error_mnemonic, int error_code, const char *format, ...);
extern void _vamqp_error(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *error_mnemonic, int error_code, const char *format, va_list args);

#define amqp_debug(context, ...) _amqp_debug(context, 2, __FILE__, __LINE__, 0, __func__, "" __VA_ARGS__)
#define amqp_debug_source(context, source, ...) _amqp_debug(context, 2, __FILE__, __LINE__, source, __func__, "" __VA_ARGS__)
#define amqp_debug_level(context, level, ...) _amqp_debug(context, level, __FILE__, __LINE__, 0, __func__, "" __VA_ARGS__)
extern void _amqp_debug(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *function, const char *format, ...);

#define amqp_trace(context, code, ...) _amqp_trace(context, 2, __FILE__, __LINE__, 0, "" __VA_ARGS__)
extern void _amqp_trace(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *format, ...);
extern void _vamqp_trace(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *format, va_list args);

// TODO - pass in errno
#define amqp_io_error(context, ...) _amqp_io_error(context, 1, __FILE__, __LINE__, 0, -1, "" __VA_ARGS__)
#define amqp_io_error_code(context, ec, ...) _amqp_io_error(context, 1, __FILE__, __LINE__, 0, ec, "" __VA_ARGS__)
extern void _amqp_io_error(amqp_context_t *context, int level, const char * filename, int line_number, const char *source, int error_code, const char *format, ...);

#define _TODO(what)  (0)
#define TODO(what)   {printf("%s:%d: todo - %s\n", __FILE__, __LINE__, #what); abort();}

#define unsupported_operation(what)  TODO(unsupported_operation_ ## what)
#define not_implemented(what)  TODO(not_implemented___ ## what ## ___)
#define illegal_program_state(what)  TODO(illegal_program_state___ ## what ## ___)

extern void amqp_api_usage_error(amqp_context_t *context, const char *message);
extern void amqp_fatal_program_error(const char *message);

#ifdef __cplusplus
}
#endif
#endif
