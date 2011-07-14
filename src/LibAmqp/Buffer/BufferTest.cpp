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

#include "Buffer/BufferTestSupport.h"
#include "Context/ErrorHandling.h"
#include "debug_helper.h"

SUITE(Buffer)
{
    unsigned char simple_test_data[] = {
        0x70, 0x10, 0x07, 0x03, 0x01
    };

    class LoadedBufferFixture : public BufferFixture
    {
    public:
        LoadedBufferFixture()
        {
            load_buffer(simple_test_data, sizeof(simple_test_data));
        }
        ~LoadedBufferFixture()
        {
        }

    public:
    };

    TEST_FIXTURE(BufferFixture, allocation)
    {
        CHECK_EQUAL(1UL, pool.stats.outstanding_allocations);
        CHECK_EQUAL(1UL, pool.stats.total_allocation_calls);
        break_one();
        amqp_deallocate(&pool, buffer);
        buffer = 0;

        CHECK_EQUAL(0UL, pool.stats.outstanding_allocations);
        CHECK_EQUAL(1UL, pool.stats.total_allocation_calls);
    }

    TEST_FIXTURE(LoadedBufferFixture, amqp_buffer_wrap_array)
    {
        CHECK_EQUAL(buffer->limit.index, (size_t) 0);
        CHECK_EQUAL(buffer->limit.size, sizeof(simple_test_data));

        for (size_t i = 0; i < sizeof(simple_test_data); i++)
        {
            CHECK_EQUAL(buffer->bytes[i], simple_test_data[i]);
        }
    }

    TEST_FIXTURE(LoadedBufferFixture, amqp_buffer_at_end)
    {
        amqp_buffer_reset(buffer);
        CHECK(amqp_buffer_at_end(buffer));
    }

    TEST_FIXTURE(LoadedBufferFixture, getc)
    {
        CHECK_EQUAL(amqp_buffer_getc(buffer), 0x70);
        CHECK_EQUAL(amqp_buffer_getc(buffer), 0x10);
        CHECK_EQUAL(amqp_buffer_getc(buffer), 0x07);
        CHECK_EQUAL(amqp_buffer_getc(buffer), 0x03);
        CHECK_EQUAL(amqp_buffer_getc(buffer), 0x01);
        CHECK(amqp_buffer_at_end(buffer));
        CHECK_EQUAL(amqp_buffer_getc(buffer), -1);
    }

    TEST_FIXTURE(BufferFixture,  amqp_ntoh_16_unsigned)
    {
        uint16_t v;
        unsigned char data[] = { 0x01, 0x02 };

        load_buffer(data, sizeof(data));

        v = amqp_ntoh_16(buffer, 0)._ushort;
        CHECK_EQUAL(0x0102, v);
    }

    TEST_FIXTURE(BufferFixture, amqp_ntoh_16)
    {
        int16_t v;
        unsigned char data[] = { 0xff, 0xfe };

        load_buffer(data, sizeof(data));

        v = amqp_ntoh_16(buffer, 0)._short;
        CHECK_EQUAL(-2, v);
    }

    TEST_FIXTURE(BufferFixture, amqp_ntoh_32)
    {
        uint32_t v;
        unsigned char data[] = { 0x01, 0x02, 0x04, 0x08 };

        load_buffer(data, sizeof(data));

        v = amqp_ntoh_32(buffer, 0)._uint;
        CHECK_EQUAL(0x01020408U, v);
    }

    TEST_FIXTURE(BufferFixture, amqp_ntoh_64)
    {
        unsigned char data[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

        load_buffer(data, sizeof(data));

        amqp_eight_byte_t v;
        amqp_ntoh_64(&v, buffer, 0);
        CHECK_EQUAL(0x0102040810204080ULL, v._ulong);
    }

    TEST_FIXTURE(BufferFixture,  amqp_hton_16)
    {
        amqp_two_byte_t v;

        v._short = -2;
        amqp_hton_16(buffer, v);
        CHECK_EQUAL((size_t) 0, buffer->limit.index);
        CHECK_EQUAL((size_t) 2, buffer->limit.size);
        CHECK_EQUAL(0xfe, buffer->bytes[1]);

        CHECK_EQUAL(-2, amqp_ntoh_16(buffer, 0)._short);
    }

    TEST_FIXTURE(BufferFixture,  amqp_hton_32)
    {
        amqp_four_byte_t v;

        v._int = -2;
        amqp_hton_32(buffer, v);
        CHECK_EQUAL((size_t) 0, buffer->limit.index);
        CHECK_EQUAL((size_t) 4, buffer->limit.size);
        CHECK_EQUAL(0xfe, buffer->bytes[3]);

        CHECK_EQUAL(-2, amqp_ntoh_32(buffer, 0)._int);
    }

    TEST_FIXTURE(BufferFixture,  amqp_hton_64)
    {
        amqp_eight_byte_t v;

        v._long = -2;
        amqp_hton_64(buffer, v);
        CHECK_EQUAL((size_t) 0, buffer->limit.index);
        CHECK_EQUAL((size_t) 8, buffer->limit.size);
        CHECK_EQUAL(0xfe, buffer->bytes[7]);

        amqp_eight_byte_t verify;
        amqp_ntoh_64(&verify, buffer, 0);
        CHECK_EQUAL(-2, verify._long);
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_read_size_one)
    {
        unsigned char data[] = { 0x01, 0x02, 0x0a, 0x0e };
        load_buffer(data, sizeof(data));

        CHECK_EQUAL(1U, amqp_buffer_read_size(buffer, 1));
        CHECK_EQUAL(2U, amqp_buffer_read_size(buffer, 1));
        CHECK_EQUAL(10U, amqp_buffer_read_size(buffer, 1));
        CHECK_EQUAL(14U, amqp_buffer_read_size(buffer, 1));
        CHECK_EQUAL(-1U, amqp_buffer_read_size(buffer, 1));
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_read_size_four)
    {
        unsigned char data[] = { 0, 0, 0, 0x0e, 0, 0, 2, 1 };
        load_buffer(data, sizeof(data));

        CHECK_EQUAL(14U, amqp_buffer_read_size(buffer, 4));
        CHECK_EQUAL(513U, amqp_buffer_read_size(buffer, 4));
        CHECK_EQUAL(-1U, amqp_buffer_read_size(buffer, 4));
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_read_size_bad)
    {
        unsigned char data[] = { 0, 0,0x0e};
        load_buffer(data, sizeof(data));

        CHECK_EQUAL(-1U, amqp_buffer_read_size(buffer, 4));
    }
}
