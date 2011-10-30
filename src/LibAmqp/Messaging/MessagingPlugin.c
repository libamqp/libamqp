#include <assert.h>
#include <string.h>

#include "Messaging/MessagingPlugin.h"
#include "Messaging/MessagingDecode.h"

#include "debug_helper.h"

void amqp_initialize_default_messaging_methods(amqp_messaging_methods_t *methods)
{
    methods->decode.source = amqp_decode_source;
    methods->decode.target = amqp_decode_target;
    methods->decode.delivery_state = amqp_decode_delivery_state;
}
