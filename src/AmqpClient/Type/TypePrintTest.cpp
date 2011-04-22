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

#include <stdio.h>

#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Type/Type.h"

#include "Type/TypePrint.h"

namespace
{
    amqp_type_t *type;
    char output[1024];
    char *p;

    int putc(int c)
    {
        return *p++ = c;
    }

    t::Context *c;

    void SetUp()
    {
        c = new t::Context();
        amqp_context_define_putc_function(c->context(), putc);
        type = 0;
        p = output;
    }

    void TearDown()
    {
        AMQP_DEALLOC(amqp_type_t, c->context(), type);
        delete c;
    }

    void print_type()
    {
        amqp_type_print(type);
        putc('\0');
    }

    void print_type_formatted()
    {
        amqp_type_print_formatted(type);
        putc('\0');
    }
}

TEST(TypePrint, utf8_string)
{
    c->transfer_from(test_data::utf8_string);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("utf8 string", output);
}

TEST(TypePrint, utf8_string_formatted)
{
    c->transfer_from(test_data::utf8_string);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type_formatted();
    ASSERT_EQUAL("string-str8-utf8: utf8 string;\n", output);
}

TEST(TypePrint, ubyte)
{
    c->transfer_from(test_data::neg_ubyte_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("195",  output);
}

TEST(TypePrint, byte)
{
    c->transfer_from(test_data::byte_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("-2",  output);
}

TEST(TypePrint, ushort)
{
    c->transfer_from(test_data::ushort_2);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("65534",  output);
}

TEST(TypePrint, short)
{
    c->transfer_from(test_data::short_2);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("-2",  output);
}

TEST(TypePrint, Float)
{
    c->transfer_from(test_data::float_4);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("123.455994",  output);
}

TEST(TypePrint, Double)
{
    c->transfer_from(test_data::double_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("123.456000",  output);
}

TEST(TypePrint, Symbol)
{
    c->transfer_from(test_data::foo_bar_symbol_32);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("FooBar",  output);
}

TEST(TypePrint, PrintInvalidSymbol)
{
    c->transfer_from(test_data::bad_symbol);
    type = amqp_decode_supress_messages(c->context());

    ASSERT_INVALID(type);
    print_type();
    ASSERT_EQUAL("invalid: 42e172", output);
}

TEST(TypePrint, PrintInvalidSymbolFormatted)
{
    c->transfer_from(test_data::bad_symbol);
    type = amqp_decode_supress_messages(c->context());

    ASSERT_INVALID(type);
    print_type_formatted();
    ASSERT_EQUAL("symbol-sym8: invalid: 42e172;\n", output);
}

TEST(TypePrint, PrintUdid)
{
    c->transfer_from(test_data::uuid_16);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();
    ASSERT_EQUAL("f81d4fae-7dec-11d0-a765-00a0c91e6bf6", output);
}

TEST(TypePrint, PrintChar)
{
    c->transfer_from(test_data::utf32_char);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);

    // TODO - print wchar_t characters
}

TEST(TypePrint, SmallArray)
{
    c->transfer_from(test_data::array_shorts);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_VALID(type->value.array.elements[0]);

    CHECK_EQUAL(0x61, type->value.array.elements[0]->format_code);
    print_type();
    ASSERT_EQUAL("[10, 11, 11, 13, 14]", output);
}

TEST(TypePrint, List)
{
    c->transfer_from(test_data::list);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    CHECK_LIST(type);

    print_type();
    ASSERT_EQUAL("{Foo; 123.456000; Hello; 10; [10, 11]; [12]}", output);

}

TEST(TypePrint, Map)
{
    c->transfer_from(test_data::map);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    CHECK_MAP(type);

    print_type();
    ASSERT_EQUAL("{{list:::{1; two; 3.141593; null; false}}, {null::true}, {pi::3.141593}, {two::2}, {one::1}}", output);
}

TEST(TypePrint, TimeStamp)
{
    c->transfer_from(test_data::timestamp_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();

    ASSERT_EQUAL("2010-12-06T17:00:00.000Z", output);
}

TEST(TypePrint, TimeStamp2)
{
    c->transfer_from(test_data::timestamp_before_epoc_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    print_type();

    ASSERT_EQUAL("1910-12-06T17:00:00.000Z", output);
}
