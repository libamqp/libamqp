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

#include "AmqpTypes/AmqpTypesTestSupport.h"

#include "AmqpTypes/AmqpTypes.h"

#include "Amqp/AmqpSecurity.h"
#include "Amqp/AmqpTransport.h"
#include "Amqp/AmqpMessaging.h"
#include "Amqp/AmqpTransactions.h"

SUITE(AmqpTypes)
{
    // If a field is added to a generated type one of these will fail.
    // These may become annoying if types change frequently
#ifdef __LP64__
    TEST_FIXTURE(AmqpTypesFixture, type_sizes)
    {
        CHECK_EQUAL(72U, sizeof(amqp_type_t));

        CHECK_EQUAL(32U, sizeof(amqp_array_t));
        CHECK_EQUAL(56U, sizeof(amqp_binary_t));
        CHECK_EQUAL(56U, sizeof(amqp_entry_t));
        CHECK_EQUAL(16U, sizeof(amqp_list_t));
        CHECK_EQUAL(64U, sizeof(amqp_hash_table_t));
        CHECK_EQUAL(32U, sizeof(amqp_multiple_symbol_t));
        CHECK_EQUAL(56U, sizeof(amqp_string_t));
        CHECK_EQUAL(56U, sizeof(amqp_symbol_t));
        CHECK_EQUAL(64U, sizeof(amqp_amqp_type_t));
        CHECK_EQUAL(24U, sizeof(amqp_memory_t));
    }
    TEST_FIXTURE(AmqpTypesFixture, error_definition)
    {
        CHECK_EQUAL(152U, sizeof(amqp_error_t));
    }
    TEST_FIXTURE(AmqpTypesFixture, frames)
    {
        CHECK_EQUAL(288U, sizeof(amqp_transport_open_t));
        CHECK_EQUAL(128U, sizeof(amqp_transport_begin_t));
        CHECK_EQUAL(304U, sizeof(amqp_transport_attach_t));
        CHECK_EQUAL(88U, sizeof(amqp_transport_flow_t));
        CHECK_EQUAL(152U, sizeof(amqp_transport_transfer_t));
        CHECK_EQUAL(72U, sizeof(amqp_transport_disposition_t));
        CHECK_EQUAL(168U, sizeof(amqp_transport_detach_t));
        CHECK_EQUAL(152U, sizeof(amqp_transport_end_t));
        CHECK_EQUAL(152U, sizeof(amqp_transport_close_t));
    }
    TEST_FIXTURE(AmqpTypesFixture, security)
    {
        CHECK_EQUAL(32U, sizeof(amqp_security_sasl_mechanisms_t));
        CHECK_EQUAL(168U, sizeof(amqp_security_sasl_init_t));
        CHECK_EQUAL(56U, sizeof(amqp_security_sasl_challenge_t));
        CHECK_EQUAL(56U, sizeof(amqp_security_sasl_response_t));
        CHECK_EQUAL(64U, sizeof(amqp_security_sasl_outcome_t));
    }
    TEST_FIXTURE(AmqpTypesFixture, messaging_addressing)
    {
        CHECK_EQUAL(256U, sizeof(amqp_addressing_source_t));
        CHECK_EQUAL(120U, sizeof(amqp_addressing_target_t));
        CHECK_EQUAL(16U, sizeof(amqp_addressing_delete_on_close_t));
        CHECK_EQUAL(16U, sizeof(amqp_addressing_delete_on_no_links_t));
        CHECK_EQUAL(16U, sizeof(amqp_addressing_delete_on_no_messages_t));
        CHECK_EQUAL(16U, sizeof(amqp_addressing_delete_on_no_links_or_messages_t));
    }
    TEST_FIXTURE(AmqpTypesFixture, messaging_delivery_state)
    {
        CHECK_EQUAL(16U, sizeof(amqp_delivery_state_received_t));
        CHECK_EQUAL(16U, sizeof(amqp_delivery_state_accepted_t));
        CHECK_EQUAL(152U, sizeof(amqp_delivery_state_rejected_t));
        CHECK_EQUAL(16U, sizeof(amqp_delivery_state_released_t));
        CHECK_EQUAL(40U, sizeof(amqp_delivery_state_modified_t));
    }
    TEST_FIXTURE(AmqpTypesFixture, messaging)
    {
        CHECK_EQUAL(20U, sizeof(amqp_messaging_header_t));
        CHECK_EQUAL(32U, sizeof(amqp_messaging_delivery_annotations_t));
        CHECK_EQUAL(32U, sizeof(amqp_messaging_message_annotations_t));
        CHECK_EQUAL(520U, sizeof(amqp_messaging_properties_t));
        CHECK_EQUAL(32U, sizeof(amqp_messaging_application_properties_t));
        CHECK_EQUAL(56U, sizeof(amqp_messaging_data_t));
        CHECK_EQUAL(16U, sizeof(amqp_messaging_amqp_sequence_t));
        CHECK_EQUAL(40U, sizeof(amqp_messaging_amqp_value_t));
        CHECK_EQUAL(32U, sizeof(amqp_messaging_footer_t));
    }
#endif
}
