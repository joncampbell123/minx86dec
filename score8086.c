#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"

#define streaming_decode
#define streaming_decode_fetch8 fetch8

static minx86_read_ptr_t cip;
static struct minx86dec_state *state;
static void (*fetch8)(struct minx86dec_state *ctx);
#include "minx86dec/x86_core_macros.h"

void minx86dec_sdecode8086(struct minx86dec_state *_state,struct minx86dec_instruction *ins,void (*_fetch8)(struct minx86dec_state *ctx)) {
	state = _state;
	cip = state->read_ip;
	fetch8 = _fetch8;
#define core_level 0
#define isdata32 0 /* no 386-style 32-bit */
#define isaddr32 0 /* no 386-style 32-bit */
#define no_32
#define fpu_level 0

	/* default to 8086-style invalid opcode (apparently there was no invalid opcode exception?) */
	ins->start = state->read_ip;
	ins->segment = -1;
	ins->opcode = MXOP_UD_NOP;
	ins->argc = 0;

	/* this follows the DOSBox style of core implementation by having one
	 * master header file with decoding logic #included with #defines to
	 * enable/disable functions */

	{
#include "x86_core.h"
	}

	/* the 8088 simply treats invalid opcodes as two-byte NO-OPs */
	if (ins->opcode == MXOP_UD_NOP) {
		ins->argc = 0;
		ins->end = state->read_ip = (ins->start + 2);
	}
	else {
		ins->end = state->read_ip = cip;
	}
}

