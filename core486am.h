#ifndef MX86_CORE8086_H
#define MX86_CORE8086_H

#include "minx86dec/state.h"

/* 8088/8086-style decoding, because some quirks don't exist in later versions */
void minx86dec_decode486am(struct minx86dec_state *state,struct minx86dec_instruction *ins);

#endif /* */

