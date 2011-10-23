
static int decode_multiple_symbol(int mandatory, amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_multiple_symbol_t *multiple)
{
    if (field_is_null(field))
    {
        if (mandatory)
        {
            amqp_mandatory_field_missing_error(context, field_number, total_fields, "multiple symbol");
            return false;
        }
        else
        {
            amqp_multiple_symbol_initialize_as_null(context, multiple);
            return true;
        }
    }

    if (!amqp_multiple_symbol_initialize(context, multiple, field))
    {
        amqp_decode_field_error(context, field_number, total_fields, "Field is not a valid multiple symbol.");
        return false;
    }

    if (multiple && multiple->size == 0)
    {
        amqp_decode_field_error(context, field_number, total_fields, "Mandatory multiple symbol field is empty.");
        return false;
    }
    return true;
}

//static
int decode_string(int mandatory, amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_string_t *string)
{
    if (field_is_null(field))
    {
        if (mandatory)
        {
            amqp_mandatory_field_missing_error(context, field_number, total_fields, "string");
            return false;
        }
        else
        {
            amqp_string_initialize_as_null(context, string);
            return true;
        }
    }

    if (!amqp_type_is_string(field))
    {
        amqp_decode_field_error(context, field_number, total_fields, "Expected a string.");
        return false;
    }

    amqp_string_initialize_from_type(context, string, field);
    return true;
}

#define FIELD_DECODE_FN(ft) \
static int decode_ ## ft( \
int mandatory, amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, \
amqp_ ## ft ## _t *ft) \
{ \
    if (field_is_null(field)) \
    { \
        if (mandatory) \
        { \
            amqp_mandatory_field_missing_error(context, field_number, total_fields, #ft); \
            return false; \
        } \
        else \
        { \
            amqp_ ## ft ## _initialize_as_null(context, ft); \
            return true; \
        } \
    } \
    if (!amqp_type_is_ ## ft(field)) \
    { \
        amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type " #ft "."); \
        return false;  \
    } \
    amqp_ ## ft ## _initialize_from_type(context, ft, field); \
    return true; \
}
FIELD_DECODE_FN(symbol)
//FIELD_DECODE_FN(binary)