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

#include <stdlib.h>
#include <string.h>

#include "TestHarness.h"
#include "Context/TestSupport/DecodeBufferHolder.h"

namespace TestSupport
{
    DecodeBufferHolder::DecodeBufferHolder()
    {
        decode_buffer = amqp_allocate_buffer(context);
    }

    DecodeBufferHolder::~DecodeBufferHolder()
    {
        amqp_deallocate_buffer(context, decode_buffer);
    }

    void DecodeBufferHolder::load_decode_buffer(test_data::TestData &data)
    {
        data.transfer_to(decode_buffer);
    }

}

