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

#include <TestHarness.h>
#include "Context/ErrorHandling.h"

#include "Codec/Decode/DecodeTestFixture.h"

#include "Codec/Type/Type.h"

SUITE(CodecDecode)
{
    using test_data::TestData;
    
    TEST_FIXTURE(DecodeTestFixture, EndOfBuffer)
    {
        load_decode_buffer(test_data::empty_buffer);
        type = amqp_decode(context, decode_buffer);
        CHECK_NULL(type);
    }

    TEST_FIXTURE(DecodeTestFixture, BadFormatCode)
    {
        load_decode_buffer(test_data::bad_format_code);
        type = amqp_decode_supress_messages(context, decode_buffer);
        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_UNKNOWN_FORMAT_CODE, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, BadFixedFormatCode)
    {
        test_data_def(bad_data, 
            0x46,
        );

        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_UNKNOWN_FORMAT_CODE, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, InvalidFormatCode)
    {
        test_data_def(bad_data,
            0x3f,
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_UNKNOWN_FORMAT_CODE, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadULong)
    {
        test_data_def(bad_data, 
            0x80, 0x10, 0x07, 0x03, 0x01
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadUInt)
    {
        test_data_def(bad_data, 
            0x70, 0xff, 0xff
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadUShort)
    {
        test_data_def(bad_data, 
            0x60, 0xff,
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadUByte)
    {
        test_data_def(bad_data, 
            0x50
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadLong)
    {
        test_data_def(bad_data, 
            0x81, 0x10, 0x07, 0x03, 0x01
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadInt)
    {
        test_data_def(bad_data, 
            0x71, 0xff, 0xff
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadShort)
    {
        test_data_def(bad_data, 
            0x61, 0xff,
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadByte)
    {
        test_data_def(bad_data, 
            0x51
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadSymbol)
    {
        test_data_def(bad_data, 
            0xa3, 3, 'F', 'o'
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadLargeSymbol)
    {
        test_data_def(bad_data, 
            0xb3, 0, 0, 0, 6, 'F', 'o', 'o', 'B', 'a'
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, BadFloat)
    {
        test_data_def(bad_data, 
            0x72, 0x42, 0xf6, 0xe9
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, BadDouble)
    {
        test_data_def(bad_data, 
            0x82, 0x42, 0xf6, 0xe9
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeBadString)
    {
        test_data_def(bad_data, 
            0xA1, 0x0B, 'B', 'a', 'd', ' ', 'W', 'o', 'r', 'l', 'd'
        );
        load_decode_buffer(bad_data);
        type = amqp_decode_supress_messages(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK(amqp_type_is_invalid(type));
        CHECK_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
    }
}