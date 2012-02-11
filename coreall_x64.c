#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"

static minx86_read_ptr_t cip;
#include "minx86dec/x64_core_macros.h"

/* NOTE: x64 decoding seems to demand that 32-bit data is on by default. */

void minx86dec_decodeall_x64(struct minx86dec_state_x64 *state,struct minx86dec_instruction_x64 *ins) {
	unsigned int dataprefix32 = 0,addrprefix32 = 0;
	register unsigned int patience = 6;
	cip = state->read_ip;
#define isdata64 ins->data64
#define isdata32 ins->data32
#define isaddr32 ins->addr32
#define core_level 99
#define fpu_level 99
#define amd_3dnow 99
#define sse_level 99
#define vex_level 1
#define pentium 99
#define emmx_cyrix
#define pentiumpro
#define everything
#define mmx_amd
#define cpuid
#define ssse3
#define mmx 99

	ins->data64 = 0;
	ins->data32 = 1;
	ins->addr32 = 0;
	ins->start = state->read_ip;
	ins->opcode = MXOP_UD;
	ins->segment = -1;
	ins->argc = 0;

	/* this follows the DOSBox style of core implementation by having one
	 * master header file with decoding logic #included with #defines to
	 * enable/disable functions */

	{
#define x64_mode
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

