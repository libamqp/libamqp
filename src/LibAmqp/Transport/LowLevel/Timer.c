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

#include "Context/Context.h"
#include "Transport/LowLevel/Timer.h"

static void
timer_expired_callback(struct ev_loop *loop, ev_timer *watcher, int revents)
{
    amqp_timer_t *timer = (amqp_timer_t *) watcher;
    (timer->expiry_handler)(timer->context, timer);
}

amqp_timer_t *amqp_timer_initialize(amqp_context_t *context, amqp_timer_expiry_handler_t expiry_handler, void *argument)
{
    amqp_timer_t *result = AMQP_MALLOC(context, amqp_timer_t);
    result->expiry_handler = expiry_handler;
    result->context = context;
    result->argument = argument;
    ev_init((ev_timer *) result, timer_expired_callback);
    return result;
}

void amqp_timer_stop(amqp_context_t *context, amqp_timer_t *timer)
{
    ev_timer_stop(context->thread_event_loop, (ev_timer *) timer);
}

void amqp_timer_destroy(amqp_context_t *context, amqp_timer_t *timer)
{
    amqp_timer_stop(context, timer);
    AMQP_FREE(context, timer);
}

void amqp_timer_start(amqp_context_t *context, amqp_timer_t *timer, ev_tstamp delay)
{
    timer->timer.repeat = delay;
    ev_timer_again(context->thread_event_loop, (ev_timer *) timer);
}
