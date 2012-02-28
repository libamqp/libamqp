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

#ifndef LIBAMQP_AMQP_TYPES_AMQP_TYPES_TEST_FIXTURE_H
#define LIBAMQP_AMQP_TYPES_AMQP_TYPES_TEST_FIXTURE_H

#include <TestHarness.h>
#include "Context/TestSupport/ContextHolder.h"
#include "Context/TestSupport/BufferHolder.h"
#include "Context/TestSupport/TypeHolder.h"

#include "Codec/Decode/Decode.h"
#include "Codec/Type/TypeExtension.h"

#include "AmqpTypes/AmqpTypes.h"
#include "debug_helper.h"

namespace SuiteAmqpTypes
{
    class AmqpTypesTestFixture :
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::BufferHolder,
            public virtual TestSupport::TypeHolder
    {
    public:
        AmqpTypesTestFixture();
        ~AmqpTypesTestFixture();

        static int key_count();
        static const char *keys[40];
        static const char *data[40];

        static int compare(const char *lhs, const char *rhs);
        static uint32_t hash(const char *data);
    };
}

#endif


