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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_TEST_SUPPORT_CONNECTION_HOLDER_H
#define LIBAMQP_TRANSPORT_CONNECTION_TEST_SUPPORT_CONNECTION_HOLDER_H

#include "libamqp_common.h"
#include "Context/TestSupport/ContextHolder.h"
#include "Transport/Connection/Connection.h"
#include "Transport/LowLevel/Timer.h"


#define repeatedly_call_run_loop_with_timeout(c) \
    while ((c)) \
    { \
        int event_triggered_before_timeout = run_loop_with_timeout(); \
        ASSERT(event_triggered_before_timeout); \
        if (!event_triggered_before_timeout) { \
            break; \
        } \
    }

#define loop_while_sm_state_is(c_fn, m, s) \
    CHECK_EQUAL(s, connection->state.m.name); \
    repeatedly_call_run_loop_with_timeout(c_fn(connection, s))

#define loop_while_connection_state_is(s) loop_while_sm_state_is(amqp_connection_is_state, connection, s)
#define loop_while_socket_state_is(s) loop_while_sm_state_is(amqp_connection_socket_is_state, socket, s)
#define loop_while_connection_amqp_state_is(s) loop_while_sm_state_is(amqp_connection_amqp_is_state, amqp, s)

#define loop_until_sm_state_is(c_fn, m, s) \
    repeatedly_call_run_loop_with_timeout(!c_fn(connection, s))

#define loop_until_connection_state_is(s)       loop_until_sm_state_is(amqp_connection_is_state, connection, s)
#define loop_until_connection_amqp_state_is(s)  loop_until_sm_state_is(amqp_connection_amqp_is_state, amqp, s)
#define loop_until_connection_socket_state_is(s)  loop_until_sm_state_is(amqp_connection_socket_is_state, socket, s)

#define loop_while_running()    repeatedly_call_run_loop_with_timeout(ConnectionHolder::running)


namespace TestSupport
{
    class ConnectionHolder :
            public virtual ContextHolder
    {
    public:
        ConnectionHolder();
        ~ConnectionHolder();

        int run_loop_with_timeout(double time_allowed);
        int run_loop_with_timeout();

    public:
        amqp_event_loop_t *loop;
        amqp_connection_t *connection;
        static int running;

    private:
        amqp_timer_t *timer;
        static void expiry_handler(amqp_context_t *context, amqp_timer_t *timer);
    };

    class UnconnectedConnectionHolder :
            public virtual ConnectionHolder
    {
    public:
        UnconnectedConnectionHolder();
        ~UnconnectedConnectionHolder();
    };

    class ClientConnectionHolder :
            public virtual ConnectionHolder
    {
    public:
        ClientConnectionHolder();
        ~ClientConnectionHolder();

        void connect_to(const char *hostname, int port_number);

    private:
        static void stopped_hook(amqp_connection_t *connection);
        static ClientConnectionHolder *client_connection;
    };
}
#endif
