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
#include "Buffer/BufferInternal.h"
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

    TEST(buffer_and_fragment_size_macros_should_be_powers_of_two)
    {
        CHECK(AMQP_BUFFER_MAX_SIZE != 0);
        CHECK((AMQP_BUFFER_MAX_SIZE & (AMQP_BUFFER_MAX_SIZE - 1)) == 0);

        CHECK(AMQP_BUFFER_FRAGMENT_SIZE != 0);
        CHECK((AMQP_BUFFER_FRAGMENT_SIZE & (AMQP_BUFFER_FRAGMENT_SIZE - 1)) == 0);
    }

    TEST_FIXTURE(BufferFixture, allocation)
    {
        CHECK_EQUAL(1UL, pool.stats.outstanding_allocations);
        CHECK_EQUAL(1UL, pool.stats.total_allocation_calls);

        amqp_deallocate(&m_context, &pool, buffer);
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
            CHECK_EQUAL(amqp_unchecked_getc_at(buffer, i), simple_test_data[i]);
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
        CHECK_EQUAL(0xfe, amqp_unchecked_getc_at(buffer, 1));

        CHECK_EQUAL(-2, amqp_ntoh_16(buffer, 0)._short);
    }

    TEST_FIXTURE(BufferFixture,  amqp_hton_32)
    {
        amqp_four_byte_t v;

        v._int = -2;
        amqp_hton_32(buffer, v);
        CHECK_EQUAL((size_t) 0, buffer->limit.index);
        CHECK_EQUAL((size_t) 4, buffer->limit.size);
        CHECK_EQUAL(0xfe, amqp_unchecked_getc_at(buffer, 3));

        CHECK_EQUAL(-2, amqp_ntoh_32(buffer, 0)._int);
    }

    TEST_FIXTURE(BufferFixture,  amqp_hton_64)
    {
        amqp_eight_byte_t v;

        v._long = -2;
        amqp_hton_64(buffer, v);
        CHECK_EQUAL((size_t) 0, buffer->limit.index);
        CHECK_EQUAL((size_t) 8, buffer->limit.size);
        CHECK_EQUAL(0xfe, amqp_unchecked_getc_at(buffer, 7));

        amqp_eight_byte_t verify;
        amqp_ntoh_64(&verify, buffer, 0);
        CHECK_EQUAL(-2, verify._long);
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_read_size_one)
    {
        unsigned char data[] = { 0x01, 0x02, 0x0a, 0x0e };
        load_buffer(data, sizeof(data));

        CHECK_EQUAL(1U, amqp_buffer_read_size_field(buffer, 1));
        CHECK_EQUAL(2U, amqp_buffer_read_size_field(buffer, 1));
        CHECK_EQUAL(10U, amqp_buffer_read_size_field(buffer, 1));
        CHECK_EQUAL(14U, amqp_buffer_read_size_field(buffer, 1));
        CHECK_EQUAL((uint32_t) -1, amqp_buffer_read_size_field(buffer, 1));
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_read_size_four)
    {
        unsigned char data[] = { 0, 0, 0, 0x0e, 0, 0, 2, 1 };
        load_buffer(data, sizeof(data));

        CHECK_EQUAL(14U, amqp_buffer_read_size_field(buffer, 4));
        CHECK_EQUAL(513U, amqp_buffer_read_size_field(buffer, 4));
        CHECK_EQUAL((uint32_t) -1, amqp_buffer_read_size_field(buffer, 4));
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_read_size_bad)
    {
        unsigned char data[] = { 0, 0,0x0e};
        load_buffer(data, sizeof(data));

        CHECK_EQUAL((uint32_t) -1, amqp_buffer_read_size_field(buffer, 4));
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_grow)
    {
        const size_t size = 32 * 1024;
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, buffer->capacity);

        amqp_buffer_grow(&m_context, buffer, size);
        CHECK_EQUAL(size, buffer->capacity);

        size_t i;
        for (i = 0; i < size; i++)
        {
            amqp_buffer_putc(buffer, (int) (i % 251));
        }
        CHECK_EQUAL(size, buffer->limit.size);
        CHECK_EQUAL(0UL, buffer->limit.index);
        CHECK_EQUAL(-1, amqp_buffer_putc(buffer, 'X'));

        for (i = 0; i < size; i++)
        {
            CHECK_EQUAL((int) (i % 251), amqp_buffer_getc(buffer));
        }
        CHECK_EQUAL(size, buffer->limit.index);
        CHECK_EQUAL(-1, amqp_buffer_getc(buffer));
    }

    TEST_FIXTURE(BufferFixture, amqp_buffer_growt_rounds_to_fragment_size)
    {
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, buffer->capacity);
        amqp_buffer_grow(&m_context, buffer, AMQP_BUFFER_FRAGMENT_SIZE + AMQP_BUFFER_FRAGMENT_SIZE / 2);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE * 2, buffer->capacity);
    }

    TEST_FIXTURE(BufferFixture, initialize_write_io_vec_single_fragment)
    {
        int iov_count;
        struct iovec *p =  amqp_buffer_write_io_vec(buffer, AMQP_BUFFER_FRAGMENT_SIZE, &iov_count);

        CHECK_EQUAL(buffer->fragments[0], p[0].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[0].iov_len);
        CHECK_EQUAL(1, iov_count);
    }
    
    TEST_FIXTURE(BufferFixture, initialize_write_io_vec_many_fragments)
    {
        int iov_count;
        const int size = AMQP_BUFFER_FRAGMENT_SIZE * 4;
        amqp_buffer_grow(&m_context, buffer, size);
        struct iovec *p =  amqp_buffer_write_io_vec(buffer, AMQP_BUFFER_FRAGMENT_SIZE * 3, &iov_count);

        CHECK_EQUAL(3, iov_count);

        CHECK_EQUAL(buffer->fragments[0], p[0].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[0].iov_len);

        CHECK_EQUAL(buffer->fragments[1], p[1].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[1].iov_len);

        CHECK_EQUAL(buffer->fragments[2], p[2].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[2].iov_len);

        CHECK_EQUAL((void *) 0, p[3].iov_base);
    }

    TEST_FIXTURE(BufferFixture, initialize_write_io_vec_many_fragments_with_offset)
    {
        int iov_count;
        const int size = AMQP_BUFFER_FRAGMENT_SIZE * 4;
        amqp_buffer_grow(&m_context, buffer, size);
        size_t amount = AMQP_BUFFER_FRAGMENT_SIZE / 2;
        int required = AMQP_BUFFER_FRAGMENT_SIZE * 3;
        amqp_buffer_advance_write_point(buffer, amount);

        struct iovec *p =  amqp_buffer_write_io_vec(buffer, required, &iov_count);
        CHECK_EQUAL(4, iov_count);

        CHECK_EQUAL(&buffer->fragments[0][amount], p[0].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE - amount, p[0].iov_len);

        CHECK_EQUAL(buffer->fragments[1], p[1].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[1].iov_len);

        CHECK_EQUAL(buffer->fragments[2], p[2].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[2].iov_len);

        CHECK_EQUAL(buffer->fragments[3], p[3].iov_base);
        CHECK_EQUAL(amount, p[3].iov_len);

        int sum = 0;
        for (int i = 0; i < iov_count; i++)
        {
            sum += p[i].iov_len;
        }
        CHECK_EQUAL(required, sum);

        CHECK_EQUAL((void *) 0, p[4].iov_base);
    }

    TEST_FIXTURE(BufferFixture, initialize_read_io_with_empty_buffer)
    {
        int iov_count;
        struct iovec *p =  amqp_buffer_read_io_vec(buffer, &iov_count);

        CHECK_EQUAL(buffer->fragments[0], p[0].iov_base);
        CHECK_EQUAL(0U, p[0].iov_len);
        CHECK_EQUAL(1, iov_count);
    }

    TEST_FIXTURE(BufferFixture, initialize_read_io_with_single_fragment)
    {
        int iov_count;

        amqp_buffer_advance_write_point(buffer, AMQP_BUFFER_FRAGMENT_SIZE / 4);

        struct iovec *p =  amqp_buffer_read_io_vec(buffer, &iov_count);

        CHECK_EQUAL(buffer->fragments[0], p[0].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE / 4, p[0].iov_len);
        CHECK_EQUAL(1, iov_count);
    }

    TEST_FIXTURE(BufferFixture, initialize_read_io_vec_many_fragments)
    {
        int iov_count;
        const int size = AMQP_BUFFER_FRAGMENT_SIZE * 4;
        amqp_buffer_grow(&m_context, buffer, size);

        size_t n = AMQP_BUFFER_FRAGMENT_SIZE * 3 + AMQP_BUFFER_FRAGMENT_SIZE / 4;
        size_t old_n = AMQP_BUFFER_FRAGMENT_SIZE / 5;

        amqp_buffer_advance_write_point(buffer, n);
        amqp_buffer_advance_read_index(buffer, old_n);

        struct iovec *p =  amqp_buffer_read_io_vec(buffer, &iov_count);

        CHECK_EQUAL(4, iov_count);

        CHECK_EQUAL(&buffer->fragments[0][old_n], p[0].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE - old_n, p[0].iov_len);

        CHECK_EQUAL(buffer->fragments[1], p[1].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[1].iov_len);

        CHECK_EQUAL(buffer->fragments[2], p[2].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[2].iov_len);

        CHECK_EQUAL(buffer->fragments[3], p[3].iov_base);
        CHECK_EQUAL(AMQP_BUFFER_FRAGMENT_SIZE, p[2].iov_len);

        CHECK_EQUAL((void *) 0, p[4].iov_base);
    }


}
