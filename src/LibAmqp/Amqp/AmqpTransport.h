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
#ifndef LIBAMQP_AMQP_AMQP_TRANSPORT_H
#define LIBAMQP_AMQP_AMQP_TRANSPORT_H
    
#include "Amqp/Types.h"
    
typedef struct amqp_frame_open_t amqp_frame_open_t;
typedef struct amqp_frame_begin_t amqp_frame_begin_t;
typedef struct amqp_frame_attach_t amqp_frame_attach_t;
typedef struct amqp_frame_flow_t amqp_frame_flow_t;
typedef struct amqp_frame_transfer_t amqp_frame_transfer_t;
typedef struct amqp_frame_disposition_t amqp_frame_disposition_t;
typedef struct amqp_frame_detach_t amqp_frame_detach_t;
typedef struct amqp_frame_end_t amqp_frame_end_t;
typedef struct amqp_frame_close_t amqp_frame_close_t;

struct amqp_frame_open_t {
    amqp_string_t container_id; /* mandatory; */
    amqp_string_t hostname; 
    uint32_t max_frame_size; /* default="4294967295"; */
    uint16_t channel_max; /* default="65535"; */
    amqp_milliseconds_t idle_time_out; 
    amqp_multiple_ietf_language_tag_t outgoing_locales; 
    amqp_multiple_ietf_language_tag_t incoming_locales; 
    amqp_multiple_symbol_t offered_capabilities; 
    amqp_multiple_symbol_t desired_capabilities; 
    amqp_property_fields_t properties;
};
struct amqp_frame_begin_t {
    uint16_t remote_channel; 
    amqp_transfer_number_t next_outgoing_id; /* mandatory; */
    uint32_t incoming_window; /* mandatory; */
    uint32_t outgoing_window; /* mandatory; */
    uint32_t handle_max; /* default="4294967295"; */
    amqp_multiple_symbol_t offered_capabilities; 
    amqp_multiple_symbol_t desired_capabilities; 
    amqp_property_fields_t properties;
};
struct amqp_frame_attach_t {
    amqp_string_t name; /* mandatory; */
    uint32_t handle; /* mandatory; */
    amqp_role_t role; /* mandatory; */
    amqp_sender_settle_mode_t snd_settle_mode; /* default="mixed"; */
    amqp_receiver_settle_mode_t rcv_settle_mode; /* default="first"; */
    amqp_source_t source; 
    amqp_target_t target; 
    amqp_map_t unsettled; 
    int incomplete_unsettled; /* default="false"; */
    amqp_sequence_no_t initial_delivery_count; 
    uint64_t max_message_size; 
    amqp_multiple_symbol_t offered_capabilities; 
    amqp_multiple_symbol_t desired_capabilities; 
    amqp_property_fields_t properties;
};
struct amqp_frame_flow_t {
    amqp_transfer_number_t next_incoming_id; 
    uint32_t incoming_window; /* mandatory; */
    amqp_transfer_number_t next_outgoing_id; /* mandatory; */
    uint32_t outgoing_window; /* mandatory; */
    uint32_t handle; 
    amqp_sequence_no_t delivery_count; 
    uint32_t link_credit; 
    uint32_t available; 
    int drain; /* default="false"; */
    int echo; /* default="false"; */
    amqp_property_fields_t properties;
};
struct amqp_frame_transfer_t {
    uint32_t handle; /* mandatory; */
    amqp_delivery_number_t delivery_id; 
    amqp_delivery_tag_t delivery_tag; 
// TODO - define message_format
    int settled; 
    int more; /* default="false"; */
    amqp_receiver_settle_mode_t rcv_settle_mode; 
    amqp_state_t state; 
    int resume; /* default="false"; */
    int aborted; /* default="false"; */
    int batchable; /* default="false"; */
};
struct amqp_frame_disposition_t {
    amqp_role_t role; /* mandatory; */
    amqp_delivery_number_t first; /* mandatory; */
    amqp_delivery_number_t last; 
    int settled; /* default="false"; */
    amqp_state_t state; 
    int batchable; /* default="false"; */
};
struct amqp_frame_detach_t {
    uint32_t handle; /* mandatory; */
    int closed; /* default="false"; */
    amqp_error_t error; 
};
struct amqp_frame_end_t {
    amqp_error_t error; 
};
struct amqp_frame_close_t {
    amqp_error_t error; 
};
#endif
