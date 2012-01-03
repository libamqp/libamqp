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

#include <assert.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "Buffer/Buffer.h"
#include "Buffer/BufferInternal.h"
#include "Memory/Memory.h"
#include "Misc/Bits.h"
#include "Context/Context.h"

#include "debug_helper.h"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL    0
#endif

int amqp_buffer_write(int fd, amqp_buffer_t *buffer)
{
    int iov_count;
    struct iovec *io_vec = amqp_buffer_read_io_vec(buffer, &iov_count);
    struct msghdr header = {
        .msg_name = 0,
        .msg_namelen = 0,
        .msg_iov = io_vec,
        .msg_iovlen = iov_count,
        .msg_control = 0,
        .msg_controllen = 0,
        .msg_flags = 0
    };
    
    return sendmsg(fd, &header, MSG_NOSIGNAL);
}