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

#ifndef STORMMQ_AMQP_BUFFER_TEST_SUPPORT_H
#define STORMMQ_AMQP_BUFFER_TEST_SUPPORT_H

#include "Buffer/Buffer.h"

namespace t
{
    class ByteArray
    {
    public:
        ByteArray();
        ByteArray(const ByteArray&);
        ByteArray(unsigned char *, size_t n);

        ~ByteArray();

        const unsigned char *bytes() const { return bytes_; }
        size_t size() const { return n_; }

        ByteArray& operator += (const ByteArray& rhs);

        void transfer_to(amqp_buffer_t *buffer) const;

    private:
        unsigned char *bytes_;
        size_t n_;
    };

    extern void amqp_buffer_dump(amqp_buffer_t *buffer, int (*putc)(int));
    extern void amqp_buffer_dump_as_byte_array(amqp_buffer_t *buffer, int (*putc)(int));
}

#define test_data_bytes(name, ...) \
    unsigned char name ## _bytes[] = { __VA_ARGS__ }; \
    t::ByteArray name(name ## _bytes, sizeof(name ## _bytes))

namespace test_data
{
    extern t::ByteArray empty_buffer;

    extern t::ByteArray hello_world;
    extern t::ByteArray utf8_string;

    extern t::ByteArray hello_big_world;
    extern t::ByteArray foo_bar_symbol_32;
    extern t::ByteArray foo_symbol_8;
}

#endif

