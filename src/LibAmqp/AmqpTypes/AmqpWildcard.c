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

#include <assert.h>
#include <string.h>

#include "Misc/Bits.h"
#include "AmqpTypes/AmqpTypesInternal.h"
#include "AmqpTypes/AmqpWildcard.h"
#include "Codec/Type/TypeExtension.h"
#include "AmqpTypes/AmqpVariableInternal.h"

#include "debug_helper.h"


static inline
void cleanup(amqp_context_t *context, amqp_wildcard_t *wildcard)
{
    wildcard->type = 0;
    wildcard->message_type_decoder = 0;
    if (wildcard->messaging_type && wildcard->cleanup_messaging_type)
    {
        wildcard->cleanup_messaging_type(context, wildcard->messaging_type);
    }
    wildcard->messaging_type = 0;
}

static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_wildcard_t *wildcard = (amqp_wildcard_t *) type;
    cleanup(context, wildcard);
}

/*
static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_wildcard_t *wildcard = (amqp_wildcard_t *) type;
    cleanup(context, wildcard);
    AMQP_FREE(context, wildcard);
}
*/

void amqp_wildcard_initialize_as_null(amqp_context_t *context, amqp_wildcard_t *wildcard)
{
    wildcard->leader.fn_table = 0;
    wildcard->type = 0;
    wildcard->message_type_decoder = 0;
    wildcard->messaging_type = 0;
    wildcard->cleanup_messaging_type = 0;
}

void amqp_wildcard_initialize(amqp_context_t *context, amqp_wildcard_t *wildcard, amqp_type_t *type, amqp_messaging_type_decoder_t message_type_decoder)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    wildcard->leader.fn_table = &table;
    wildcard->type = type;
    wildcard->message_type_decoder = message_type_decoder;
    wildcard->messaging_type = 0;
    wildcard->cleanup_messaging_type = 0;
}
