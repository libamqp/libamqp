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
#include "AmqpTypes/AmqpVariable.h"
#include "Codec/Type/TypeExtension.h"

#include "debug_helper.h"

uint8_t *duplicate(amqp_context_t *context, const char *data, size_t size)
{
    uint8_t *result = (uint8_t *) amqp_malloc(context, size);
    memcpy(result, data, size);
    return result;
}

static
amqp_memory_t *data_to_block(amqp_variable_t *variable)
{
    if (variable->type)
    {
        assert(amqp_type_is_variable(variable->type));
        return (amqp_memory_t *) variable->type->value.variable.buffer;
    }
    else
    {
        return (amqp_memory_t *) &variable->block;
    }
}

static
size_t data_offset(amqp_variable_t *variable)
{
    return variable->type ? variable->type->position.index : 0;
}

int amqp_variable_compare(amqp_variable_t *lhs, amqp_variable_t *rhs)
{
    int n, rc;
    assert(lhs != 0 && rhs != 0);

    n = lhs->size;
    if (rhs->size < n) n = rhs->size;

    rc = amqp_block_compare(data_to_block(lhs), data_offset(lhs), data_to_block(rhs), data_offset(rhs), n);
    return rc != 0 ? rc : lhs->size - rhs->size;
}

int amqp_variable_compare_with_cstr(amqp_variable_t *lhs, const char *rhs)
{
    struct {
        struct amqp_block_header header;
        unsigned char *rhs;
    } block;

    int n, rc;
    int rhs_size;

    assert(lhs != 0 && rhs != 0);

    rhs_size = strlen(rhs);
    block.header.capacity = block.header.fragment_size = rhs_size;
    block.header.n_fragments = 1;
    block.rhs = (uint8_t *) rhs;

    n = lhs->size;
    if (rhs_size < n) n = rhs_size;

    rc = amqp_block_compare(data_to_block(lhs), data_offset(lhs), (amqp_memory_t *) &block, 0, n);
    return rc != 0 ? rc : lhs->size - rhs_size;
}

uint32_t amqp_variable_hash(amqp_variable_t *variable)
{
    assert(variable != 0);
    // TODO - introduce a hash for fragmented types
    return variable->type ?
        amqp_hash((void *) amqp_buffer_pointer(variable->type->value.variable.buffer, variable->type->position.index), variable->type->position.size) :
        amqp_hash((void *) variable->data, variable->size);
}

