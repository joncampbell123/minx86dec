#ifndef MX86_CORE8086_H
#define MX86_CORE8086_H

#include "minx86dec/state.h"

/* general decoding of all instructions. not trying to limit to realistic CPU model */
/* use this for a general decompiler and you don't fucking care what model :) */
void minx86dec_decodeall(struct minx86dec_state *state,struct minx86dec_instruction *ins);

#endif /* */

