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
#include <string.h>

#include "AmqpTypes/Decode/AmqpDecodeInternal.h"

#include "debug_helper.h"

void _amqp_decode_field_error(amqp_context_t *context, const char *filename, int line_number, const char *error_mnemonic, int error_code, int field, int total, const char *format, ...)
{
    char message[256];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    _amqp_error(context, 1, filename, line_number, 0, error_mnemonic, error_code, "Failed to decode field at position %d (%d of %d). %s", field, field + 1, total, message);
}
