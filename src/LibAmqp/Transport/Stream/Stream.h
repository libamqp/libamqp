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

#ifndef LIBAMQP_FRAMING_STREAM_H
#define LIBAMQP_FRAMING_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_STREAM_T_TYPE
#define LIBAMQP_STREAM_T_TYPE
typedef struct amqp_stream_t amqp_stream_t;
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

extern amqp_stream_t *amqp_buffer_stream_create(amqp_context_t *context, amqp_buffer_t *buffer);
extern amqp_stream_t *amqp_socket_stream_create(amqp_context_t *context, int fd);

extern void amqp_destroy_stream(amqp_context_t *context, amqp_stream_t *stream);

extern int amqp_stream_read(amqp_stream_t *stream, unsigned char *buffer, size_t bufsiz, size_t offset, int n);
extern int amqp_stream_write(amqp_stream_t *stream, unsigned char *buffer, int n);

#ifdef __cplusplus
}
#endif
#endif
