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

#ifndef LIBAMQP_TEST_DATA_TEST_DATA_H
#define LIBAMQP_TEST_DATA_TEST_DATA_H

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#define test_data_def(name, ...) \
    static unsigned char name ## _bytes[] = { __VA_ARGS__ }; \
    TestData name(name ## _bytes, sizeof(name ## _bytes));

namespace test_data
{
    class TestData
    {
    public:
        TestData(unsigned char *, size_t n);
        ~TestData();

        const unsigned char *bytes() const { return bytes_; }
        size_t size() const { return n_; }

        void transfer_to(amqp_buffer_t *buffer) const;

    private:
        unsigned char *bytes_;
        size_t n_;
    };
}

namespace t
{
    extern void amqp_buffer_dump(amqp_buffer_t *buffer, int (*putc)(int));
    extern void amqp_buffer_dump_as_byte_array(amqp_buffer_t *buffer, int (*putc)(int));
}

namespace test_data
{
    extern TestData empty_buffer;

    extern TestData hello_world;
    extern TestData utf8_string;

    extern TestData hello_big_world;
    extern TestData foo_bar_symbol_32;
    extern TestData foo_symbol_8;

    extern TestData bad_format_code;

    extern TestData bad_symbol;

    extern TestData float_4;
    extern TestData double_8;

    extern TestData varied_ulong_8;
    extern TestData ulong_8;
    extern TestData ulong_zero;
    extern TestData ulong_small;
    extern TestData ulong_small_zero;
    extern TestData uint_4;
    extern TestData uint_small;
    extern TestData uint_small_zero;
    extern TestData uint_zero;
    extern TestData ushort_2;
    extern TestData ubyte_1;
    extern TestData neg_ubyte_1;

    extern TestData long_8;
    extern TestData long_8_just;
    extern TestData int_4;
    extern TestData long_small;
    extern TestData long_small_zero;
    extern TestData int_1;
    extern TestData short_2;
    extern TestData byte_1;

    extern TestData utf32_char;
    extern TestData timestamp_8;
    extern TestData timestamp_before_epoc_8;
    extern TestData uuid_16;

    extern TestData null_0;
    extern TestData true_0;
    extern TestData false_0;

    extern TestData array_shorts;
    extern TestData single_element_array;
    extern TestData map;
    extern TestData list_of_shorts;
    extern TestData list;

    extern TestData bin_8;
    extern TestData bin_32;

    extern TestData string_array;

    extern const char *lorem_ipsum;
    extern TestData encoded_lorem_ipsum;

    extern TestData described_list;
    extern TestData missing_descriptor;
    extern TestData missing_described_type;
    extern TestData invalid_descriptor;
    extern TestData invalid_described_type;

    extern TestData empty_map;
    extern TestData empty_list_8;

    extern TestData multiple_true_symbol_null;
    extern TestData multiple_true_symbol_one_value;
    extern TestData multiple_true_symbol_many_values;
}

#endif

