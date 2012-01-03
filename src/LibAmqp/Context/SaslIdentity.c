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

#include <stdarg.h>
#include <string.h>
#include "Context/Context.h"
#include "Plugin/Sasl.h"

#include "debug_helper.h"

void amqp_context_register_identity_hooks(amqp_context_t *context, amqp_identity_hook_method_t provide_login, amqp_identity_hook_method_t provide_password, amqp_identity_hook_method_t provide_email)
{
    assert((provide_login && provide_password) || provide_email);

    context->sasl.identity_hooks.login = provide_login;
    context->sasl.identity_hooks.password = provide_password;
    context->sasl.identity_hooks.email = provide_email;
}
