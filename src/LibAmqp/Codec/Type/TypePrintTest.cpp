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

#include <TestHarness.h>

#include "Context/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Context/ContextTestSupport.h"
#include "Codec/Type/Type.h"

#include "Codec/Type/TypePrint.h"

#ifdef putc
#undef putc
#endif

SUITE(TypePrint)
{
    class TypePrintFixture;
    static TypePrintFixture *active_test;
    static int printc(int c);

    class TypePrintFixture : public SuiteContext::ContextFixture
    {

    public:
        TypePrintFixture() : type(0), count(0)
        {
            active_test = this;
            amqp_context_define_putc_function(context, printc);
        }
        ~TypePrintFixture() 
        {
            amqp_deallocate(context, &context->pools.amqp_type_t_pool, type);
        }
        int putc(int c)
        {
            if (count < sizeof(output))
            {
                output[count++] = c;
                output[count] = '\0';
            }
            return c;
        }
        void print_type()
        {
            amqp_type_print(type);
        }
    
        void print_type_formatted()
        {
            amqp_type_print_formatted(type);
        }

        void check_output(const char *expected)
        {
            CHECK_EQUAL(expected, (const char *) output);
        }

    public:
        amqp_type_t *type;

    private:
        char output[1024];
        size_t count;
    };

    static int printc(int c)
    {
        return active_test->putc(c);
    }

    TEST_FIXTURE(TypePrintFixture, utf8_string)
    {
        load_decode_buffer(test_data::utf8_string);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("utf8 string");
    }

    TEST_FIXTURE(TypePrintFixture, utf8_string_formatted)
    {
        load_decode_buffer(test_data::utf8_string);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type_formatted();
        check_output("string-str8-utf8: utf8 string;\n");
    }

    TEST_FIXTURE(TypePrintFixture, ubyte)
    {
        load_decode_buffer(test_data::neg_ubyte_1);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("195");
    }

    TEST_FIXTURE(TypePrintFixture, byte)
    {
        load_decode_buffer(test_data::byte_1);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("-2");
    }

    TEST_FIXTURE(TypePrintFixture, ushort)
    {
        load_decode_buffer(test_data::ushort_2);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("65534");
    }

    TEST_FIXTURE(TypePrintFixture, short)
    {
        load_decode_buffer(test_data::short_2);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("-2");
    }

    TEST_FIXTURE(TypePrintFixture, Float)
    {
        load_decode_buffer(test_data::float_4);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("123.455994");
    }

    TEST_FIXTURE(TypePrintFixture, Double)
    {
        load_decode_buffer(test_data::double_8);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("123.456000");
    }

    TEST_FIXTURE(TypePrintFixture, Symbol)
    {
        load_decode_buffer(test_data::foo_bar_symbol_32);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("FooBar");
    }

    TEST_FIXTURE(TypePrintFixture, PrintInvalidSymbol)
    {
        load_decode_buffer(test_data::bad_symbol);
        type = amqp_decode_supress_messages(context);

        ASSERT_INVALID(type);
        print_type();
        check_output("invalid: 42e172");
    }

    TEST_FIXTURE(TypePrintFixture, PrintInvalidSymbolFormatted)
    {
        load_decode_buffer(test_data::bad_symbol);
        type = amqp_decode_supress_messages(context);

        ASSERT_INVALID(type);
        print_type_formatted();
        check_output("symbol-sym8: invalid: 42e172;\n");
    }

    TEST_FIXTURE(TypePrintFixture, PrintUdid)
    {
        load_decode_buffer(test_data::uuid_16);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();
        check_output("f81d4fae-7dec-11d0-a765-00a0c91e6bf6");
    }

    TEST_FIXTURE(TypePrintFixture, PrintChar)
    {
        load_decode_buffer(test_data::utf32_char);
        type = amqp_decode(context);

        ASSERT_VALID(type);

        // TODO - print wchar_t characters
    }

    TEST_FIXTURE(TypePrintFixture, SmallArray)
    {
        load_decode_buffer(test_data::array_shorts);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        ASSERT_VALID(type->value.array.elements[0]);

        CHECK_EQUAL(0x61, type->value.array.elements[0]->format_code);
        print_type();
        check_output("[10, 11, 11, 13, 14]");
    }

    TEST_FIXTURE(TypePrintFixture, List)
    {
        load_decode_buffer(test_data::list);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_LIST(type);

        print_type();
        check_output("{Foo; 123.456000; Hello; 10; [10, 11]; [12]}");
    }

    TEST_FIXTURE(TypePrintFixture, empty_list)
    {
        load_decode_buffer(test_data::empty_list_8);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_LIST(type);

        print_type();
        check_output("{}");
    }

    TEST_FIXTURE(TypePrintFixture, empty_list_0)
    {
        load_decode_buffer(test_data::empty_list_0);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_LIST(type);

        print_type();
        check_output("{}");
    }

    TEST_FIXTURE(TypePrintFixture, Map)
    {
        load_decode_buffer(test_data::map);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_MAP(type);

        print_type();
        check_output("{{list:::{1; two; 3.141593; null; false}}, {null::true}, {pi::3.141593}, {two::2}, {129::129}}");
    }

    TEST_FIXTURE(TypePrintFixture, TimeStamp)
    {
        load_decode_buffer(test_data::timestamp_8);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();

        check_output("2010-12-06T17:00:00.000Z");
    }

    TEST_FIXTURE(TypePrintFixture, TimeStamp2)
    {
        load_decode_buffer(test_data::timestamp_before_epoc_8);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        print_type();

        check_output("1910-12-06T17:00:00.000Z");
    }

    TEST_FIXTURE(TypePrintFixture, BooleanTrue)
    {
        load_decode_buffer(test_data::true_0);
        type = amqp_decode(context);

        ASSERT_VALID(type);

        print_type();
        check_output("true");
    }

    TEST_FIXTURE(TypePrintFixture, BooleanFalse)
    {
        load_decode_buffer(test_data::false_0);
        type = amqp_decode(context);

        ASSERT_VALID(type);

        print_type();
        check_output("false");
    }

    TEST_FIXTURE(TypePrintFixture, BooleanOneByteEncodingTrueValue)
    {
        load_decode_buffer(test_data::true_1);
        type = amqp_decode(context);

        ASSERT_VALID(type);

        print_type();
        check_output("false");
    }

    TEST_FIXTURE(TypePrintFixture, BooleanOneByteEncodingFalseValue)
    {
        load_decode_buffer(test_data::false_1);
        type = amqp_decode(context);

        ASSERT_VALID(type);

        print_type();
        check_output("true");
    }
}
