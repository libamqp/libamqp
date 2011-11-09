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

#ifndef LIBAMQP_CONTEXT_SASL_IDENTITY_H
#define LIBAMQP_CONTEXT_SASL_IDENTITY_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
    typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_SASL_IDENTITY_TYPE_T
#define LIBAMQP_AMQP_SASL_IDENTITY_TYPE_T
    typedef struct amqp_sasl_identity_t amqp_sasl_identity_t;
#endif

    typedef const char *(*amqp_identity_hook_method_t)(amqp_context_t *context);

    struct amqp_sasl_identity_t
    {
        amqp_identity_hook_method_t login;
        amqp_identity_hook_method_t password;
        amqp_identity_hook_method_t email;
    };

#ifdef __cplusplus
}
#endif
#endif
