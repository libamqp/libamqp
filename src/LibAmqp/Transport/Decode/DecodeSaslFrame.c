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
#include "Codec/Decode/Decode.h"

#include "AmqpTypes/Decode/AmqpTypesDecode.h"
#include "AmqpTypes/Decode/AmqpDefinitionsDecode.h"

#include "Transport/Connection/FrameDispatch.h"
#include "Transport/Frame/Frame.h"

#include "Transport/Decode/DecodeSaslFrame.h"

#include "debug_helper.h"

#ifdef AMQP_DECODE_GENERATED_HEADER
#undef AMQP_DECODE_GENERATED_HEADER
#endif
#define AMQP_DECODE_GENERATED_BODY

#include "Transport/Decode/decode_sasl_frame.gen"
