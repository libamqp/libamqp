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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_TEST_SUPPORT_WRITE_INTERCEPTOR_H
#define LIBAMQP_TRANSPORT_CONNECTION_TEST_SUPPORT_WRITE_INTERCEPTOR_H

#include "Transport/LowLevel/ListenerThread.h"
#include "Transport/DummyBroker/EchoServer.h"
#include "Transport/Connection/Connection.h"

#include "Context/TestSupport/ContextHolder.h"
#include "Transport/Connection/TestSupport/ConnectionHolder.h"

namespace TestSupport
{
    class WriteInterceptor :
            public virtual ContextHolder,
            public virtual ConnectionHolder
    {
    public:
        WriteInterceptor();
        ~WriteInterceptor();

        amqp_buffer_t *intercepted_write() const;


    private:
        static void write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer);
        static void illegal_write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer);
        static amqp_buffer_t *_intercepted_write;
    };
}
#endif
