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

#ifndef LIBAMQP_AMQP_TYPES_TYPES_H
#define LIBAMQP_AMQP_TYPES_TYPES_H

#include <stdlib.h>
#include "AmqpTypes/AmqpLeader.h"

#include "AmqpTypes/AmqpArray.h"
#include "AmqpTypes/AmqpBinary.h"
#include "AmqpTypes/AmqpEntry.h"
#include "AmqpTypes/AmqpList.h"
#include "AmqpTypes/AmqpMap.h"
#include "AmqpTypes/AmqpHashTable.h"
#include "AmqpTypes/AmqpMultiple.h"
#include "AmqpTypes/AmqpString.h"
#include "AmqpTypes/AmqpSymbol.h"
#include "AmqpTypes/AmqpWildcard.h"

#include "Amqp/AmqpDefinitions.h"

union amqp_amqp_type_t
{
    amqp_leader_t leader;
    amqp_binary_t binary;
    amqp_entry_t entry;
    amqp_hash_table_t hash_table;
    amqp_list_t list;
    amqp_map_t map;
    amqp_multiple_symbol_t multiple_symbol;
    amqp_string_t string;
    amqp_symbol_t symbol;
    amqp_wildcard_t wildcard;
};

#endif
