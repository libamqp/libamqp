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

#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Context/Context.h"


// TODO - lock error output stream
static void output_message(FILE *stream, const char *filename, int line_number, const char *label, const char *extra, const char *format, va_list args)
{
    fprintf(stream, "%s:%d: %s: %s", filename, line_number, label, extra);
    if (format && *format)
    {
        fputc(' ', stream);
        fputc('-', stream);
        fputc(' ', stream);
        vfprintf(stream, format, args);
    }
    fputc('\n', stream);
}

static const char *shorten_error_mnemonic(const char *mnemonic)
{
    const char *leader = "LIBAMQP_ERROR_";

    while (*leader && *leader == *mnemonic)
    {
        leader++;
        mnemonic++;
    }
    return mnemonic;
}

void _amqp_io_error(amqp_context_t *context, int level, const char * filename, int line_number, const char *format, ...)
{
    context->error_code = errno;
    
    if (context->debug.stream && level < context->debug.level)
    {
        char message[256];
        strerror_r(context->error_code, message, sizeof(message));

        char extra[256];
        snprintf(extra, sizeof(extra), "%s(%d)", message, context->error_code);

        va_list args;
        va_start(args, format);
        output_message(context->debug.stream, filename, line_number, "io error", extra, format, args);
        va_end(args);
    }
}

void _amqp_error(amqp_context_t *context, int level, const char * filename, int line_number, const char *error_mnemonic, int error_code, const char *format, ...)
{
    context->error_code = error_code;

    if (context->debug.stream && level < context->debug.level)
    {
        char extra[128];
        snprintf(extra, sizeof(extra), "%s(%d)", shorten_error_mnemonic(error_mnemonic), error_code);

        va_list args;
        va_start(args, format);
        output_message(context->debug.stream, filename, line_number, "error", extra, format, args);
        va_end(args);
    }
}

void _amqp_debug(const amqp_context_t *context, int level, const char * filename, int line_number, const char *function, const char *format, ...)
{
    if (context->debug.stream && level < context->debug.level)
    {
        va_list args;
        va_start(args, format);
        output_message(context->debug.stream, filename, line_number, "debug", function, format, args);
        va_end(args);
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
    
    write(2, buffer, leader_length + actual + 1);
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
