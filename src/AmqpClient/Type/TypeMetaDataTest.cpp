#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Type.h"
#include "Type/TypeMetaData.h"

namespace
{
    amqp_context_t *context;
    
    void SetUp()
    {   
        context = AMQP_NEW(amqp_context_t);
    }

    void TearDown()
    {
        AMQP_DELETE(amqp_context_t, context);
    }
}

TEST(TypeMetaData, lookup_invalid_codes)
{
    CHECK_NULL(amqp_type_meta_data_lookup(context, 0x3e));
    CHECK_NULL(amqp_type_meta_data_lookup(context, 0x3f));
    CHECK_NULL(amqp_type_meta_data_lookup(context, 0x43));
    CHECK_NULL(amqp_type_meta_data_lookup(context, 0x4e));
    CHECK_NULL(amqp_type_meta_data_lookup(context, 0x56));
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
TEST(TypeMetaData, lookup_described_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x00);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x00, meta_data->format_code);
    CHECK_EQUAL(0, meta_data->width);
    CHECK_EQUAL("described", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);

}
TEST(TypeMetaData, lookup_null_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x40);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x40, meta_data->format_code);
    CHECK_EQUAL(0, meta_data->width);
    CHECK_EQUAL("null", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_boolean_true)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x41);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x41, meta_data->format_code);
    CHECK_EQUAL(0, meta_data->width);
    CHECK_EQUAL("boolean", meta_data->name);
    CHECK_EQUAL("true", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_boolean_false)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x42);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x42, meta_data->format_code);
    CHECK_EQUAL(0, meta_data->width);
    CHECK_EQUAL("boolean", meta_data->name);
    CHECK_EQUAL("false", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_ubyte_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x50);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x50, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("ubyte", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_byte_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x51);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x51, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("byte", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_ushort_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x60);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x60, meta_data->format_code);
    CHECK_EQUAL(2, meta_data->width);
    CHECK_EQUAL("ushort", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_short_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x61);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x61, meta_data->format_code);
    CHECK_EQUAL(2, meta_data->width);
    CHECK_EQUAL("short", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_uint_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x70);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x70, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("uint", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_int_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x71);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x71, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("int", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_float_ieee_754)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x72);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x72, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("float", meta_data->name);
    CHECK_EQUAL("ieee-754", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_char_utf32)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x73);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x73, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("char", meta_data->name);
    CHECK_EQUAL("utf32", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_decimal32_ieee_754)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x74);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x74, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("decimal32", meta_data->name);
    CHECK_EQUAL("ieee-754", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_ulong_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x80);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x80, meta_data->format_code);
    CHECK_EQUAL(8, meta_data->width);
    CHECK_EQUAL("ulong", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_long_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x81);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x81, meta_data->format_code);
    CHECK_EQUAL(8, meta_data->width);
    CHECK_EQUAL("long", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_double_ieee_754)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x82);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x82, meta_data->format_code);
    CHECK_EQUAL(8, meta_data->width);
    CHECK_EQUAL("double", meta_data->name);
    CHECK_EQUAL("ieee-754", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_timestamp_ms64)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x83);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x83, meta_data->format_code);
    CHECK_EQUAL(8, meta_data->width);
    CHECK_EQUAL("timestamp", meta_data->name);
    CHECK_EQUAL("ms64", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_decimal64_ieee_754)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x84);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x84, meta_data->format_code);
    CHECK_EQUAL(8, meta_data->width);
    CHECK_EQUAL("decimal64", meta_data->name);
    CHECK_EQUAL("ieee-754", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_uuid_)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x98);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x98, meta_data->format_code);
    CHECK_EQUAL(16, meta_data->width);
    CHECK_EQUAL("uuid", meta_data->name);
    CHECK_EQUAL("", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_binary_vbin8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xa0);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xa0, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("binary", meta_data->name);
    CHECK_EQUAL("vbin8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_string_str8_utf8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xa1);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xa1, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("string", meta_data->name);
    CHECK_EQUAL("str8-utf8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_string_str8_utf16)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xa2);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xa2, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("string", meta_data->name);
    CHECK_EQUAL("str8-utf16", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_symbol_sym8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xa3);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xa3, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("symbol", meta_data->name);
    CHECK_EQUAL("sym8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_binary_vbin32)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xb0);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xb0, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("binary", meta_data->name);
    CHECK_EQUAL("vbin32", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_string_str32_utf8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xb1);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xb1, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("string", meta_data->name);
    CHECK_EQUAL("str32-utf8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_string_str32_utf16)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xb2);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xb2, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("string", meta_data->name);
    CHECK_EQUAL("str32-utf16", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_symbol_sym32)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xb3);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xb3, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("symbol", meta_data->name);
    CHECK_EQUAL("sym32", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_list_8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xc0);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xc0, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("list", meta_data->name);
    CHECK_EQUAL("list8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_map_8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xc1);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xc1, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("map", meta_data->name);
    CHECK_EQUAL("map8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_list_32)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xd0);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xd0, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("list", meta_data->name);
    CHECK_EQUAL("list32", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_map_32)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xd1);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xd1, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("map", meta_data->name);
    CHECK_EQUAL("map32", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_array_8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xe0);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xe0, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("list", meta_data->name);
    CHECK_EQUAL("array8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_array_32)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xf0);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xf0, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("list", meta_data->name);
    CHECK_EQUAL("array32", meta_data->encoding_name);
}


// extension types
TEST(TypeMetaData, lookup_extension_fixed_0)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x4f);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x4f, meta_data->format_code);
    CHECK_EQUAL(0, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("fixed-0", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_fixed_1)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x5f);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x5f, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("fixed-1", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_fixed_2)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x6f);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x6f, meta_data->format_code);
    CHECK_EQUAL(2, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("fixed-2", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_fixed_4)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x7f);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x7f, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("fixed-4", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_fixed_8)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x8f);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x8f, meta_data->format_code);
    CHECK_EQUAL(8, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("fixed-8", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_fixed_16)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0x9f);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0x9f, meta_data->format_code);
    CHECK_EQUAL(16, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("fixed-16", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_variable_1)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xaf);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xaf, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("variable-1", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_variable_4)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xbf);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xbf, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("variable-4", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_compound_1)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xcf);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xcf, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("compound-1", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_compound_4)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xdf);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xdf, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("compound-4", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_array_1)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xef);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xef, meta_data->format_code);
    CHECK_EQUAL(1, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("array-1", meta_data->encoding_name);
}
TEST(TypeMetaData, lookup_extension_array_4)
{
    amqp_type_meta_data_t *meta_data = amqp_type_meta_data_lookup(context, 0xff);
    ASSERT_NOT_NULL(meta_data);
    CHECK_EQUAL(0xff, meta_data->format_code);
    CHECK_EQUAL(4, meta_data->width);
    CHECK_EQUAL("extension", meta_data->name);
    CHECK_EQUAL("array-4", meta_data->encoding_name);
}

