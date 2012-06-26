#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"

static minx86_read_ptr_t cip;
#include "minx86dec/x86_core_macros.h"

/* General 386-level decoder.
   Includes decoding of IBTS/XBTS undocumented instructions, which the downstream decoder
   is free to handle, or ignore depending on which "stepping" of the i386 you are emulating.
   Extensions added by others that are in conflict with Intel's decoder are not handled by this core. */

void minx86dec_decode386(struct minx86dec_state *state,struct minx86dec_instruction *ins) {
	unsigned int dataprefix32 = 0,addrprefix32 = 0;
	cip = state->read_ip;
#define core_level 3
#define isdata32 ins->data32
#define isaddr32 ins->addr32
#define fpu_level 3

	ins->data32 = state->data32;
	ins->addr32 = state->addr32;
	ins->start = state->read_ip;
	ins->opcode = MXOP_UD;
	ins->segment = -1;
	ins->argc = 0;

	/* this follows the DOSBox style of core implementation by having one
	 * master header file with decoding logic #included with #defines to
	 * enable/disable functions */

	{
#include "x86_core.h"
	}

	/* invalid opcode. step 1 forward (2 if FPU instruction) */
	if (ins->opcode == MXOP_UD) {
		ins->argc = 0;
		if ((*(state->read_ip) & 0xF8) == 0xD8)
			ins->end = state->read_ip = (ins->start + 2);
		else if (*(state->read_ip) == 0x0F)
			ins->end = state->read_ip = (ins->start + 2);
		else
			ins->end = state->read_ip = (ins->start + 1);
	}
	else {
		ins->end = state->read_ip = cip;
	}
}

