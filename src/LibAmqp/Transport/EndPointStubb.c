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
#include "Transport/EndPointStubb.h"

amqp_endpoint_t *amqp__initialize_endpoint_stubb(amqp_context_t *context, amqp_endpoint_address_t *address)
{
    amqp_endpoint_t *result = AMQP_MALLOC(amqp_endpoint_t);
    not_implemented(amqp__initialize_local_endpoint);
    return result;
}

