
static int decode_multiple_symbol(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_multiple_symbol_t *multiple)
{
    if (!amqp_multiple_symbol_initialize(context, multiple, field))
    {
        amqp_decode_field_error(context, field_number, total_fields, "Field is not a valid multiple symbol.");
        return false;
    }
    return true;
}

static int decode_mandatory_multiple_symbol(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_multiple_symbol_t *multiple)
{
    int rc = decode_multiple_symbol(context, field, field_number, total_fields, multiple);
    if (rc)
    {
        if (multiple->size == 0)
        {
            amqp_mandatory_field_missing_error(context, field_number, total_fields, "multiple symbol");
            return false;
        }
    }
    return rc;
}

#define FIELD_DECODE_FN(ft) \
static int decode_ ## ft( \
amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_ ## ft ## _t *ft) \
{ \
    if (amqp_type_is_null(field)) \
    { \
        amqp_ ## ft ## _initialize_as_null(context, ft); \
        return true; \
    } \
    if (!amqp_type_is_ ## ft(field)) \
    { \
        amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type " #ft "."); \
        return false;  \
    } \
    amqp_ ## ft ## _initialize_from_type(context, ft, field); \
    return true; \
} \
static int decode_mandatory_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_ ## ft ## _t *ft) \
{ \
    int rc = decode_ ## ft(context, field, field_number, total_fields, ft); \
    if (rc) \
    { \
        if (amqp_ ## ft ## _is_null(ft)) \
        { \
            amqp_mandatory_field_missing_error(context, field_number, total_fields, #ft); \
            return false; \
        } \
    } \
    return rc; \
} \
/* TODO - remove this */ \
void (*shutup_about_ ## ft)() = (void (*)()) decode_mandatory_ ## ft;

FIELD_DECODE_FN(symbol)
FIELD_DECODE_FN(binary)
FIELD_DECODE_FN(string)
FIELD_DECODE_FN(map)

#define NO_DEFAULT_VALUE    0
#define PRIMITIVE_FIELD_DECODE_FN(ft, t) \
static int \
decode__ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, t *result) \
{ \
    if (!amqp_type_is_ ## ft(field)) \
    { \
        amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type " #ft "."); \
        return false;  \
    } \
    *result = amqp_type_to_ ## ft(field); \
    return true; \
} \
static int \
decode_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, t *result, t default_value) \
{ \
    if (amqp_type_is_null(field)) \
    { \
        *result = default_value; \
        return true; \
    } \
    return decode__ ## ft(context, field, field_number, total_fields, result); \
} \
static int \
decode_mandatory_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, t *result) \
{ \
    if (amqp_type_is_null(field)) \
    { \
        amqp_mandatory_field_missing_error(context, field_number, total_fields, #ft); \
        return false; \
    } \
    return decode__ ## ft(context, field, field_number, total_fields, result); \
} \
/* TODO - remove these */ \
void (*shutup_about_ ## ft)() = (void (*)()) decode_ ## ft; \
void (*shutup_about_mandatory ## ft)() = (void (*)()) decode_mandatory_ ## ft;

PRIMITIVE_FIELD_DECODE_FN(ubyte, uint8_t)
PRIMITIVE_FIELD_DECODE_FN(ushort, uint16_t)
PRIMITIVE_FIELD_DECODE_FN(uint, uint32_t)
PRIMITIVE_FIELD_DECODE_FN(ulong, uint64_t)

PRIMITIVE_FIELD_DECODE_FN(byte, int8_t)
PRIMITIVE_FIELD_DECODE_FN(short, int16_t)
PRIMITIVE_FIELD_DECODE_FN(int, int32_t)
PRIMITIVE_FIELD_DECODE_FN(long, int64_t)
PRIMITIVE_FIELD_DECODE_FN(boolean, int)

static inline int
decode_mandatory_sasl_code(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, uint8_t *result)
{
    if (decode_mandatory_ubyte(context, field, field_number, total_fields, result))
    {
        return *result <= amqp_sasl_code_temp_system_error;
    }
    return false;
}

static inline int
decode_sender_settle_mode(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_sender_settle_mode_t *result, amqp_sender_settle_mode_t default_value)
{
    int rc = decode_ubyte(context, field, field_number, total_fields, result, default_value);
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

static inline int
decode_receiver_settle_mode(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_receiver_settle_mode_t *result, amqp_receiver_settle_mode_t default_value)
{
    int rc = decode_ubyte(context, field, field_number, total_fields, result, default_value);
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

static void
amqp_error_cleanup(amqp_context_t *context, amqp_error_t *error)
{
    amqp_symbol_cleanup(context, &error->condition);
    amqp_string_cleanup(context, &error->description);
    amqp_map_cleanup(context, &error->info);
}

static int
decode_error(amqp_context_t *context, amqp_type_t *field_type, int field_number, int total_fields, amqp_error_t *result)
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

    rc = decode_mandatory_symbol(context, field(error_field_list, 0), 0, 3, &result->condition) &&
            decode_string(context, field(error_field_list, 1), 1, 3, &result->description) &&
            decode_map(context, field(error_field_list, 2), 2, 3, &result->info);

    if (rc == 0)
    {
        amqp_decode_frame_error(context, "AMQP Error");
    }

    return rc;
}