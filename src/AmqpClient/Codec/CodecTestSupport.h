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

#ifndef STORMMQ_AMQP_DECODE_TEST_SUPPORT_H
#define STORMMQ_AMQP_DECODE_TEST_SUPPORT_H

#include <string.h>

#include "Codec/Decode.h"
#include "Codec/Encode.h"
#include "Type/Type.h"

#include "Buffer/BufferTestSupport.h"
#include "Context/ContextTestSupport.h"


#define ASSERT_FLAG_SET(flag)   ASSERT_EQUAL(1U, flag)
#define ASSERT_FLAG_CLEAR(flag)   ASSERT_EQUAL(0U, flag)

namespace t
{
    void dump_type(amqp_type_t *type);
    void dump_type_buffer(amqp_type_t *type);

    int compare_buffers(const unsigned char *lhs, size_t lhs_size, const unsigned char *rhs, size_t rhs_size);
}

namespace test_data
{
    extern t::ByteArray bad_format_code;

    extern t::ByteArray bad_symbol;

    extern t::ByteArray float_4;
    extern t::ByteArray double_8;

    extern t::ByteArray varied_ulong_8;
    extern t::ByteArray ulong_8;
    extern t::ByteArray uint_4;
    extern t::ByteArray ulong_1;
    extern t::ByteArray uint_1;
    extern t::ByteArray ushort_2;
    extern t::ByteArray ubyte_1;
    extern t::ByteArray neg_ubyte_1;

    extern t::ByteArray long_8;
    extern t::ByteArray int_4;
    extern t::ByteArray long_1;
    extern t::ByteArray int_1;
    extern t::ByteArray short_2;
    extern t::ByteArray byte_1;

    extern t::ByteArray utf32_char;
    extern t::ByteArray timestamp_8;
    extern t::ByteArray timestamp_before_epoc_8;
    extern t::ByteArray uuid_16;

    extern t::ByteArray null_0;
    extern t::ByteArray true_0;
    extern t::ByteArray false_0;

    extern t::ByteArray array_shorts;
    extern t::ByteArray single_element_array;
    extern t::ByteArray map;
    extern t::ByteArray list_of_shorts;
    extern t::ByteArray list;

    extern t::ByteArray bin_8;
    extern t::ByteArray bin_32;

    extern t::ByteArray string_array;

    extern const char *lorem_ipsum;
    extern t::ByteArray encoded_lorem_ipsum;

    extern t::ByteArray described_list;
    extern t::ByteArray missing_descriptor;
    extern t::ByteArray missing_described_type;
    extern t::ByteArray invalid_descriptor;
    extern t::ByteArray invalid_described_type;

    extern t::ByteArray empty_map;
    extern t::ByteArray empty_list_8;

    extern t::ByteArray multiple_true_symbol_null;
    extern t::ByteArray multiple_true_symbol_one_value;
    extern t::ByteArray multiple_true_symbol_many_values;
}

#define ASSERT_VALID(type)  \
    ASSERT_NOT_NULL(type); \
    ASSERT(amqp_type_is_valid(type))

#define ASSERT_INVALID(type)  \
    ASSERT_NOT_NULL(type); \
    ASSERT(!amqp_type_is_valid(type))

#define ASSERT_BUFFERS_MATCH(context, byte_array) \
     ASSERT(t::compare_buffers(byte_array.bytes(), byte_array.size(), context->bytes, context->limit.size))

#define CHECK_ARRAY(type) \
    CHECK(amqp_type_is_compound(type)); \
    CHECK(amqp_type_is_array(type)); \
    CHECK(!amqp_type_is_map(type)); \
    CHECK(!amqp_type_is_list(type))

#define CHECK_LIST(type) \
    CHECK(amqp_type_is_compound(type)); \
    CHECK(!amqp_type_is_array(type)); \
    CHECK(!amqp_type_is_map(type)); \
    CHECK(amqp_type_is_list(type))

#define CHECK_MAP(type) \
    CHECK(amqp_type_is_compound(type)); \
    CHECK(!amqp_type_is_array(type)); \
    CHECK(amqp_type_is_map(type)); \
    CHECK(!amqp_type_is_list(type))

#endif

