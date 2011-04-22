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

#ifndef STORMMQ_AMQP_CONTEXT_TEST_SUPPORT_H
#define STORMMQ_AMQP_CONTEXT_TEST_SUPPORT_H

#include "Buffer/BufferTestSupport.h"
#include "Context/Context.h"

namespace t
{
    class Context
    {
    public:
        Context();
        Context(const ByteArray &buffer);
        ~Context();
        void transfer_from(const ByteArray &buffer);

        amqp_context_t *context() const { return context_; }
        amqp_buffer_t *buffer() const { return context_->decode.buffer; }

        void transfer_from(ByteArray& bytes) { bytes.transfer_to(context_->decode.buffer);}

    private:
        amqp_context_t *context_;
    };
}

#endif

