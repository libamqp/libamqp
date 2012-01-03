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

#include "Context/Context.h"
#include "Transport/LowLevel/EventLoop.h"

typedef void (*livev_event_handler_t)(amqp_event_loop_t *loop, ev_io* io, const int revents);

void amqp_io_event_watcher_adjust_priority(amqp_io_event_watcher_t *watcher, int amount)
{
    ev_set_priority((ev_io *) watcher, amount);
}

void amqp_io_event_watcher_start(amqp_io_event_watcher_t *watcher)
{
    assert(watcher != 0 && watcher->loop != 0);
    ev_io_start(watcher->loop, &watcher->io);
}

void amqp_io_event_watcher_stop(amqp_io_event_watcher_t *watcher)
{
    assert(watcher != 0);
    assert(watcher->loop != 0);
    ev_io_stop(watcher->loop, &watcher->io);
}

amqp_io_event_watcher_t *amqp_io_event_watcher_initialize(amqp_context_t *context, amqp_event_loop_t *loop, amqp_event_handler_t handler, int fd, const int revents)
{
    amqp_io_event_watcher_t *result = AMQP_MALLOC(context, amqp_io_event_watcher_t);
    result->context = context;
    result->loop = loop;
    ev_io_init((ev_io *) result, (livev_event_handler_t) handler, fd, revents);
    return result;
}

void amqp_io_event_watcher_cb_set(amqp_io_event_watcher_t *watcher, amqp_event_handler_t handler)
{
    ev_set_cb((ev_io *) watcher, (livev_event_handler_t) handler);
}

void amqp_io_event_watcher_destroy(amqp_context_t *context, amqp_io_event_watcher_t *watcher)
{
    amqp_io_event_watcher_stop(watcher);
    AMQP_FREE(context, watcher);
}
