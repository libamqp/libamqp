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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_STATE_MACHINE_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_STATE_MACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"

#include <errno.h>
#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include "Buffer/Buffer.h"
#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"

#include "debug_helper.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state(n)  const char* old__state__name = n
#define trace_transition(f, n) amqp_connection_trace_transition(connection, __FILE__, __LINE__, f, old__state__name, n)
#define trace_state_transition(f, n) amqp_connection_trace_transition(connection, file_name, line_number, f, old__state__name, n)
#define LIBAMQP_TRACE_STATE_ARGS_DECL       , const char *file_name, int line_number
#define LIBAMQP_TRACE_STATE_ARGS            , file_name, line_number
#define transition_to_state(c, s)           transition_to_ ## s(c, __FILE__, __LINE__)
#define transition_to_timeout(c, s)         transition_to_timeout_state(c, #s, __FILE__, __LINE__)
#define DECLARE_TRANSITION_FUNCTION(s)      static void transition_to_ ## s(amqp_connection_t *connection, const char *file_name, int line_number)
#else
#define save_old_state(n)
#define trace_transition(f, n)
#define trace_state_transition(f, n)
#define LIBAMQP_TRACE_STATE_ARGS_DECL
#define LIBAMQP_TRACE_STATE_ARGS
#define transition_to_state(c, s)           transition_to_ ## s(c)
#define transition_to_timeout(c, s)         transition_to_timeout_state(c, #s)
#define DECLARE_TRANSITION_FUNCTION(s)      static void transition_to_ ## s(amqp_connection_t *connection)
#endif

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL);

#define DEFINE_TRANSITION_TO_STATE(state) \
static void initialise_actions_for_ ## state(amqp_connection_t *connection); \
static void transition_to_ ## state(amqp_connection_t *connection LIBAMQP_TRACE_STATE_ARGS_DECL) \
{ \
    default_state_initialization(connection, #state, initialise_actions_for_ ## state  LIBAMQP_TRACE_STATE_ARGS); \
} \
static void initialise_actions_for_ ## state(amqp_connection_t *connection)

#define DEFINE_TRANSITION_TO_TIMEOUT_STATE() \
static void initialise_actions_for_timeout_state(amqp_connection_t *connection); \
static void transition_to_timeout_state(amqp_connection_t *connection, const char *state_name LIBAMQP_TRACE_STATE_ARGS_DECL) \
{ \
    default_state_initialization(connection, state_name, initialise_actions_for_timeout_state  LIBAMQP_TRACE_STATE_ARGS); \
} \
static void initialise_actions_for_timeout_state(amqp_connection_t *connection)



#ifdef __cplusplus
}
#endif
#endif
