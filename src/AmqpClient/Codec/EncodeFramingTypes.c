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
#include <stdarg.h>


#include "Codec/EncodeFramingTypes.h"
#include "Memory/Memory.h"


// TODO - this is going too far, simplify
amqp_type_t *
amqp_encode_multiple_true_va(amqp_context_t *context, amqp_type_encode_meta_data_t *meta_data, ...)
{
    va_list args;
    va_list args_copy;
    void *arg;
    amqp_type_t *result;
    amqp_type_description_t descriptions[32];
    int count;
    int needed = 0;

    va_start(args, meta_data);
    for (count = 0; count < 32 && needed < 254; count++)
    {
        va_copy(args_copy, args);
        arg = va_arg(args, void *);
        if (arg == 0) break;
        (*meta_data->describe)(context, &descriptions[count], args_copy);
        needed += descriptions[count].size + descriptions[count].overhead;
    }
    va_end(args);

    result = amqp_start_encode_described_type(context);
    amqp_encode_boolean(context, true);

    va_start(args, meta_data);
    if (count == 0)
    {
        amqp_encode_null(context);
    }
    else if (count == 1)
    {
        (*meta_data->encode)(context, &descriptions[0], args);
    }
    else
    {
        amqp_type_t *list = (count >= 32|| needed > 254) ? amqp_encode_list_32(context) : amqp_encode_list_8(context);
        int i;
        for (i = 0; ; i++)
        {
            va_copy(args_copy, args);
            arg = va_arg(args, void *);
            if (arg == 0) break;
            (*meta_data->encode)(context, i < count ? &descriptions[i] : 0, args_copy);
        }
        amqp_complete_type(context, list);
    }
    va_end(args);

    amqp_complete_type(context, result);
    return result;
}

