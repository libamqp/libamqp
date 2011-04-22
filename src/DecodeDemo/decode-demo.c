
#include <stdio.h>

#include "Memory/Memory.h"
#include "Buffer/Buffer.h"
#include "Codec/Decode.h"
#include "Type/TypePrint.h"
#include "Context/Context.h"


int main(int argc, char *argv[])
{
    int c;
    amqp_type_t *type;
    amqp_context_t *context = AMQP_NEW(amqp_context_t);

    while ((c = getc(stdin)) != -1)
    {
        amqp_buffer_putc(context->decode.buffer, c);
    }

    type = amqp_decode(context);
    amqp_type_print_formatted(type);

    AMQP_DELETE(amqp_context_t, context);

    return 0;
}
