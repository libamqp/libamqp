
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

/* What an expanded FIELD_DECODE_FN(string) looks like
static int decode_string(int mandatory, amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_string_t *string)
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
*/

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
FIELD_DECODE_FN(binary)
FIELD_DECODE_FN(string)
FIELD_DECODE_FN(map)

#define NO_DEFAULT_VALUE    0
#define PRIMITIVE_FIELD_DECODE_FN(ft, t) \
static int \
decode_ ## ft(int mandatory, amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, t *result, t default_value) \
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
            *result = default_value; \
            return true; \
        } \
    } \
    if (!amqp_type_is_ ## ft(field)) \
    { \
        amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type " #ft "."); \
        return false;  \
    } \
    *result = amqp_type_to_ ## ft(field); \
    return true; \
}
PRIMITIVE_FIELD_DECODE_FN(ubyte, uint8_t)
PRIMITIVE_FIELD_DECODE_FN(ushort, uint16_t)
PRIMITIVE_FIELD_DECODE_FN(uint, uint32_t)
