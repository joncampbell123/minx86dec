#ifndef MX86_CORE8086_H
#define MX86_CORE8086_H

#include "minx86dec/state.h"

/* 8088/8086-style decoding with stream decode, because some quirks don't exist in later versions */
void minx86dec_sdecode8086(struct minx86dec_state *state,struct minx86dec_instruction *ins,void (*fetch8)(struct minx86dec_state *ctx));

#endif /* */

