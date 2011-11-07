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

//#include "Context/Context.h"

#include "string.h"

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Frame/Frame.h"
#include "Transport/Sasl/SaslMechanisms.h"
#include "Transport/Frame/FrameEncode.h"
#include "Codec/Encode/Encode.h"
#include "Plugin/Sasl.h"

#include "Transport/Sasl/ServerSasl.h"

#include "debug_helper.h"


int amqp_sasl_prepare_mechanisms_frame(amqp_connection_t *connection)
{
    return amqp_encode_sasl_mechanisms_frame(connection, connection->buffer.write);
}