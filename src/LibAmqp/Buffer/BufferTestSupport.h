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

#ifndef LIBAMQP_BUFFER_BUFFER_TEST_SUPPORT_H
#define LIBAMQP_BUFFER_BUFFER_TEST_SUPPORT_H

#include "Context/Context.h"
#include "Buffer/Buffer.h"

namespace t
{
    void amqp_buffer_dump_fragment(amqp_context_t* context, amqp_buffer_t *buffer, size_t start, size_t end);
    void amqp_buffer_dump(amqp_context_t* context, amqp_buffer_t *buffer);
}
#endif


