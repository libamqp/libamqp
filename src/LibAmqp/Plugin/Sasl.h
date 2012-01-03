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

#ifndef LIBAMQP_PLUGIN_SASL_H
#define LIBAMQP_PLUGIN_SASL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "Amqp/AmqpSecurity.h"
#include "AmqpTypes/AmqpBinary.h"

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
    typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_TYPE_T
#define LIBAMQP_AMQP_TYPE_T
    typedef struct amqp_type_t amqp_type_t;
#endif

#ifndef LIBAMQP_AMQP_SASL_PLUGIN_TYPE_T
#define LIBAMQP_AMQP_SASL_PLUGIN_TYPE_T
typedef struct amqp_sasl_plugin_t amqp_sasl_plugin_t;
#endif

#ifndef LIBAMQP_AMQP_SASL_PLUGIN_STATE_TYPE_T
#define LIBAMQP_AMQP_SASL_PLUGIN_STATE_TYPE_T
typedef struct amqp_sasl_plugin_state_t amqp_sasl_plugin_state_t;
#endif

enum {
    amqp_plugin_handler_failed,
    amqp_plugin_handler_challenge,
    amqp_plugin_handler_outcome_accepted,
    amqp_plugin_handler_outcome_rejected,
};

typedef void (*amqp_sasl_plugin_cleanup_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);
typedef amqp_sasl_plugin_t *(*amqp_sasl_plugin_instance_create_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);

typedef void (*amqp_sasl_plugin_instance_cleanup_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);

typedef amqp_type_t *(*amqp_sasl_plugin_initial_response_encoder)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_sasl_identity_t *identity_hooks, amqp_buffer_t *buffer);
typedef int (*amqp_sasl_plugin_initial_response_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_sasl_identity_t *identity_hooks, amqp_security_sasl_init_t *init_response);

typedef int (*amqp_sasl_plugin_query_challenge_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_sasl_identity_t *identity_hooks, amqp_security_sasl_init_t *init_response);
typedef amqp_type_t *(*amqp_sasl_plugin_challenge_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks, amqp_security_sasl_init_t *init_response);
typedef amqp_type_t *(*amqp_sasl_plugin_challenge_response_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_binary_t *challenge, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks);
typedef int (*amqp_sasl_plugin_outcome_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_security_sasl_outcome_t *outcome);

struct amqp_sasl_plugin_t
{
    const char *mechanism_name;
    union {
        struct {
            amqp_sasl_plugin_cleanup_handler cleanup_plugin_handler;
            amqp_sasl_plugin_instance_create_handler instance_create_handler;
        } plugin;
        struct {
            amqp_sasl_plugin_instance_cleanup_handler cleanup_instance_handler;

            amqp_sasl_plugin_initial_response_encoder initial_response_encoder;
            amqp_sasl_plugin_initial_response_handler initial_response_handler;

            amqp_sasl_plugin_challenge_handler challenge_handler;
            amqp_sasl_plugin_challenge_response_handler challenge_response_handler;
            amqp_sasl_plugin_outcome_handler outcome_handler;
        } instance;
    } essence;
    amqp_sasl_plugin_state_t *plugin_state;
    char *login;
    char *password;
    char *hostname;
    int outcome;
};

extern amqp_sasl_plugin_t *amqp_sasl_plugin_instance_create(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);

extern amqp_type_t *amqp_sasl_plugin_initial_response_encode(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_sasl_identity_t *identity_hooks, amqp_buffer_t *buffer);
extern int amqp_sasl_plugin_initial_response_handle(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_sasl_identity_t *identity_hooks, amqp_security_sasl_init_t *init_response);


extern amqp_type_t *amqp_sasl_plugin_challenge(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks, amqp_security_sasl_init_t *init_response);
extern amqp_type_t *amqp_sasl_plugin_challenge_response(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_binary_t *challenge, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks);

extern int amqp_sasl_plugin_handle_outcome(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_security_sasl_outcome_t *outcome);

extern amqp_sasl_plugin_t *amqp_sasl_plugin_base_create(amqp_context_t *context, const char *name, amqp_sasl_plugin_instance_create_handler instance_create_handler);
extern amqp_sasl_plugin_t *amqp_sasl_plugin_base_instance_create(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);
extern void amqp_sasl_plugin_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);

extern void amqp_sasl_plugin_instance_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);

#ifdef __cplusplus
}
#endif
#endif
