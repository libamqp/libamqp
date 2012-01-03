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

#include <assert.h>
#include <string.h>

#include "AmqpTypes/Decode/AmqpDecodeInternal.h"
#include "AmqpTypes/Decode/AmqpTypesDecode.h"
#include "AmqpTypes/Decode/AmqpDefinitionsDecode.h"

#include "debug_helper.h"


int amqp_decode_multiple_type_symbol(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_multiple_symbol_t *multiple)
{
    if (!amqp_multiple_symbol_initialize(context, multiple, field))
    {
        amqp_decode_field_error(context, field_number, total_fields, "Field is not a valid multiple symbol.");
        return false;
    }
    return true;
}

int amqp_decode_mandatory_multiple_type_symbol(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_multiple_symbol_t *multiple)
{
    int rc = amqp_decode_multiple_type_symbol(context, field, field_number, total_fields, multiple);
    if (rc)
    {
        if (amqp_multiple_symbol_is_null(multiple))
        {
            amqp_mandatory_field_missing_error(context, field_number, total_fields, "multiple symbol");
            return false;
        }
    }
    return rc;
}

int amqp_decode_mandatory_primitive_sasl_code(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint8_t *result)
{
    if (amqp_decode_mandatory_primitive_ubyte(context, field, field_number, total_fields, result))
    {
        return *result <= amqp_sasl_code_temp_system_error;
    }
    return false;
}

int amqp_decode_primitive_sender_settle_mode(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_sender_settle_mode_t *result, amqp_sender_settle_mode_t default_value)
{
    int rc = amqp_decode_primitive_ubyte(context, field, field_number, total_fields, result, default_value);
    if (rc)
    {
        if (*result > amqp_sender_settle_mode_mixed)
        {
            amqp_field_range_error(context, field_number, total_fields, "Sender settle mode value out of range: %d.", (int) *result);
            return false;
        }
    }
    return rc;
}

int amqp_decode_primitive_receiver_settle_mode(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_receiver_settle_mode_t *result, amqp_receiver_settle_mode_t default_value)
{
    int rc = amqp_decode_primitive_ubyte(context, field, field_number, total_fields, result, default_value);
    if (rc)
    {
        if (*result > amqp_receiver_settle_mode_second)
        {
            amqp_field_range_error(context, field_number, total_fields, "Receiver settle mode value out of range: %d.", (int) *result);
            return false;
        }
    }
    return rc;
}

int amqp_decode_primitive_handle(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_handle_t *result, uint32_t default_value)
{
    if (amqp_type_is_null(field))
    {
        result->provided = false;
        result->value = default_value;
        return true;
    }

    if (amqp_type_is_uint(field))
    {
        result->provided = true;
        result->value = amqp_type_to_uint(field);
        return true;
    }

    result->provided = false;
    amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type (uint) handle.");
    return false;
}

int amqp_decode_mandatory_primitive_handle(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_handle_t *result)
{
    if (amqp_type_is_null(field))
    {
        result->provided = false;
        amqp_mandatory_field_missing_error(context, field_number, total_fields, "(uint) handle");
        return false;
    }

    if (amqp_type_is_uint(field))
    {
        result->provided = true;
        result->value = amqp_type_to_uint(field);
        return true;
    }

    result->provided = false;
    amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type (uint) handle.");
    return false;
}

int amqp_decode_primitive_sequence_no(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_sequence_no_t *result, uint32_t default_value)
{
    if (amqp_type_is_null(field))
    {
        result->provided = false;
        result->value = default_value;
        return true;
    }

    if (amqp_type_is_uint(field))
    {
        result->provided = true;
        result->value = amqp_type_to_uint(field);
        return true;
    }

    result->provided = false;
    amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type (uint) sequence-no.");
    return false;
}

int amqp_decode_mandatory_primitive_sequence_no(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_sequence_no_t *result)
{
    if (amqp_type_is_null(field))
    {
        result->provided = false;
        amqp_mandatory_field_missing_error(context, field_number, total_fields, "(uint) sequence-no");
        return false;
    }

    if (amqp_type_is_uint(field))
    {
        result->provided = true;
        result->value = amqp_type_to_uint(field);
        return true;
    }

    result->provided = false;
    amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type (uint) sequence-no.");
    return false;
}

int amqp_decode_primitive_message_format(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_message_format_t *result, uint32_t default_value)
{
    if (amqp_type_is_null(field))
    {
        result->provided = false;
        result->value = default_value;
        return true;
    }
    if (amqp_type_is_uint(field))
    {
        result->provided = true;
        result->value = amqp_type_to_uint(field);
        return true;
    }

    result->provided = false;
    amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type (uint) message-format.");
    return false;
}

void amqp_amqp_error_cleanup(amqp_context_t *context, amqp_error_t *error)
{
    if (error && !error->is_null)
    {
        amqp_symbol_cleanup(context, &error->condition);
        amqp_string_cleanup(context, &error->description);
        amqp_map_cleanup(context, &error->info);
    }
}

int amqp_decode_type_amqp_error(amqp_context_t *context, amqp_type_t *field_type, int field_number, int total_fields, amqp_error_t *result)
{
    amqp_type_t *error_field_list;
    int rc;

    if (amqp_type_is_null(field_type))
    {
        result->is_null = true;
        amqp_symbol_initialize_as_null(context, &result->condition);
        amqp_string_initialize_as_null(context, &result->description);
        amqp_map_initialize_as_null(context, &result->info);
        return true;
    }
    if (!amqp_type_is_composite(field_type))
    {
        amqp_decode_field_error(context, field_number, total_fields, "Expected a composite type describing an AMQP Error condition.");
        return false;
    }

    // TODO - check class code is 0x1d. If not, lookup error handling plugin manager

    error_field_list = amqp_type_get_described(field_type);
    if (!amqp_type_is_list(error_field_list))
    {
        amqp_decode_field_error(context, field_number, total_fields, "Expected a described list containing error fields.");
        return false;
    }

    rc = amqp_decode_mandatory_type_symbol(context, amqp_field_from_list(error_field_list, 0), 0, 3, &result->condition) &&
            amqp_decode_type_string(context, amqp_field_from_list(error_field_list, 1), 1, 3, &result->description) &&
            amqp_decode_type_map(context, amqp_field_from_list(error_field_list, 2), 2, 3, &result->info);

    if (rc == 0)
    {
        amqp_decode_frame_error(context, "AMQP Error");
    }

    return rc;
}

static int decode_type_wildcard(amqp_context_t *context, amqp_wildcard_t *wildcard, amqp_type_t *type, amqp_messaging_type_decoder_t decoder)
{
    if (amqp_type_is_null(type))
    {
        amqp_wildcard_initialize_as_null(context, wildcard);
    }
    else
    {
        amqp_wildcard_initialize(context, wildcard, type, decoder);
    }
    return true;
}

int amqp_decode_type_wildcard_expecting_source(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_wildcard_t *wildcard)
{
    return decode_type_wildcard(context, wildcard, field, context->reference.plugins.messaging.decode.source);
}

int amqp_decode_type_wildcard_expecting_target(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_wildcard_t *wildcard)
{
    return decode_type_wildcard(context, wildcard, field, context->reference.plugins.messaging.decode.target);
}

int amqp_decode_type_wildcard_expecting_delivery_state(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_wildcard_t *wildcard)
{
    return decode_type_wildcard(context, wildcard, field, context->reference.plugins.messaging.decode.delivery_state);
}
