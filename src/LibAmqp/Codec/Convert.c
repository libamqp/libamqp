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

#include <stdio.h>
#include <string.h>

#include "Buffer/Buffer.h"
#include "Codec/Type/Type.h"
#include "Codec/Decode/Decode.h"

char *amqp_convert_bytes_to_cstr(amqp_type_t *type)
{
    char *result = amqp_malloc(type->position.size + 1 TRACE_ARGS);
    strncpy(result, (const char *) &type->context->convert_buffer->bytes[type->position.index], type->position.size);
    return result;
}

