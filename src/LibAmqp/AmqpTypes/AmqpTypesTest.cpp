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

#include "AmqpTypes/AmqpTypesTestSupport.h"

#include "AmqpTypes/AmqpTypes.h"

SUITE(AmqpTypes)
{
    TEST_FIXTURE(AmqpTypesFixture, type_sizes)
    {
        SOUTV(sizeof(amqp_binary_t));
        SOUTV(sizeof(amqp_list_t));
        SOUTV(sizeof(amqp_map_t));
        SOUTV(sizeof(amqp_multiple_symbol_t));
        SOUTV(sizeof(amqp_string_t));
        SOUTV(sizeof(amqp_symbol_t));
    }
}
