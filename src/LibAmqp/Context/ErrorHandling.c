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

#include "Context/ErrorHandling.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "Context/Context.h"


static const char *shorten_error_mnemonic(const char *mnemonic)
{
    const char *leader = "AMQP_ERROR_";

    if (strncmp(mnemonic, leader, strlen(leader)) != 0)
    {
        return mnemonic;
    }

    while (*leader && *leader == *mnemonic)
    {
        leader++;
        mnemonic++;
    }
    return mnemonic;
}

void _amqp_io_error(amqp_context_t *context, int level, const char * filename, int line_number, const char *source, int error_code, const char *format, ...)
{
    if (error_code == -1)
    {
        error_code = errno;
    }
    context->error_code = error_code;
    
    if (context->debug.outputter && level < context->debug.level)
    {
        char message[256];
        char extra[256];
        va_list args;

        strerror_r(context->error_code, message, sizeof(message));
        snprintf(extra, sizeof(extra), "%s(%d)", message, context->error_code);

        va_start(args, format);
        (*context->debug.outputter)(&context->debug.arg, filename, line_number, context->debug.name, "io error", source, extra, format, args);
        va_end(args);
    }
}

void _vamqp_error(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *error_mnemonic, int error_code, const char *format, va_list args)
{
    context->error_code = error_code;

    if (context->debug.outputter && level < context->debug.level)
    {
        char extra[128];
        snprintf(extra, sizeof(extra), "%s(%d)", shorten_error_mnemonic(error_mnemonic), error_code);

        (*context->debug.outputter)(&context->debug.arg, filename, line_number, context->debug.name, "error", source, extra, format, args);
    }
}

void _amqp_error(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *error_mnemonic, int error_code, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vamqp_error(context, level, filename, line_number, source, error_mnemonic, error_code, format, args);
    va_end(args);
}

void _amqp_debug(amqp_context_t *context, int level, const char * filename, int line_number, const char *source, const char *function, const char *format, ...)
{
    if (context->debug.outputter && level < context->debug.level)
    {
        va_list args;
        va_start(args, format);
        (*context->debug.outputter)(&context->debug.arg, filename, line_number, context->debug.name, "debug", source, function, format, args);
        va_end(args);
    }
}

void _vamqp_trace(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *format, va_list args)
{
    if (context->debug.outputter && level < context->debug.level)
    {
        (*context->debug.outputter)(&context->debug.arg, filename, line_number, context->debug.name, "trace", source, 0, format, args);
    }
}

void _amqp_trace(amqp_context_t *context, int level, const char *filename, int line_number, const char *source, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vamqp_trace(context, level, filename, line_number, source, format, args);
    va_end(args);
}

static void write_to_stderr(const char *buffer, size_t n)
{
    int written;
    while ((n > 0) && ((written = write(2, buffer, n)) >= 0 || errno == EINTR))
    {
        buffer += written;
        n -= written;
    }
}

static void print_error_message(const char *leader, const char *message)
{
    char buffer[128];
    int leader_length = strlen(leader);
    int message_length = strlen(message);
    int limit = sizeof(buffer) - leader_length - 2;
    int actual = message_length > limit ? limit : message_length;

    strncpy(buffer, leader, leader_length);
    strncpy(buffer + leader_length,  message, actual);
    buffer[leader_length + actual] = '\n';
    
    write_to_stderr(buffer, leader_length + actual + 1);
}

void amqp_fatal_program_error(const char *message)
{
    print_error_message("libamqp - fatal - illegal state detected - ", message);
    abort();
    exit(1);
}

void amqp_api_usage_error(amqp_context_t *context, const char *message)
{
    fprintf(stderr, "libamqp - illegal state detected - %s\n", message);
    if (!context->config.continue_on_usage_error)
    {
        abort();
        exit(1);
    }
}
