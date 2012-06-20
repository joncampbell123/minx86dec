/* IIT 3C87 (387 compatible) "auxililary decode".
   I'd rather not muck up the Makefile to compile every core twice just to accomodate
   minor variations in instruction set, it's simpler to have the decoding routine
   written to check for failure to decode instruction and give it a "second try" here
   to catch and decode IIT FPU instructions. Also, it is said that the 3C87 was designed
   to be a drop-in replacement for the 387 FPU on Intel/AMD systems, IBM 486SLC, and
   Cyrix based PCs, so we allow the caller to represent the different FPUs through
   alternate "aux" decoding rather than yet another core.

   NOTE: Decoding IIT FPU instructions is only worthwhile for the 286, 386, and 486
         based cores. The Pentium has it's own FPU so it's highly unlikely the caller
         would decode or emulate a Pentium + IIT387 combination. For P6 (Pentium Pro
         and Pentium II) or later cores, this code will never get called because the
         IIT 3C87 opcodes here overlap with the FUCOMI/FCOMI/etc. instructions that
         Intel added at that revision of the architecture. If the emulator/decompiler
         specializes in Pentium or higher cores, then this code should not be called
         and should not be linked into the final binary. */

#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/core386.h"
#include "minx86dec/opcodes_str.h"
#include "minx86dec/iit387.h"
#include <string.h>
#include <stdio.h>

void minx86dec_auxdecode387iit(struct minx86dec_state *st,struct minx86dec_instruction *i) {
#define FPU_CODE(fb,sb)	((((fb)&7)<<8)|(sb))

	switch (i->fpu_code) {
		case FPU_CODE(0xDB,0xF1): i->opcode = MXOP_F4X4; break;
		case FPU_CODE(0xDB,0xE8): i->opcode = MXOP_FSBP0; break;
		case FPU_CODE(0xDB,0xEB): i->opcode = MXOP_FSBP1; break;
		case FPU_CODE(0xDB,0xEA): i->opcode = MXOP_FSBP2; break;
	}

#undef FPU_CODE
}

