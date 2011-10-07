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

#include "Context/Context.h"
#include "Transport/Frame/TypeInit.h"

#include "debug_helper.h"


int amqp_type_to_symbol(amqp_symbol_t *symbol, amqp_buffer_t *buffer, amqp_type_t *type)
{
    // TODO - deal with case where symbol straddles buffer fragments - not an issue for performative frames as they should fit in the smallest buffer.
    amqp_symbol_initialize_reference(symbol, buffer, amqp_buffer_pointer(buffer, type->position.index), type->position.size);
    return true;
}

int amqp_type_to_multiple_symbol(amqp_context_t *context, amqp_frame_sasl_mechanisms_t *sasl_mechanisms, amqp_buffer_t *buffer, amqp_type_t *type)
{
    not_implemented(todo);
}
