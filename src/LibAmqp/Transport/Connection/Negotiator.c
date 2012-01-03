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

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/Negotiator.h"

#include "debug_helper.h"

void amqp_negotiator_encode_version(amqp_context_t *context, uint32_t version, amqp_buffer_t *buffer)
{
    assert(amqp_buffer_size(buffer) == 0);
    assert(amqp_buffer_check_free_space(buffer, 8));

    amqp_unchecked_putc(buffer, 'A');
    amqp_unchecked_putc(buffer, 'M');
    amqp_unchecked_putc(buffer, 'Q');
    amqp_unchecked_putc(buffer, 'P');
    amqp_unchecked_putc(buffer, amqp_version_protocol_id(version));
    amqp_unchecked_putc(buffer, amqp_version_major(version));
    amqp_unchecked_putc(buffer, amqp_version_minor(version));
    amqp_unchecked_putc(buffer, amqp_version_release(version));
}

uint32_t amqp_negotiator_decode_version(amqp_context_t *context, amqp_buffer_t *buffer)
{
    uint32_t result = 0;

    int ok = (amqp_buffer_getc(buffer) == 'A') &&
            (amqp_buffer_getc(buffer) == 'M') &&
            (amqp_buffer_getc(buffer) == 'Q') &&
            (amqp_buffer_getc(buffer) == 'P');

    if (ok)
    {
        result = amqp_buffer_getc(buffer);
        result = (result << 8) | amqp_buffer_getc(buffer);
        result = (result << 8) | amqp_buffer_getc(buffer);
        result = (result << 8) | amqp_buffer_getc(buffer);
    }

    return result;
}

uint32_t amqp_negotiator_choose_tls_protocol_version(amqp_connection_t *connection, int requested_version)
{
    // Only support amqp 1.0.0 so nothing complicated needed here
//    return connection->specification_version.tls;
    not_implemented(amqp_negotiator_choose_tls_protocol_version);
}

uint32_t amqp_negotiator_choose_sasl_protocol_version(amqp_connection_t *connection, int requested_version)
{
    // Only support amqp 1.0.0 so nothing complicated needed here
    return connection->specification_version.required.sasl;
}

uint32_t amqp_negotiator_choose_amqp_protocol_version(amqp_connection_t *connection, int requested_version)
{
    // Only support amqp 1.0.0 so nothing complicated needed here
    return connection->specification_version.required.amqp;
}
