
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

    rc = decode_multiple_symbol(mandatory, context, field(field_list, 0), 0, 1, &frame->frames.sasl.mechanisms.sasl_server_mechanisms);

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
    amqp_binary_cleanup(context, &frame->frames.sasl.init.initial_response);
    amqp_string_cleanup(context, &frame->frames.sasl.init.hostname);
}
static int decode_sasl_init_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_fields = 3;
    int field_number = 0;
    int rc;
    frame->dispatch = amqp_dispatch_sasl_init;
    frame->cleanup = cleanup_sasl_init_frame;

    rc = decode_symbol(mandatory, context, field(field_list, field_number), field_number, total_fields, &frame->frames.sasl.init.mechanism); field_number++;
    rc = rc && decode_binary(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.sasl.init.initial_response); field_number++;
    rc = rc && decode_string(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.sasl.init.hostname); field_number++;

    assert(field_number = total_fields);
    if (rc == 0)
    {
        amqp_decode_frame_error(context, "SASL Init");
    }

    return rc;
}

/*
    <type name="sasl-challenge" class="composite" source="list" provides="sasl-frame">
      <descriptor name="amqp:sasl-challenge:list" code="0x00000000:0x00000042"/>
      <field name="challenge" type="binary" mandatory="true"/>
    </type>
*/
static void cleanup_sasl_challenge_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    amqp_binary_cleanup(context, &frame->frames.sasl.challenge.challenge);
}
static int decode_sasl_challenge_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_fields = 1;
    int rc;

    frame->dispatch = amqp_dispatch_sasl_challenge;
    frame->cleanup = cleanup_sasl_challenge_frame;

    rc = decode_binary(mandatory, context, field(field_list, 0), 0, total_fields, &frame->frames.sasl.challenge.challenge);

    if (rc == 0)
    {
        amqp_decode_frame_error(context, "SASL Challenge");
    }

    return rc;
}

/*
    <type name="sasl-response" class="composite" source="list" provides="sasl-frame">
      <descriptor name="amqp:sasl-response:list" code="0x00000000:0x00000043"/>
      <field name="response" type="binary" mandatory="true"/>
    </type>
*/
static void cleanup_sasl_response_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    amqp_binary_cleanup(context, &frame->frames.sasl.response.response);
}
static int decode_sasl_response_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_fields = 1;
    int rc;

    frame->dispatch = amqp_dispatch_sasl_response;
    frame->cleanup = cleanup_sasl_response_frame;

    rc = decode_binary(mandatory, context, field(field_list, 0), 0, total_fields, &frame->frames.sasl.response.response);

    if (rc == 0)
    {
        amqp_decode_frame_error(context, "SASL Response");
    }

    return rc;
}

/*
    <type name="sasl-outcome" class="composite" source="list" provides="sasl-frame">
      <descriptor name="amqp:sasl-outcome:list" code="0x00000000:0x00000044"/>
      <field name="code" type="sasl-code" mandatory="true"/>
      <field name="additional-data" type="binary"/>
    </type>
*/
static void cleanup_sasl_outcome_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    amqp_binary_cleanup(context, &frame->frames.sasl.outcome.additional_data);
}
static int decode_sasl_outcome_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_fields = 2;
    int field_number = 0;
    int rc;

    frame->dispatch = amqp_dispatch_sasl_outcome;
    frame->cleanup = cleanup_sasl_outcome_frame;

    rc = decode_ubyte(mandatory, context, field(field_list, field_number), field_number, total_fields, &frame->frames.sasl.outcome.code, NO_DEFAULT_VALUE); field_number++;
    rc = rc && decode_binary(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.sasl.outcome.additional_data); field_number++;

    assert(field_number = total_fields);
    if (rc == 0)
    {
        amqp_decode_frame_error(context, "SASL Outcome");
    }

    return rc;
}

/*
    <type name="open" class="composite" source="list" provides="frame">
      <descriptor name="amqp:open:list" code="0x00000000:0x00000010"/>
      <field name="container-id" type="string" mandatory="true"/>
      <field name="hostname" type="string"/>
      <field name="max-frame-size" type="uint" default="4294967295"/>
      <field name="channel-max" type="ushort" default="65535"/>
      <field name="idle-time-out" type="milliseconds"/>
      <field name="outgoing-locales" type="ietf-language-tag" multiple="true"/>
      <field name="incoming-locales" type="ietf-language-tag" multiple="true"/>
      <field name="offered-capabilities" type="symbol" multiple="true"/>
      <field name="desired-capabilities" type="symbol" multiple="true"/>
      <field name="properties" type="fields"/>
    </type>
*/
static void cleanup_amqp_open_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    amqp_string_cleanup(context, &frame->frames.amqp.open.container_id);
    amqp_string_cleanup(context, &frame->frames.amqp.open.hostname);
    // max_frame_size
    // channel_max
    // idle_time_out
    amqp_multiple_symbol_cleanup(context, &frame->frames.amqp.open.outgoing_locales);
    amqp_multiple_symbol_cleanup(context, &frame->frames.amqp.open.incoming_locales);
    amqp_multiple_symbol_cleanup(context, &frame->frames.amqp.open.offered_capabilities);
    amqp_multiple_symbol_cleanup(context, &frame->frames.amqp.open.desired_capabilities);

    amqp_map_cleanup(context, &frame->frames.amqp.open.properties);
}

static int decode_amqp_open_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_fields = 10;
    int field_number = 0;
    int rc;

    frame->dispatch = amqp_dispatch_amqp_open;
    frame->cleanup = cleanup_amqp_open_frame;

    rc = decode_string(mandatory, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.container_id); field_number++;
    rc = rc && decode_string(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.hostname); field_number++;
    rc = rc && decode_uint(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.max_frame_size, 4294967295U); field_number++;
    rc = rc && decode_ushort(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.channel_max, 65535U); field_number++;
    rc = rc && decode_uint(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.idle_time_out, 0U); field_number++;
    rc = rc && decode_multiple_symbol(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.outgoing_locales); field_number++;
    rc = rc && decode_multiple_symbol(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.incoming_locales); field_number++;

    rc = rc && decode_multiple_symbol(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.offered_capabilities); field_number++;
    rc = rc && decode_multiple_symbol(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.desired_capabilities); field_number++;
    rc = rc && decode_map(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.properties); field_number++;

    assert(field_number = total_fields);
    if (rc == 0)
    {
        amqp_decode_frame_error(context, "AMQP Open");
    }

    return rc;
}

/*
    <type name="begin" class="composite" source="list" provides="frame">
      <descriptor name="amqp:begin:list" code="0x00000000:0x00000011"/>
      <field name="remote-channel" type="ushort"/>
      <field name="next-outgoing-id" type="transfer-number" mandatory="true"/>
      <field name="incoming-window" type="uint" mandatory="true"/>
      <field name="outgoing-window" type="uint" mandatory="true"/>
      <field name="handle-max" type="handle" default="4294967295"/>
      <field name="offered-capabilities" type="symbol" multiple="true"/>
      <field name="desired-capabilities" type="symbol" multiple="true"/>
      <field name="properties" type="fields"/>
    </type>
*/
static void cleanup_amqp_begin_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    // remote_channel
    // next_outgoing_id
    // incoming_window
    // outgoing_window
    // handle_max
    amqp_multiple_symbol_cleanup(context, &frame->frames.amqp.begin.offered_capabilities);
    amqp_multiple_symbol_cleanup(context, &frame->frames.amqp.begin.desired_capabilities);
    amqp_map_cleanup(context, &frame->frames.amqp.begin.properties);
}

static int decode_amqp_begin_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_fields = 8;
    int field_number = 0;
    int rc;

    frame->dispatch = amqp_dispatch_amqp_begin;
    frame->cleanup = cleanup_amqp_begin_frame;

    rc = rc && decode_ushort(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.begin.remote_channel, 0U); field_number++;
    rc = rc && decode_uint(mandatory, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.begin.next_outgoing_id, 0U); field_number++;
    rc = rc && decode_uint(mandatory, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.begin.incoming_window, 0U); field_number++;
    rc = rc && decode_uint(mandatory, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.begin.outgoing_window, 0U); field_number++;
    rc = rc && decode_uint(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.begin.handle_max, 4294967295U); field_number++;

    rc = rc && decode_multiple_symbol(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.begin.offered_capabilities); field_number++;
    rc = rc && decode_multiple_symbol(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.begin.desired_capabilities); field_number++;
    rc = rc && decode_map(optional, context, field(field_list, field_number), field_number, total_fields, &frame->frames.amqp.open.properties); field_number++;

    assert(field_number = total_fields);
    if (rc == 0)
    {
        amqp_decode_frame_error(context, "AMQP Begin");
    }

    return rc;
}
