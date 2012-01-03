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

#include "AmqpTypes/AmqpMessagingType.h"
#include "AmqpTypes/AmqpTypesInternal.h"

void amqp_messaging_type_cleanup(amqp_context_t *context, amqp_messaging_type_t *messaging_type)
{
    if (messaging_type && messaging_type->leader.fn_table && messaging_type->leader.fn_table->dtor)
    {
        messaging_type->leader.fn_table->dtor(context, (amqp_amqp_type_t *) messaging_type);
    }
}
