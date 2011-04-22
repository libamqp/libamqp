#ifndef STORMMQ_AMQP_TYPE_TYPE_VALIDATE_H
#define STORMMQ_AMQP_TYPE_TYPE_VALIDATE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "amqp.h"
#include "Context/Context.h"
#include "Type/Type.h"

inline static int amqp_validate_char(wchar_t c)
{
    return c < 0x000d800 || (c > 0x0000dfff && c <= 0x0010ffff);
}

#ifdef __cplusplus
}
#endif
#endif


