
/*
    <type name="sasl-mechanisms" class="composite" source="list" provides="sasl-frame">
      <descriptor name="amqp:sasl-mechanisms:list" code="0x00000000:0x00000040"/>
      <field name="sasl-server-mechanisms" type="symbol" mandatory="true" multiple="true"/>
    </type>
*/
static void cleanup_sasl_mechanisms_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    amqp_multiple_symbol_cleanup(context, &frame->frames.sasl.mechanisms.sasl_server_mechanisms);
}
static int decode_sasl_mechanisms_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    int rc;
    frame->dispatch = amqp_dispatch_sasl_mechanisms;
    frame->cleanup = cleanup_sasl_mechanisms_frame;

    rc = decode_multiple_symbol(mandatory, context, field(field_list, field_zero), field_zero, 1, &frame->frames.sasl.mechanisms.sasl_server_mechanisms);

    if (rc == 0)
    {
        amqp_decode_frame_error(context, "SASL Mechanisms");
        return false;
    }
    return true;
}

/*
    <type name="sasl-init" class="composite" source="list" provides="sasl-frame">
      <descriptor name="amqp:sasl-init:list" code="0x00000000:0x00000041"/>
      <field name="mechanism" type="symbol" mandatory="true"/>
      <field name="initial-response" type="binary"/>
      <field name="hostname" type="string"/>
    </type>
*/
static void cleanup_sasl_init_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    amqp_symbol_cleanup(context, &frame->frames.sasl.init.mechanism);
//    amqp_binary_cleanup(context, &frame->frames.sasl.init.initial_response);
//    amqp_string_cleanup(context, &frame->frames.sasl.init.hostname);
}
static int decode_sasl_init_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_frames = 3;
    int rc;
    frame->dispatch = amqp_dispatch_sasl_init;
    frame->cleanup = cleanup_sasl_init_frame;

    rc = decode_symbol(mandatory, context, field(field_list, field_zero), field_zero, total_frames, &frame->frames.sasl.init.mechanism);
//    rc = rc && decode_binary(context, field(field_list, field_one), field_one, total_frames, &frame->frames.sasl.init.initial_response);
//    rc = rc && decode_string(context, field(field_list, field_two), field_two, total_frames, &frame->frames.sasl.init.hostname);

    if (rc == 0)
    {
        amqp_decode_frame_error(context, "SASL Init");
    }

    return rc;
}
