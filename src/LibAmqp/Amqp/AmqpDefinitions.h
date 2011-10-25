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

#ifndef LIBAMQP_AMQP_DEFINITIONS_H
#define LIBAMQP_AMQP_DEFINITIONS_H

#include <stdint.h>

enum {
    amqp_sasl_code_ok,
    amqp_sasl_code_auth_error,
    amqp_sasl_code_system_error,
    amqp_sasl_code_perm_system_error,
    amqp_sasl_code_temp_system_error
};

typedef uint8_t amqp_sasl_code_t;

typedef enum amqp_role_t
{
    amqp_sender_role = false,
    amqp_receiver_role = true
} amqp_role_t;

typedef enum amqp_sender_settle_mode_t
{
    amqp_sender_settle_mode_unsettled = 0,
    amqp_sender_settle_mode_settled = 1,
    amqp_sender_settle_mode_mixed = 2
} amqp_sender_settle_mode_t;

typedef enum amqp_receiver_settle_mode_t
{
    amqp_receiver_settle_mode_first = 0,
    amqp_receiver_settle_mode_second = 1
} amqp_receiver_settle_mode_t;

typedef uint32_t amqp_milliseconds_t;
typedef uint32_t amqp_seconds_t;
typedef amqp_binary_t amqp_delivery_tag_t;
typedef uint32_t amqp_sequence_no_t;    // RFC-1982 sequence number
typedef amqp_sequence_no_t amqp_delivery_number_t;
typedef amqp_sequence_no_t amqp_transfer_number_t;
typedef uint32_t amqp_message_format_t;
typedef amqp_multiple_symbol_t amqp_multiple_ietf_language_tag_t;

typedef amqp_map_t amqp_property_fields_t;
typedef amqp_map_t amqp_fields_t;

typedef struct amqp_error_t         // descriptor 0000:001d
{
    amqp_symbol_t condition;
    amqp_string_t description;
    amqp_fields_t info;
} amqp_error_t;

typedef struct amqp_wildcard_t
{
    int filler;
} amqp_wildcard_t;

typedef int  amqp_terminus_durability_t;
typedef int  amqp_terminus_expiry_policy_t;
typedef int  amqp_filter_set_t;
#endif
