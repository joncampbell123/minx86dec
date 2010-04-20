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
#define everything
#define pentium 99
#define emmx_cyrix
#define pentiumpro
#define mmx_amd
#define cpuid
#define ssse3
#define mmx 99
#define vex_level 1

	ins->data32 = state->data32;
	ins->addr32 = state->addr32;
	ins->start = state->read_ip;
	ins->opcode = MXOP_UD;
	ins->segment = -1;
	ins->argc = 0;

	/* bring in the core.
	 * don't bitch about ugliness, this avoid maintaining multiple copies of the same code.
	 * and DOSBox does this too to keep it's various "cores" clean, so there. */
	{
#include "x86_core.h"
	}

	/* invalid opcode. step 1 forward */
	if (ins->opcode == MXOP_UD)
		ins->end = state->read_ip = (ins->start + 1);
	else
		ins->end = state->read_ip = cip;
}

