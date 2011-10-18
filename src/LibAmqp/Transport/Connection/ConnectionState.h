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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Transport/LowLevel/Connect.h"
#include "Transport/LowLevel/Timer.h"
#include "Transport/Connection/ConnectionTrace.h"
#include "Transport/Connection/ConnectionRead.h"
#include "Transport/Connection/ConnectionSocket.h"
#include "Transport/Connection/ConnectionWrite.h"
#include "Transport/Connection/ConnectionNegotiation.h"
#include "Transport/Connection/ConnectionTls.h"
#include "Transport/Connection/ConnectionSasl.h"
#include "Transport/Connection/ConnectionAmqp.h"
#include "Transport/Connection/ConnectionFrame.h"

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_CONNECTION_TYPE_T
#define LIBAMQP_AMQP_CONNECTION_TYPE_T
typedef struct amqp_connection_t amqp_connection_t;
#endif

#ifndef LIBAMQP_AMQP_CONNECTIONS_TYPE_T
#define LIBAMQP_AMQP_CONNECTIONS_TYPE_T
typedef struct amqp_connections_t amqp_connections_t;
#endif

#ifndef LIBAMQP_AMQP_FRAME_TYPE_T
#define LIBAMQP_AMQP_FRAME_TYPE_T
typedef struct amqp_frame_t amqp_frame_t;
#endif

enum amqp_connection_protocols
{
    AMQP_PROTOCOL_AMQP = 0x01,
    AMQP_PROTOCOL_SASL = 0x02,
    AMQP_PROTOCOL_TLS = 0x04
};

typedef struct amqp_connection_socket_t amqp_connection_socket_t;

typedef void (*amqp_connection_action_f)(amqp_connection_t *connection);
typedef void (*amqp_connection_read_callback_f)(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount);

typedef void (*amqp_connection_connect_f)(amqp_connection_t *connection, const char *hostname, int port);
typedef void (*amqp_connection_accept_f)(amqp_connection_t *connection, int fd, struct sockaddr_storage *client_address, socklen_t address_size);
typedef void (*amqp_connection_error_f)(amqp_connection_t *connection, int error_code);
typedef void (*amqp_connection_write_f)(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback);
typedef void (*amqp_connection_read_f)(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f done_callback);

typedef void (*amqp_connection_frame_available_f)(amqp_connection_t *connection, amqp_buffer_t *buffer);

typedef struct amqp_connection_writer_state_t
{
    const char *name;
    amqp_connection_action_f enable;
    amqp_connection_write_f commence_write;
    amqp_connection_action_f continue_write;
    void (*stop)(amqp_connection_t *connection, amqp_connection_action_f done_callback);
    amqp_connection_error_f fail;
    amqp_connection_action_f abort;
    amqp_connection_action_f write_complete;
} amqp_connection_writer_state_t;

typedef struct amqp_connection_reader_state_t
{
    const char *name;
    amqp_connection_action_f enable;
    amqp_connection_read_f commence_read;
    amqp_connection_action_f continue_read;
    amqp_connection_action_f reset;             // TODO - need to review implementation, it's rather crap atm
    amqp_connection_action_f stop;
    amqp_connection_error_f fail;
} amqp_connection_reader_state_t;

typedef struct amqp_connection_socket_state_t
{
    const char *name;
    amqp_connection_connect_f connect;
    amqp_connection_action_f shutdown;

    amqp_connection_error_f fail;
    amqp_connection_action_f done;
    amqp_connection_action_f try_connect;
} amqp_connection_socket_state_t;


typedef void (*amqp_connection_fail_callback_f)(amqp_connection_t *connection, int errro_code);
typedef void (*amqp_connection_negotiate_callback_f)(amqp_connection_t *connection, uint32_t version);
typedef void (*amqp_message_dispatch_t)(amqp_connection_t *connection, amqp_frame_t *frame);

typedef struct amqp_connection_negotiator_state_t
{
    const char *name;
    // TODO - rename
    void (*start)(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback, amqp_connection_negotiate_callback_f reject_callback);
    void (*wait)(amqp_connection_t *connection, amqp_connection_negotiate_callback_f request_callback);
    void (*send)(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback);

    amqp_connection_read_callback_f read_done;
    amqp_connection_action_f done;
    amqp_connection_action_f reset;
} amqp_connection_negotiator_state_t;

typedef struct amqp_connection_negotiator_actions_t
{
    amqp_connection_action_f confirm;
    amqp_connection_negotiate_callback_f reject;
    amqp_connection_negotiate_callback_f request;
} amqp_connection_negotiator_actions_t;

typedef struct amqp_connection_tunnel_actions_t
{
    void (*accept)(amqp_connection_t *connection, uint32_t protocol_version);
} amqp_connection_tunnel_actions_t;

typedef struct amqp_connection_tls_state_t
{
    const char *name;
    amqp_connection_action_f connect;   // Client connect
//    amqp_connection_action_f done;
    amqp_connection_tunnel_actions_t tunnel;
} amqp_connection_tls_state_t;

typedef struct amqp_connection_sasl_state_t
{
    const char *name;
    amqp_connection_action_f connect;
    amqp_connection_action_f done;
    amqp_connection_tunnel_actions_t tunnel;
    struct {
        amqp_message_dispatch_t mechanisms;
    } messages;
} amqp_connection_sasl_state_t;

typedef struct amqp_connection_amqp_state_t
{
    const char *name;
    amqp_connection_action_f connect;
    amqp_connection_action_f done;
    amqp_connection_tunnel_actions_t tunnel;
} amqp_connection_amqp_state_t;

typedef struct amqp_connection_frame_reader_state_t
{
    const char *name;
    amqp_connection_action_f enable;
    void (*read)(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_frame_available_f frame_available_callback);
    amqp_connection_read_callback_f read_done;
    amqp_connection_action_f stop;
} amqp_connection_frame_reader_state_t;

typedef struct amqp_connection_state_t
{
    const char *name;
    amqp_connection_action_f hangup;        // Just pull the plug
    amqp_connection_action_f drain;         // Close output and drain input
    amqp_connection_action_f shutdown;      // Allow write complete then pull the plug on any reads
    union {
        struct {
            amqp_connection_connect_f connect;
        } client;
        struct {
            amqp_connection_negotiate_callback_f begin;
            amqp_connection_negotiate_callback_f reject;
        } server;
    } mode;
//    amqp_connection_write_f write;
//    amqp_connection_read_f read;
    amqp_connection_read_callback_f read_done;
    amqp_connection_action_f done;
    amqp_connection_action_f fail;
    amqp_connection_action_f timeout;
} amqp_connection_state_t;

struct amqp_connection_socket_t
{
    const char *hostname;
    int port_number;
    int fd;
    int errno_error_code;
    struct {
        struct {
            struct addrinfo *iterator;
            struct addrinfo *addrinfo;
            int eai_error_code;
        } lookup;
        amqp_io_event_watcher_t *watcher;
    } connect;
    amqp_connection_action_f connected_callback;
};

typedef struct amqp_specification_version_t
{
    uint32_t tls;
    uint32_t sasl;
    uint32_t amqp;
} amqp_specification_version_t;

typedef enum amqp_cs_shutdown_mode_t {
    amqp_cs_complete_write_drain_input_and_close_socket = 2, // TODO - remove
    amqp_cs_complete_write_and_close_socket,
    amqp_cs_close_socket,
    amqp_cs_abort_socket
} amqp_cs_shutdown_mode_t;

struct amqp_connection_t
{
    // TODO - record error code
    amqp_context_t *context;
    amqp_connection_socket_t socket;
    int flags;
    uint32_t failure_flags;
    int trace_flags;
    int protocols;
    struct
    {
        amqp_specification_version_t required;
        amqp_specification_version_t supported;
    } specification_version;
    struct {
        amqp_connection_state_t connection;
        amqp_connection_socket_state_t socket;
        amqp_connection_writer_state_t writer;
        amqp_connection_reader_state_t reader;

        amqp_connection_tls_state_t tls;
        amqp_connection_sasl_state_t sasl;
        amqp_connection_amqp_state_t amqp;

        amqp_connection_negotiator_state_t negotiator;
        amqp_connection_frame_reader_state_t frame;

        amqp_connection_action_f stopped_hook;  // used by tests, not used by SM
    } state;
    union {
        struct {
            int protocols;
            uint32_t preferred_version;
            amqp_connection_negotiator_actions_t callbacks;
        } handshake;
        struct {
            amqp_cs_shutdown_mode_t mode;
        } shutdown;
    } data;
    struct {
        amqp_buffer_t *read;
        amqp_buffer_t *write;
    } buffer;
    struct {
        struct
        {
            amqp_buffer_t *buffer;
            amqp_connection_action_f done_callback;
            amqp_io_event_watcher_t *watcher;
        } write;
        struct
        {
            amqp_buffer_t *buffer;
            amqp_connection_read_callback_f read_callback;
            amqp_io_event_watcher_t *watcher;
            size_t n_required;
            size_t satisfied;
        } read;
        struct {
            amqp_buffer_t *buffer;
            amqp_connection_frame_available_f frame_available_callback;
            uint32_t frame_size;
        } frame;
    } io;
    struct {
        uint32_t max_frame_size;
    } limits;
    amqp_accept_handler_arguments_t *accept_handler_arguments;
    amqp_timer_t *timer;
};

extern void amqp_connection_state_initialize(amqp_connection_t *connection);
extern void amqp_connection_client_state_initialize(amqp_connection_t *connection);
extern void amqp_connection_server_state_initialize(amqp_connection_t *connection, int fd, struct sockaddr_storage *client_address, socklen_t address_size);

extern void amqp_connection_enable_io(amqp_connection_t *connection);

extern void amqp__connection_default_state_initialization(amqp_connection_t *connection, const char *new_state_name);

extern void amqp_connection_state_cleanup(amqp_connection_t *connection);
extern int amqp_connection_is_state(const amqp_connection_t *connection, const char *state_name);

#ifdef __cplusplus
}
#endif
#endif
