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

#include "TestHarness.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "Buffer/BufferTestSupport.h"
#include "Codec/CodecTestFixture.h"
#include "Codec/Type/TypePrint.h"
#include "Context/Context.h"

namespace SuiteCodec
{
    CodecTestFixture::CodecTestFixture()
    {
    }
    
    CodecTestFixture::~CodecTestFixture()
    {
    }

    amqp_type_t *
    CodecTestFixture::allocate_type()
    {
        return amqp_allocate_type(context);
    }

    void
    CodecTestFixture::deallocate_type(amqp_type_t *type)
    {
        amqp_deallocate_type(context, type);
    }

    bool
    CodecTestFixture::check_valid_array()
    {
        return type != null &&
            amqp_type_is_valid(type) &&
            amqp_type_is_container(type) &&
            amqp_type_is_array(type) &&
            !(amqp_type_is_map(type) || amqp_type_is_list(type));
    }
}
