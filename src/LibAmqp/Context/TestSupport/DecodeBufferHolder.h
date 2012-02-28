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

#ifndef LIBAMQP_CONTEXT_TEST_SUPPORT_DECODE_BUFFER_HOLDER_H
#define LIBAMQP_CONTEXT_TEST_SUPPORT_DECODE_BUFFER_HOLDER_H

#include "Context/TestSupport/ContextHolder.h"
#include "TestData/TestData.h"

namespace TestSupport
{
    class DecodeBufferHolder : public virtual ContextHolder
    {
    public:
        DecodeBufferHolder();
        ~DecodeBufferHolder();

        void load_decode_buffer(test_data::TestData &data);

    public:
        amqp_buffer_t *decode_buffer;
    };
}

#ifndef CHECK_BUFFERS_MATCH
#define CHECK_BUFFERS_MATCH(buffer, byte_array) \
     CHECK(t::compare_buffers(context, byte_array.bytes(), byte_array.size(), buffer))
#endif

#endif

