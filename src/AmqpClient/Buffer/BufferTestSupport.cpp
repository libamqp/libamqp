#include <stdlib.h>
#include <string.h>

#include "Memory/Memory.h"
#include "Buffer/BufferTestSupport.h"


namespace test_data
{
    unsigned char empty_buffer_bytes[0];
    t::ByteArray empty_buffer(empty_buffer_bytes, 0);

    test_data_bytes(hello_world, 0xA1,
        0x0B,
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'
    );

    test_data_bytes(utf8_string, 0xA1,
        11,
        'u', 't', 'f', '8', ' ', 's', 't', 'r', 'i', 'n', 'g'
    );

    test_data_bytes(hello_big_world, 0xb1,
        0, 0, 0, 0x0f,
        'H', 'e', 'l', 'l', 'o', ' ', 'B', 'i', 'g', ' ', 'W', 'o', 'r', 'l', 'd'
    );

    test_data_bytes(foo_bar_symbol_32,
        0xb3, 0, 0, 0, 6, 'F', 'o', 'o', 'B', 'a', 'r'
    );

    test_data_bytes(foo_symbol_8,
        0xa3, 3, 'F', 'o', 'o'
    );
}

namespace t
{
    ByteArray::ByteArray() : bytes_(0), n_(0)
    {
    }

    ByteArray::ByteArray(const ByteArray& rhs) : n_(rhs.n_)
    {
        bytes_ = (unsigned char *) malloc(rhs.n_);
        memcpy(bytes_, rhs.bytes_, rhs.n_);
    }

    ByteArray::ByteArray(unsigned char * bytes, size_t n) : n_(n)
    {
        bytes_ = (unsigned char *) malloc(n);
        memcpy(bytes_, bytes, n);
    }

    ByteArray::~ByteArray()
    {
        free(bytes_);
    }

    ByteArray& ByteArray::operator += (const ByteArray& rhs)
    {
        unsigned char *t = (unsigned char *) malloc(n_ + rhs.n_);
        if (n_ > 0)
        {
            memcpy(t, bytes_, n_);
            free(bytes_);
        }
        memcpy(t + n_, rhs.bytes_, rhs.n_);
        n_ += rhs.n_;
        bytes_ = t;

        return *this;
    }

    void ByteArray::transfer_to(amqp_buffer_t *buffer) const
    {
        // TODO - throw exception
        amqp_buffer_puts(buffer, bytes_, n_);
    }

    void amqp_buffer_dump(amqp_buffer_t *buffer, int (*putc)(int))
    {
        size_t i;

        for (i = 0; i < buffer->limit.size; i++)
        {
            int c = buffer->bytes[i];
            (*putc)(to_hex(c >> 4));
            (*putc)(to_hex(c & 0x0f));
            if (i > 0 && (i & 0x0f) == 0)
            {
                (*putc)('\n');
            }
        }
    }

    void amqp_buffer_dump_as_byte_array(amqp_buffer_t *buffer, int (*putc)(int))
    {
        size_t i;

        (*putc)('{');
        for (i = 0; i < buffer->limit.size; i++)
        {
            int c = buffer->bytes[i];
            (*putc)('0');
            (*putc)('x');
            (*putc)(to_hex(c >> 4));
            (*putc)(to_hex(c & 0x0f));
            (*putc)(',');
            (*putc)(' ');
            if (i > 0 && (i & 0x0f) == 0)
            {
                (*putc)('\n');
            }
        }
        (*putc)('}');
        (*putc)('\n');
    }

}
