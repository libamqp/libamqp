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

#include "libamqp_common.h"
#include "Thread/Thread.h"

void amqp_semaphore_initialize(amqp_semaphore_t *semaphore)
{
    semaphore->count = 0;
    amqp_mutex_initialize(&semaphore->mutex);
    amqp_condition_initialize(&semaphore->cv);
}

void amqp_semaphore_destroy(amqp_semaphore_t *semaphore)
{
    amqp_mutex_destroy(&semaphore->mutex);
    amqp_condition_destroy(&semaphore->cv);
}

void amqp_semaphore_wait(amqp_semaphore_t *semaphore)
{
    amqp_mutex_lock(&semaphore->mutex);
    while (semaphore->count == 0)
    {
        amqp_condition_wait(&semaphore->cv, &semaphore->mutex);
    }
    semaphore->count--;
    amqp_mutex_unlock(&semaphore->mutex);
}

void amqp_semaphore_signal(amqp_semaphore_t *semaphore)
{
    amqp_mutex_lock(&semaphore->mutex);
    semaphore->count++;
    amqp_condition_notify(&semaphore->cv);
    amqp_mutex_unlock(&semaphore->mutex);
}

