#ifndef STORMMQ_AMQP_MISC_H
#define STORMMQ_AMQP_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

/* for stuff whose true place has to be decided */

static inline int to_hex(int c)
{
    return c + (c < 10 ? '0' : ('a' - 10));
}

extern void break_one();
extern void break_two();

#ifdef __cplusplus
}
#endif
#endif


