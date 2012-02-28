/*
   Copyright 2011-2012 StormMQ Limited

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

#include <stdarg.h>
#include <stdio.h>
#include "Context/Context.h"

amqp_debug_print_c_t *
amqp_context_define_putc_function(amqp_context_t *context, amqp_debug_print_c_t *putc)
{
    amqp_debug_print_c_t *old_value = context->config.putc;
    context->config.putc = putc;
    return old_value;
}

int amqp_context_printf(amqp_context_t *context, int level, const char *format, ...)
{
    char buffer[256];
    int i, n;
    va_list ap;

    if (level >= context->debug.level)
    {
        return 0;
    }
    va_start(ap, format);
    n = vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    for (i = 0; i < n; i++)
    {
        amqp_context_putc(context, buffer[i]);
    }
    return n;
}

static void putc_repeat(amqp_context_t *context, int c, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        (*context->config.putc)(c);
    }
}

int amqp_context_putc(amqp_context_t *context, int c)
{
    if (context->debug.indent.auto_indent)
    {
        if ((c == '\n') && ((context->debug.indent.last_char == '{') || (context->debug.indent.last_char == '[')))
        {
            context->debug.indent.indent += context->debug.indent.auto_indent;
        }
        if (context->debug.indent.last_char == '\n')
        {
            if (c == '\n') return c;
            if ((c == '}') || (c == ']'))
            {
                context->debug.indent.indent -= context->debug.indent.auto_indent;
            }
        }
    }
    if (context->debug.indent.last_char == '\n' && context->debug.indent.indent > 0)
    {
        putc_repeat(context, '\t', context->debug.indent.indent / 8);
        putc_repeat(context, ' ', context->debug.indent.indent % 8);
    }
    context->debug.indent.last_char = c;
    return (*context->config.putc)(c);
}

int amqp_context_increase_print_indent(amqp_context_t *context, int delta)
{
    int old_indent = context->debug.indent.indent;
    context->debug.indent.indent += delta;
    return old_indent;
}
int amqp_context_set_print_indent(amqp_context_t *context, int indent)
{
    int old_indent = context->debug.indent.indent;
    context->debug.indent.indent = indent;
    return old_indent;
}

int amqp_context_set_auto_indent(amqp_context_t *context, int amount)
{
    int old_indent = context->debug.indent.auto_indent;
    context->debug.indent.auto_indent = amount;
    return old_indent;
}
