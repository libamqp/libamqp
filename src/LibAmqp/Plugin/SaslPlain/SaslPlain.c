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

#include <string.h>

#include "Context/Context.h"
#include "Codec/Encode/Encode.h"
#include "Plugin/SaslPlain/SaslPlain.h"
#include "debug_helper.h"

static void cleanup_instance_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    assert(sasl_plugin != 0);
    AMQP_FREE(context, sasl_plugin);
}

static amqp_type_t *encode_initial_response(amqp_context_t *context, amqp_buffer_t *buffer, const char *login, const char *password)
{
    amqp_type_t *result;
    size_t login_size = strlen(login);
    size_t password_size = strlen(password);
    size_t size = login_size + password_size + 2;
    size_t needed_space = (((size - 1) / 512) + 1) * 512;
    unsigned char *space = amqp_malloc(context, needed_space);
    unsigned char *p = space;

    assert(login_size < 256 && password_size < 256);

    *p++ = '\0';
    memcpy(p, login, login_size);
    p += login_size;
    *p++ = '\0';
    memcpy(p, password, password_size);

    result = amqp_encode_binary(context, buffer, space, size);
    amqp_free(context, space);
    return result;
}

static amqp_type_t *initial_response_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks)
{
    const char *login;
    const char *password;

    assert(identity_hooks->login && identity_hooks->password);

    login = identity_hooks->login(context);
    password = identity_hooks->password(context);

    return encode_initial_response(context, buffer, login, password);
}

static amqp_sasl_plugin_t *instance_create_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    amqp_sasl_plugin_t *result = amqp_sasl_plugin_base_instance_create(context, sasl_plugin);

    result->essence.instance.cleanup_instance_handler = cleanup_instance_handler;
    result->essence.instance.initial_response_handler = initial_response_handler;

    return result;
}

amqp_sasl_plugin_t *amqp_plugin_sasl_plain_create(amqp_context_t *context)
{
    return amqp_sasl_plugin_base_create(context, "PLAIN", instance_create_handler);
}

