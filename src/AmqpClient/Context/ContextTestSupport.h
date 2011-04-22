#ifndef STORMMQ_AMQP_CONTEXT_TEST_SUPPORT_H
#define STORMMQ_AMQP_CONTEXT_TEST_SUPPORT_H

#include "Buffer/BufferTestSupport.h"
#include "Context/Context.h"

namespace t
{
    class Context
    {
    public:
        Context();
        Context(const ByteArray &buffer);
        ~Context();
        void transfer_from(const ByteArray &buffer);

        amqp_context_t *context() const { return context_; }
        amqp_buffer_t *buffer() const { return context_->decode.buffer; }

        void transfer_from(ByteArray& bytes) { bytes.transfer_to(context_->decode.buffer);}

    private:
        amqp_context_t *context_;
    };
}

#endif

