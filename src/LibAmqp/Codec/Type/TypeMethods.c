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

#include "Codec/Type/TypeMethods.h"
#include "Codec/Type/TypePrint.h"

// :s/^\(.*amqp_methods_\)\([^ ]*\) = {/\1\2 = { /
amqp_type_methods_t amqp_methods_described = { amqp_type_method_described_print };
amqp_type_methods_t amqp_methods_null = { amqp_type_method_null_print };
amqp_type_methods_t amqp_methods_boolean_true = { amqp_type_method_boolean_true_print };
amqp_type_methods_t amqp_methods_boolean_false = { amqp_type_method_boolean_false_print };
amqp_type_methods_t amqp_methods_boolean = { amqp_type_method_boolean_print };
amqp_type_methods_t amqp_methods_ubyte = { amqp_type_method_ubyte_print };
amqp_type_methods_t amqp_methods_byte = { amqp_type_method_byte_print };
amqp_type_methods_t amqp_methods_ushort = { amqp_type_method_ushort_print };
amqp_type_methods_t amqp_methods_short = { amqp_type_method_short_print };
amqp_type_methods_t amqp_methods_uint = { amqp_type_method_uint_print };
amqp_type_methods_t amqp_methods_int = { amqp_type_method_int_print };

amqp_type_methods_t amqp_methods_float = { amqp_type_method_float_print };
amqp_type_methods_t amqp_methods_char = { amqp_type_method_char_print };

amqp_type_methods_t amqp_methods_decimal32 = { amqp_type_method_decimal32_print };
amqp_type_methods_t amqp_methods_decimal64 = { amqp_type_method_decimal64_print };
amqp_type_methods_t amqp_methods_decimal128 = { amqp_type_method_decimal128_print };

amqp_type_methods_t amqp_methods_ulong = { amqp_type_method_ulong_print };
amqp_type_methods_t amqp_methods_long = { amqp_type_method_long_print };

amqp_type_methods_t amqp_methods_double = { amqp_type_method_double_print };
amqp_type_methods_t amqp_methods_timestamp = { amqp_type_method_timestamp_print };
amqp_type_methods_t amqp_methods_uuid = { amqp_type_method_uuid_print };

amqp_type_methods_t amqp_methods_binary_vbin8 = { amqp_type_method_binary_vbin8_print };
amqp_type_methods_t amqp_methods_string_str8_utf8 = { amqp_type_method_string_str8_utf8_print };
amqp_type_methods_t amqp_methods_symbol_sym8 = { amqp_type_method_symbol_sym8_print };
amqp_type_methods_t amqp_methods_binary_vbin32 = { amqp_type_method_binary_vbin32_print };
amqp_type_methods_t amqp_methods_string_str32_utf8 = { amqp_type_method_string_str32_utf8_print };
amqp_type_methods_t amqp_methods_symbol_sym32 = { amqp_type_method_symbol_sym32_print };

amqp_type_methods_t amqp_methods_list_0 = { amqp_type_method_list_print };
amqp_type_methods_t amqp_methods_list_8 = { amqp_type_method_list_print };
amqp_type_methods_t amqp_methods_list_32 = { amqp_type_method_list_print };

amqp_type_methods_t amqp_methods_map_8 = { amqp_type_method_map_print };
amqp_type_methods_t amqp_methods_map_32 = { amqp_type_method_map_print };

amqp_type_methods_t amqp_methods_array_8 = { amqp_type_method_array_print };
amqp_type_methods_t amqp_methods_array_32 = { amqp_type_method_array_print };

amqp_type_methods_t amqp_methods_extension_type = { amqp_type_method_extension_type_print };
