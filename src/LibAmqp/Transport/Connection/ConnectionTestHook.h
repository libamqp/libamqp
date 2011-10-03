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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_TEST_HOOK_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_TEST_HOOK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Transport/Connection/Connection.h"
#include "Transport/AcceptHandler.h"

#ifndef LIBAMQP_AMQP_CONNECTION_TEST_HOOK_TYPE_T
#define LIBAMQP_AMQP_CONNECTION_TEST_HOOK_TYPE_T
typedef struct amqp_connection_test_hook_t amqp_connection_test_hook_t;
#endif


#ifndef LIBAMQP_DISABLE_TEST_HOOKS
#define amqp_call_test_hook(connection, hook) \
if (connection->accept_handler_arguments->test_hooks && connection->accept_handler_arguments->test_hooks->hook) \
    { \
        connection->accept_handler_arguments->test_hooks->hook(connection); \
    }
#else
#define amqp_call_test_hook(c, h)
#endif

struct amqp_connection_test_hook_t
{
    void (*post_initialize)(amqp_connection_t *connection);
};

#ifdef __cplusplus
}
#endif
#endif
