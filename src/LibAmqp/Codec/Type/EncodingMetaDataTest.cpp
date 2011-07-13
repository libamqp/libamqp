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

#include "Context/ContextTestSupport.h"

#include "Type.h"
#include "Codec/Type/EncodingMetaData.h"

extern amqp_encoding_meta_data_t *amqp__type_lookup_table[];
extern const int amqp__type_lookup_table_ntypes;

SUITE(TypeMetaData)
{
    class TypeMetaDataFixture : public SuiteContext::ContextFixture
    {
    public:
        TypeMetaDataFixture() {}
        ~TypeMetaDataFixture() {}

    public:
    };

    TEST(type_look_table_should_be_sorted_by_format_code)
    {
        unsigned char format_code = amqp__type_lookup_table[0]->format_code;

        for (int i = 0; i < amqp__type_lookup_table_ntypes; i++)
        {
            CHECK(amqp__type_lookup_table[i]->format_code >= format_code);
            format_code = amqp__type_lookup_table[i]->format_code;
        }
    }
    
    TEST_FIXTURE(TypeMetaDataFixture, lookup_invalid_codes)
    {
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x3e));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x3f));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x46));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x4e));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x57));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x5e));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x62));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x6e));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x63));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x6e));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x75));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x7e));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x75));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0x9e));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0xa4));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0xae));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0xb4));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0xbe));
        CHECK_NULL(amqp_type_meta_data_lookup(context, 0xc2));
    }

    // These were generated and protect against changes to the lookup messing up.
    TEST_FIXTURE(TypeMetaDataFixture, lookup_described_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x00);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x00, meta_data->format_code);
        CHECK_EQUAL(0, meta_data->width);
        CHECK_EQUAL("described", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);

    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_null_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x40);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x40, meta_data->format_code);
        CHECK_EQUAL(0, meta_data->width);
        CHECK_EQUAL("null", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_boolean_true)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x41);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x41, meta_data->format_code);
        CHECK_EQUAL(0, meta_data->width);
        CHECK_EQUAL("boolean", meta_data->name);
        CHECK_EQUAL("true", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_boolean_false)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x42);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x42, meta_data->format_code);
        CHECK_EQUAL(0, meta_data->width);
        CHECK_EQUAL("boolean", meta_data->name);
        CHECK_EQUAL("false", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_uint_zero)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x43);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x43, meta_data->format_code);
        CHECK_EQUAL(0, meta_data->width);
        CHECK_EQUAL("uint", meta_data->name);
        CHECK_EQUAL("uint0", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_ulong_zero)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x44);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x44, meta_data->format_code);
        CHECK_EQUAL(0, meta_data->width);
        CHECK_EQUAL("ulong", meta_data->name);
        CHECK_EQUAL("ulong0", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_ubyte_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x50);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x50, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("ubyte", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_byte_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x51);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x51, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("byte", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_small_uint)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x52);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x52, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("uint", meta_data->name);
        CHECK_EQUAL("smalluint", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_small_ulong)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x53);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x53, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("ulong", meta_data->name);
        CHECK_EQUAL("smallulong", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_small_int)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x54);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x54, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("int", meta_data->name);
        CHECK_EQUAL("smallint", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_small_long)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x55);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x55, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("long", meta_data->name);
        CHECK_EQUAL("smalllong", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_boolean)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x56);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x56, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("boolean", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_ushort_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x60);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x60, meta_data->format_code);
        CHECK_EQUAL(2, meta_data->width);
        CHECK_EQUAL("ushort", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_short_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x61);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x61, meta_data->format_code);
        CHECK_EQUAL(2, meta_data->width);
        CHECK_EQUAL("short", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_uint_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x70);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x70, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("uint", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_int_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x71);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x71, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("int", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_float_ieee_754)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x72);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x72, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("float", meta_data->name);
        CHECK_EQUAL("ieee-754", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_char_utf32)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x73);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x73, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("char", meta_data->name);
        CHECK_EQUAL("utf32", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_decimal32_ieee_754)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x74);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x74, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("decimal32", meta_data->name);
        CHECK_EQUAL("ieee-754", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_ulong_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x80);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x80, meta_data->format_code);
        CHECK_EQUAL(8, meta_data->width);
        CHECK_EQUAL("ulong", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_long_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x81);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x81, meta_data->format_code);
        CHECK_EQUAL(8, meta_data->width);
        CHECK_EQUAL("long", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_double_ieee_754)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x82);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x82, meta_data->format_code);
        CHECK_EQUAL(8, meta_data->width);
        CHECK_EQUAL("double", meta_data->name);
        CHECK_EQUAL("ieee-754", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_timestamp_ms64)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x83);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x83, meta_data->format_code);
        CHECK_EQUAL(8, meta_data->width);
        CHECK_EQUAL("timestamp", meta_data->name);
        CHECK_EQUAL("ms64", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_decimal64_ieee_754)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x84);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x84, meta_data->format_code);
        CHECK_EQUAL(8, meta_data->width);
        CHECK_EQUAL("decimal64", meta_data->name);
        CHECK_EQUAL("ieee-754", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_uuid_)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x98);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x98, meta_data->format_code);
        CHECK_EQUAL(16, meta_data->width);
        CHECK_EQUAL("uuid", meta_data->name);
        CHECK_EQUAL("", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_binary_vbin8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xa0);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xa0, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("binary", meta_data->name);
        CHECK_EQUAL("vbin8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_string_str8_utf8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xa1);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xa1, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("string", meta_data->name);
        CHECK_EQUAL("str8-utf8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_symbol_sym8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xa3);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xa3, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("symbol", meta_data->name);
        CHECK_EQUAL("sym8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_binary_vbin32)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xb0);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xb0, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("binary", meta_data->name);
        CHECK_EQUAL("vbin32", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_string_str32_utf8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xb1);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xb1, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("string", meta_data->name);
        CHECK_EQUAL("str32-utf8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_symbol_sym32)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xb3);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xb3, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("symbol", meta_data->name);
        CHECK_EQUAL("sym32", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_list_8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xc0);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xc0, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("list", meta_data->name);
        CHECK_EQUAL("list8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_map_8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xc1);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xc1, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("map", meta_data->name);
        CHECK_EQUAL("map8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_list_32)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xd0);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xd0, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("list", meta_data->name);
        CHECK_EQUAL("list32", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_map_32)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xd1);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xd1, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("map", meta_data->name);
        CHECK_EQUAL("map32", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_array_8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xe0);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xe0, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("array", meta_data->name);
        CHECK_EQUAL("array8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_array_32)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xf0);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xf0, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("array", meta_data->name);
        CHECK_EQUAL("array32", meta_data->encoding_name);
    }


    // extension types
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_fixed_0)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x4f);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x4f, meta_data->format_code);
        CHECK_EQUAL(0, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("fixed-0", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_fixed_1)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x5f);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x5f, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("fixed-1", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_fixed_2)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x6f);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x6f, meta_data->format_code);
        CHECK_EQUAL(2, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("fixed-2", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_fixed_4)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x7f);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x7f, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("fixed-4", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_fixed_8)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x8f);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x8f, meta_data->format_code);
        CHECK_EQUAL(8, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("fixed-8", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_fixed_16)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x9f);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0x9f, meta_data->format_code);
        CHECK_EQUAL(16, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("fixed-16", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_variable_1)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xaf);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xaf, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("variable-1", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_variable_4)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xbf);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xbf, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("variable-4", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_compound_1)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xcf);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xcf, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("compound-1", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_compound_4)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xdf);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xdf, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("compound-4", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_array_1)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xef);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xef, meta_data->format_code);
        CHECK_EQUAL(1, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("array-1", meta_data->encoding_name);
    }
    TEST_FIXTURE(TypeMetaDataFixture, lookup_extension_array_4)
    {
        amqp_encoding_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xff);
        CHECK_NOT_NULL(meta_data);
        CHECK_EQUAL(0xff, meta_data->format_code);
        CHECK_EQUAL(4, meta_data->width);
        CHECK_EQUAL("extension", meta_data->name);
        CHECK_EQUAL("array-4", meta_data->encoding_name);
    }
}