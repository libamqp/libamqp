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

#include "Buffer/Buffer.h"
#include "TestData/TestData.h"

namespace test_data
{
    TestData::TestData(unsigned char * bytes, size_t n) : n_(n)
    {
        bytes_ = (unsigned char *) malloc(n);
        memcpy(bytes_, bytes, n);
    }

    TestData::~TestData()
    {
        free(bytes_);
    }

    void TestData::transfer_to(amqp_buffer_t *buffer) const
    {
        // TODO - throw exception
        amqp_buffer_puts(buffer, bytes_, n_);
    }
}

namespace test_data
{
    static unsigned char empty_buffer_bytes[0];
    TestData empty_buffer(empty_buffer_bytes, 0);

    test_data_def(hello_world, 0xA1,
        0x0B,
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'
    );

    test_data_def(utf8_string, 0xA1,
        11,
        'u', 't', 'f', '8', ' ', 's', 't', 'r', 'i', 'n', 'g'
    );

    test_data_def(hello_big_world, 0xb1,
        0, 0, 0, 0x0f,
        'H', 'e', 'l', 'l', 'o', ' ', 'B', 'i', 'g', ' ', 'W', 'o', 'r', 'l', 'd'
    );

    test_data_def(foo_bar_symbol_32,
        0xb3, 0, 0, 0, 6, 'F', 'o', 'o', 'B', 'a', 'r'
    );

    test_data_def(foo_symbol_8,
        0xa3, 3, 'F', 'o', 'o'
    );

    test_data_def(bad_symbol,
        0xa3, 3, 'B', '\xe1', 'r'
    );

    // 123.456
    test_data_def(float_4,
        0x72,
        0x42, 0xf6, 0xe9, 0x78
    );

    // 123.456
    test_data_def(double_8,
        0x82,
        0x40, 0x5E, 0xDD, 0x2F, 0x1A, 0x9F, 0xBE, 0x77
    );

    test_data_def(bad_format_code,
        0xb7, 0, 0, 0, 6, 'F', 'o', 'o', 'B', 'a', 'r'
    );

    // 1154895135052768677
    test_data_def(varied_ulong_8,
        0x80,
        0x10, 0x07, 0x03, 0x01, 0xa5, 0xa5, 0xa5, 0xa5
    );

    // 18446744073709551614
    test_data_def(ulong_8,
        0x80,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe
    );

    test_data_def(uint_4,
        0x70,
        0xff, 0xff, 0xff, 0xfe
    );

    test_data_def(ulong_small,
        0x53,
        0xfe
    );

    test_data_def(ulong_small_zero,
        0x53,
        0
    );

    test_data_def(uint_small,
        0x52,
        0xfe
    );


    test_data_def(uint_small_zero,
        0x52,
        0
    );

    test_data_def(uint_zero,
        0x43,
    );

    test_data_def(ulong_zero,
        0x44,
    );

    test_data_def(ushort_2,
        0x60,
        0xff, 0xfe
    );

    test_data_def(ubyte_1,
        0x50,
        0xfe
    );

    test_data_def(neg_ubyte_1,
        0x50, 0xc3
    );

    test_data_def(long_8,
        0x81,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe
    );

    // -129
    test_data_def(long_8_minus_129,
        0x81,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f
    );

    test_data_def(int_4,
        0x71,
        0xff, 0xff, 0xff, 0xfe
    );

    // - 129
    test_data_def(int_4_minus_129,
        0x71,
        0xff, 0xff, 0xff, 0x7f
    );

    // -4
    test_data_def(long_small,
        0x55,
        0xfc
    );

    test_data_def(long_small_zero,
        0x55,
        0
    );

    test_data_def(int_1,
        0x54,
        0xfd
    );

    test_data_def(int_1_zero,
        0x54,
        0
    );

    test_data_def(short_2,
        0x61,
        0xff, 0xfe
    );

    test_data_def(byte_1,
        0x51, 0xfe
    );

    test_data_def(utf32_char,
        0x73, 0x00, 0x00, 0x20, 0xAC
    );

    test_data_def(timestamp_8,
        0x83, 0x00, 0x00, 0x01, 0x2C, 0xBC, 0xA0, 0x22, 0x80
    );

    test_data_def(timestamp_before_epoc_8,
        0x83, 0xFF, 0xFF, 0xFE, 0x4D, 0xFA, 0xAB, 0xF6, 0x80
    );

    test_data_def(uuid_16,
        0x98,
        0xf8, 0x1d, 0x4f, 0xae, 0x7d, 0xec, 0x11, 0xd0, 0xa7, 0x65, 0x00, 0xa0, 0xc9, 0x1e, 0x6b, 0xf6
    );

    test_data_def(null_0,
        0x40
    );

    test_data_def(true_0,
        0x41
    );

    test_data_def(false_0,
        0x42
    );

    test_data_def(true_1,
        0x56, 0x01
    );

    test_data_def(false_1,
        0x56, 0x00
    );

    test_data_def(array_shorts,
        0xe0,
        0x0c,           // size
        0x05,           // count
        0x61,           // member constructor
        0x00, 0x0a,
        0x00, 0x0b,
        0x00, 0x0b,
        0x00, 0x0d,
        0x00, 0x0e,
    );

    test_data_def(single_element_array,
        0xe0,
        0x04,           // size
        0x01,           // count
        0x61,           // member constructor
        0x00, 0x0a,
    );

    test_data_def(map,
        0xc1, 0x41, 0x0a,
        0xa1, 0x05, 0x6c, 0x69, 0x73, 0x74, 0x3a,
        0xc0, 0x13, 0x05,
            0x55, 0x01,
            0xa1, 0x03, 0x74, 0x77, 0x6f,
            0x82, 0x40, 0x09, 0x21, 0xfb, 0x82, 0xc2, 0xbd, 0x7f,
            0x40,
            0x42,

        0x40,
        0x41,

        0xa1, 0x02, 0x70, 0x69,
        0x82, 0x40, 0x09, 0x21, 0xfb, 0x82, 0xc2, 0xbd, 0x7f,

        0xa1, 0x03, 0x74, 0x77, 0x6f,
        0x55, 0x02,

        0xa1, 0x03, 0x31, 0x32, 0x39,
        0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81
    );

    test_data_def(list_of_shorts,
        0xc0,
        0x10,           // size
        0x05,           // count
        0x61, 0x00, 0x0a,
        0x61, 0x00, 0x0b,
        0x61, 0x00, 0x0b,
        0x61, 0x00, 0x0d,
        0x61, 0x00, 0x0e,
    );

    test_data_def(list,
        0xd0,
        0, 0, 0, 0x2a,
        0, 0, 0, 6,

        // symbol
        0xa3, 3, 'F', 'o', 'o',

        // double
        0x82, 0x40, 0x5E, 0xDD, 0x2F, 0x1A, 0x9F, 0xBE, 0x77,

        // utf8 string
        0xA1, 0x05, 'H', 'e', 'l', 'l', 'o',

        // short
        0x61, 0x00, 0x0a,

        // array of two shorts
        0xe0, 6, 0x02, 0x61, 0x00, 0x0a, 0x00, 0x0b,

        // array of one short
        0xe0, 4, 1, 0x61, 0x00, 0x0c
    );

    test_data_def(bin_8, 0xA0,
        12,
        'b', 'i', 'n', 'a', 'r', 'y', ' ', 'a', 'r', 'r', 'a', 'y'
    );

    test_data_def(bin_32, 0xb0,
        0, 0, 0, 16,
        'b', 'i', 'g', ' ', 'b', 'i', 'n', 'a', 'r', 'y', ' ', 'a', 'r', 'r', 'a', 'y'
    );

    // TODO - add a decode test for this
    test_data_def(string_array,
        0xe0,
        0x17, 0x04,
        0xa1,
        0x03, 0x74, 0x68, 0x65,
        0x06, 0x6c, 0x69, 0x74, 0x74, 0x6c, 0x65,
        0x05, 0x62, 0x72, 0x6f, 0x77, 0x6e,
        0x03, 0x66, 0x6f, 0x78
    );

    const char *lorem_ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse sed nibh dolor, eget blandit risus. Sed vulputate neque sed sapien porttitor accumsan eget non lectus. Ut nunc dui, dignissim at eleifend eget, pellentesque vel ipsum. Etiam placerat amet.";

    test_data_def(encoded_lorem_ipsum,
        0xb1,
        0x00, 0x00, 0x01, 0x01,
        0x4c, 0x6f, 0x72, 0x65, 0x6d, 0x20, 0x69, 0x70, 0x73, 0x75, 0x6d, 0x20,
        0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x73, 0x69, 0x74, 0x20, 0x61, 0x6d, 0x65, 0x74, 0x2c, 0x20,
        0x63, 0x6f, 0x6e, 0x73, 0x65, 0x63, 0x74, 0x65, 0x74, 0x75, 0x72, 0x20, 0x61, 0x64, 0x69, 0x70,
        0x69, 0x73, 0x63, 0x69, 0x6e, 0x67, 0x20, 0x65, 0x6c, 0x69, 0x74, 0x2e, 0x20, 0x53, 0x75, 0x73,
        0x70, 0x65, 0x6e, 0x64, 0x69, 0x73, 0x73, 0x65, 0x20, 0x73, 0x65, 0x64, 0x20, 0x6e, 0x69, 0x62,
        0x68, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x2c, 0x20, 0x65, 0x67, 0x65, 0x74, 0x20, 0x62, 0x6c,
        0x61, 0x6e, 0x64, 0x69, 0x74, 0x20, 0x72, 0x69, 0x73, 0x75, 0x73, 0x2e, 0x20, 0x53, 0x65, 0x64,
        0x20, 0x76, 0x75, 0x6c, 0x70, 0x75, 0x74, 0x61, 0x74, 0x65, 0x20, 0x6e, 0x65, 0x71, 0x75, 0x65,
        0x20, 0x73, 0x65, 0x64, 0x20, 0x73, 0x61, 0x70, 0x69, 0x65, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74,
        0x74, 0x69, 0x74, 0x6f, 0x72, 0x20, 0x61, 0x63, 0x63, 0x75, 0x6d, 0x73, 0x61, 0x6e, 0x20, 0x65,
        0x67, 0x65, 0x74, 0x20, 0x6e, 0x6f, 0x6e, 0x20, 0x6c, 0x65, 0x63, 0x74, 0x75, 0x73, 0x2e, 0x20,
        0x55, 0x74, 0x20, 0x6e, 0x75, 0x6e, 0x63, 0x20, 0x64, 0x75, 0x69, 0x2c, 0x20, 0x64, 0x69, 0x67,
        0x6e, 0x69, 0x73, 0x73, 0x69, 0x6d, 0x20, 0x61, 0x74, 0x20, 0x65, 0x6c, 0x65, 0x69, 0x66, 0x65,
        0x6e, 0x64, 0x20, 0x65, 0x67, 0x65, 0x74, 0x2c, 0x20, 0x70, 0x65, 0x6c, 0x6c, 0x65, 0x6e, 0x74,
        0x65, 0x73, 0x71, 0x75, 0x65, 0x20, 0x76, 0x65, 0x6c, 0x20, 0x69, 0x70, 0x73, 0x75, 0x6d, 0x2e,
        0x20, 0x45, 0x74, 0x69, 0x61, 0x6d, 0x20, 0x70, 0x6c, 0x61, 0x63, 0x65, 0x72, 0x61, 0x74, 0x20,
        0x61, 0x6d, 0x65, 0x74, 0x2e,
    );

    test_data_def(described_list,
        0x00, 0xa1, 0x04, 'L', 'i', 's', 't', 0xc0,
        0x17,
        0x03,
        0xa1, 0x03, 'P', 'i', 'e',    // string
        0x00, 0xa1, 0x02, 'P', 'I',
            0x82, 0x40, 0x09, 0x21, 0xfb, 0x82, 0xc2, 0xbd, 0x7f,
        0x61, 0x00, 0x0a,       // short
    );

    test_data_def(missing_descriptor,
        0x00
    );

    test_data_def(missing_described_type,
        0x00, 0xa1, 0x04, 'L', 'i', 's', 't'
    );

    test_data_def(invalid_descriptor,
        0x00, 0xa1, 0x04, 'L', 'i', 's'
    );

    test_data_def(invalid_described_type,
        0x00, 0xa1, 0x04, 'L', 'i', 's', 't', 0xc0,
        0x17,
    );

    test_data_def(empty_map,
        0xc1, 0x01, 0x00,
    );

    test_data_def(empty_list_8,
        0xc0, 0x01, 0x00,
    );

    test_data_def(boolean_array,
        0xe0,
        0x04,           // size
        0x02,           // count
        0x56,           // member constructor
        0x01,           // true
        0x00,           // false
    );

    // TODO - add decode test for this
    test_data_def(multiple_true_symbol_null,
        0x00, 0x41, 0x40,
    );

    // TODO - add decode test for this
    test_data_def(multiple_true_symbol_one_value,
        0x00, 0x41, 0xa3, 0x03, 0x46, 0x6f, 0x6f,
    );

    // TODO - add decode test for this
    test_data_def(multiple_true_symbol_many_valuesx,
        0x00, 0x41,
            0xd0, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x02,
                0xa3, 0x03, 0x46, 0x6f, 0x6f,
                0xa3, 0x03, 0x46, 0x75, 0x6d,
    );
    // TODO - add decode test for this
    test_data_def(multiple_true_symbol_many_values,
        0x00, 0x41,
            0xc0, 0x0b, 0x02,
                0xa3, 0x03, 0x46, 0x6f, 0x6f,
                0xa3, 0x03, 0x46, 0x75, 0x6d,
    );
}

