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

#include <stdlib.h>
#include <string.h>

#include "Context/ContextTestSupport.h"
#include "Buffer/Buffer.h"
#include "TestData/TestData.h"

namespace SuiteContext
{
    ContextFixture::ContextFixture()
    {
        context = amqp_create_context();
    }

    ContextFixture::~ContextFixture()
    {
        amqp_destroy_context(context);
        context = 0;
    }

    void ContextFixture::load_decode_buffer(test_data::TestData &data)
    {
        data.transfer_to(context->decode.buffer);
    }
}

