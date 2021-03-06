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
#include <assert.h>

#include "Codec/Codec.h"

int main(int argc, char *argv[])
{
    int c;
    amqp_type_t *type;
    amqp_context_t *context = amqp_create_context();
    amqp_buffer_t *buffer = amqp_allocate_buffer(context);
    int rc;

    while ((c = getc(stdin)) != -1)
    {
        amqp_buffer_putc(buffer, c);
    }

    type = amqp_decode(context, buffer);
    amqp_type_print_formatted(context, type);

    amqp_deallocate_type(context, type);
    amqp_deallocate_buffer(context, buffer);
    rc = amqp_context_destroy(context);

    assert(rc);

    return 0;
}
