#include <stdlib.h>
#include <string.h>

#include "Context/ContextTestSupport.h"

namespace t
{
    Context::Context(const ByteArray &buffer)
    {
        context_ = amqp_new_amqp_context_t();
        buffer.transfer_to(context_->decode.buffer);
    }
    
    Context::Context()
    {
        context_ = amqp_new_amqp_context_t();
    }

    Context::~Context()
    {
        amqp_delete_amqp_context_t(context_);
    }

    void Context::transfer_from(const ByteArray &buffer)
    {
        amqp_buffer_reset(context_->decode.buffer);
        buffer.transfer_to(context_->decode.buffer);
    }


}
