#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"

static minx86_read_ptr_t cip;
#include "minx86dec/x86_core_macros.h"

void minx86dec_decodeall(struct minx86dec_state *state,struct minx86dec_instruction *ins) {
	unsigned int dataprefix32 = 0,addrprefix32 = 0;
	register unsigned int patience = 6;
	cip = state->read_ip;
#define isdata32 ins->data32
#define isaddr32 ins->addr32
#define core_level 99
#define fpu_level 99
#define amd_3dnow 99
#define sse_level 99
#define vex_level 1
#define everything
#define pentium 99
#define emmx_cyrix
#define pentiumpro
#define mmx_amd
#define cpuid
#define ssse3
#define mmx 99

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
		if ((*(state->read_ip) & 0xF8) == 0xD8)
			ins->end = state->read_ip = (ins->start + 2);
		else
			ins->end = state->read_ip = (ins->start + 1);
	}
	else {
		ins->end = state->read_ip = cip;
	}
}

