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

#ifndef LIBAMQP_TRANSPORT_SOCKET_INTERNAL_H
#define LIBAMQP_TRANSPORT_SOCKET_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ev.h>

#ifndef LIBAMQP_AMQP_TRANSPORT_STATE_TYPE_T
#define LIBAMQP_AMQP_TRANSPORT_STATE_TYPE_T
typedef struct amqp_transport_state_t amqp_transport_state_t;
#endif

const int amqp_max_listen_queue_length = 5;

struct amqp_transport_state_t
{
    struct ev_loop *loop;
    int listen_fd;
    struct ev_io *accept_watcher;
};

#ifdef __cplusplus
}
#endif
#endif
