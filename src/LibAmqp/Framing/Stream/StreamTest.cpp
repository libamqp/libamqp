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

#include <TestHarness.h>

#include "Context/Context.h"

#include "Framing/Stream/Stream.h"
#include "Framing/FramingTestSupport.h"

SUITE(StreamTests)
{
    class StreamFixture : public SuiteFraming::FramingFixture
    {
    public:
        StreamFixture()
        {
            buffer = amqp_allocate_buffer(context);
            stream = 0;
        }
        ~StreamFixture()
        {
            amqp_destroy_stream(context, stream);
            amqp_deallocate_buffer(context, buffer);
        }
    public:
        amqp_buffer_t *buffer;
        amqp_stream_t *stream;
    };

    TEST_FIXTURE(StreamFixture, empty_buffer_stream)
    {
        int rc;

        stream = amqp_buffer_stream_create(context, buffer);
        rc = amqp_buffer_read_from_stream(context->decode.buffer, stream, 1);
        CHECK_EQUAL(0, rc);
    }

    TEST_FIXTURE(StreamFixture, buffer_stream_read)
    {
        int rc;
        int c;

        test_data::hello_world.transfer_to(buffer);
        stream = amqp_buffer_stream_create(context, buffer);

        rc = amqp_buffer_read_from_stream(context->decode.buffer, stream, 3);
        CHECK_EQUAL(3, rc);

        CHECK_EQUAL((size_t) 3, amqp_buffer_size(context->decode.buffer));
        CHECK_EQUAL((size_t) 0, amqp_buffer_index(context->decode.buffer));

        c = amqp_buffer_getc(context->decode.buffer);
        CHECK_EQUAL(0xa1, c);
        c = amqp_buffer_getc(context->decode.buffer);
        CHECK_EQUAL(0x0b, c);
        c = amqp_buffer_getc(context->decode.buffer);
        CHECK_EQUAL('H', c);
    }
}
