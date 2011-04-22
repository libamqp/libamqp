#include "ErrorHandling/ErrorHandling.h"

#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "Context/Context.h"


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
    const char *leader = "AMQP_ERROR_";

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
        char message[128];
        strerror_r(context->error_code, message, sizeof(message));

        char extra[128];
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
