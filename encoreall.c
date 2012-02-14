#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/coreall.h"
#include "minx86dec/opcodes_str.h"
#include "minx86dec/encoreall.h"
#include <string.h>
#include <stdio.h>

static inline minx86_write_ptr_t minx86enc_32_sse_mod(struct minx86dec_argv *a,struct minx86enc_state *est,minx86_write_ptr_t o,unsigned int wordsize) {
	if (a->regtype == MX86_RT_NONE && (a->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
	if ((wordsize?1:0)/*^(est->data32?1:0)*/) *o++ = 0x66;
	return o;
}

/* code will call this to generate the appropriate 386 data override prefix.
 * if 16-bit mode and the arg provided is 32 bits wide, OR
 * if 32-bit mode and the arg provided is 16 bits wide */
static inline minx86_write_ptr_t minx86enc_32_overrides(struct minx86dec_argv *a,struct minx86enc_state *est,minx86_write_ptr_t o,unsigned int wordsize) {
	if (a->regtype == MX86_RT_NONE && (a->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
	if (wordsize && ((a->size>>2)^(est->data32?1:0))) *o++ = 0x66;
	return o;
}

static inline minx86_write_ptr_t minx86enc_32_overrides_far(struct minx86dec_argv *a,struct minx86enc_state *est,minx86_write_ptr_t o,unsigned int wordsize) {
	if (a->regtype == MX86_RT_NONE && (a->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
	if (wordsize && (((a->size==6)?1:0)^(est->data32?1:0))) *o++ = 0x66;
	return o;
}

minx86_write_ptr_t minx86enc_encode_rm_reg(struct minx86dec_argv *a,unsigned int reg,unsigned int rm,minx86_write_ptr_t o) {
	*o++ = (3 << 6) | (reg << 3) | rm;
	return o;
}

minx86_write_ptr_t minx86enc_seg_overrides(struct minx86dec_argv *a,struct minx86enc_state *est,minx86_write_ptr_t o,int force_seg_override) {
	int need_ds = 0,i;

	if (a->memregs != 0) {
		/* if any reference is made to EBP/ESP then assume you need DS to encode DS ref */
		for (i=0;i < a->memregs;i++) {
			if (a->memreg[i] == MX86_REG_SP || a->memreg[i] == MX86_REG_BP)
				need_ds = 1;
		}
	}

	if (a->segment == MX86_SEG_DS) {
		if (force_seg_override || need_ds) *o++ = 0x3E;	/* DS: */
	}
	else if (a->segment == MX86_SEG_SS) {
		if (force_seg_override || !need_ds) *o++ = 0x36;	/* SS: */
	}
	else if (a->segment == MX86_SEG_ES) {
		*o++ = 0x26;
	}
	else if (a->segment == MX86_SEG_CS) {
		*o++ = 0x2E;
	}
	else if (a->segment == MX86_SEG_FS) {
		*o++ = 0x64;
	}
	else if (a->segment == MX86_SEG_GS) {
		*o++ = 0x65;
	}

	return o;
}

minx86_write_ptr_t minx86enc_seg_overrides_es(struct minx86dec_argv *a,struct minx86enc_state *est,minx86_write_ptr_t o,int force_seg_override) {
	if (a->segment == MX86_SEG_DS) {
		*o++ = 0x3E;	/* DS: */
	}
	else if (a->segment == MX86_SEG_SS) {
		*o++ = 0x36;	/* SS: */
	}
	else if (a->segment == MX86_SEG_ES) {
		if (force_seg_override) *o++ = 0x26;
	}
	else if (a->segment == MX86_SEG_CS) {
		*o++ = 0x2E;
	}
	else if (a->segment == MX86_SEG_FS) {
		*o++ = 0x64;
	}
	else if (a->segment == MX86_SEG_GS) {
		*o++ = 0x65;
	}

	return o;
}

minx86_write_ptr_t minx86enc_encode_memreg_far(struct minx86dec_argv *a,minx86_write_ptr_t o,unsigned int regval) {
	int mod = 0,sib = -1,memref = 0;

	if (a->memref_base != 0) {
		if (	(a->memregsz == 4 && (int32_t)a->memref_base >= -0x80 && (int32_t)a->memref_base < 0x80) ||
			(a->memregsz == 2 && (int16_t)a->memref_base >= -0x80 && (int16_t)a->memref_base < 0x80))
			mod = 1;
		else
			mod = 2;
	}

	if (a->memregs == 1) {
		if (a->memregsz == 4) {/* 32-bit */
			if (mod == 0) {
				if (a->memreg[0] == 5)
					mod = 1;	/* [EBP] -> [EBP+0] */
			}
			*o++ = (mod << 6) | (regval << 3) | a->memreg[0];
			{
				if (a->memreg[0] == 4) {
					/* the code for [ESP] is used instead for
					 * encoding the SIB so we must encode the SIB byte
					 * to properly make [ESP] reference */
					*o++ = (a->scalar << 6) | (4 << 3) | MX86_REG_ESP; /* scale=s/index=none/base=ESP */
				}
			}
		}
		else {/* 16-bit */
			switch (a->memreg[0]) {
				case MX86_REG_SI:	*o++ = 4 | (regval << 3) | (mod<<6); break;
				case MX86_REG_DI:	*o++ = 5 | (regval << 3) | (mod<<6); break;
				case MX86_REG_BP: {
					if (mod == 0) mod = 1;	/* No such [BP] must encode as [BP+0] */
					*o++ = 6 | (regval << 3) | (mod<<6); } break;
				case MX86_REG_BX:	*o++ = 7 | (regval << 3) | (mod<<6); break;
			}
		}
	}
	else if (a->memregs == 2) {
		if (a->memregsz == 4) {/* 32-bit */
			if (mod == 0) {
				if (a->memreg[0] == 5 || a->memreg[1] == 5)
					mod = 1;	/* [EBP] -> [EBP+0] */
			}
			*o++ = (mod << 6) | (regval << 3) | 4;	/* SIB */
			*o++ = (a->scalar << 6) | (a->memreg[0] << 3) | a->memreg[1]; /* scale=s/index=none/base=ESP */
		}
		else {/* 16-bit */
			uint8_t c = 0;

			/* BX or BP */
			switch (a->memreg[0]) {
				case MX86_REG_BP:	c = 2; break;
			}
			/* second one may be SI or DI */
			switch (a->memreg[1]) {
				case MX86_REG_DI:	c |= 1; break;
			}

			*o++ = c | (regval << 3) | (mod<<6);
		}
	}
	else if (a->memregs == 0) {
		/* direct memory reference */
		mod = 0;
		memref = 1;
		if (a->memregsz == 4) {/* 32-bit */
			*o++ = 5 | (regval << 3) | (mod<<6);
		}
		else {
			*o++ = 6 | (regval << 3) | (mod<<6);
		}
	}

	if (mod == 1)
		*o++ = (uint8_t)(a->memref_base);
	else if (mod == 2 || (mod == 0 && memref)) {
		if (a->memregsz == 4) {
			*((uint32_t*)o) = (uint32_t)(a->memref_base);
			o += 4;
		}
		else {
			*((uint16_t*)o) = (uint16_t)(a->memref_base);
			o += 2;
		}
	}

	return o;
}

minx86_write_ptr_t minx86enc_encode_memreg(struct minx86dec_argv *a,minx86_write_ptr_t o,unsigned int regval) {
	int mod = 0,sib = -1,memref = 0;

	if (a->memref_base != 0) {
		if (	(a->memregsz == 4 && (int32_t)a->memref_base >= -0x80 && (int32_t)a->memref_base < 0x80) ||
			(a->memregsz == 2 && (int16_t)a->memref_base >= -0x80 && (int16_t)a->memref_base < 0x80))
			mod = 1;
		else
			mod = 2;
	}

	if (a->memregs == 1) {
		if (a->memregsz == 4) {/* 32-bit */
			if (mod == 0) {
				if (a->memreg[0] == 5)
					mod = 1;	/* [EBP] -> [EBP+0] */
			}
			*o++ = (mod << 6) | (regval << 3) | a->memreg[0];
			{
				if (a->memreg[0] == 4) {
					/* the code for [ESP] is used instead for
					 * encoding the SIB so we must encode the SIB byte
					 * to properly make [ESP] reference */
					*o++ = (a->scalar << 6) | (4 << 3) | MX86_REG_ESP; /* scale=s/index=none/base=ESP */
				}
			}
		}
		else {/* 16-bit */
			switch (a->memreg[0]) {
				case MX86_REG_SI:	*o++ = 4 | (regval << 3) | (mod<<6); break;
				case MX86_REG_DI:	*o++ = 5 | (regval << 3) | (mod<<6); break;
				case MX86_REG_BP: {
					if (mod == 0) mod = 1;	/* No such [BP] must encode as [BP+0] */
					*o++ = 6 | (regval << 3) | (mod<<6); } break;
				case MX86_REG_BX:	*o++ = 7 | (regval << 3) | (mod<<6); break;
			}
		}
	}
	else if (a->memregs == 2) {
		if (a->memregsz == 4) {/* 32-bit */
			if (mod == 0) {
				if (a->memreg[1] == 5)
					mod = 1;	/* [EBP] -> [EBP+0] */
			}
			*o++ = (mod << 6) | (regval << 3) | 4;	/* SIB */
			*o++ = (a->scalar << 6) | (a->memreg[0] << 3) | a->memreg[1]; /* scale=s/index=none/base=ESP */
		}
		else {/* 16-bit */
			uint8_t c = 0;

			/* BX or BP */
			switch (a->memreg[0]) {
				case MX86_REG_BP:	c = 2; break;
			}
			/* second one may be SI or DI */
			switch (a->memreg[1]) {
				case MX86_REG_DI:	c |= 1; break;
			}

			*o++ = c | (regval << 3) | (mod<<6);
		}
	}
	else if (a->memregs == 0) {
		/* direct memory reference */
		mod = 0;
		memref = 1;
		if (a->memregsz == 4) {/* 32-bit */
			*o++ = 5 | (regval << 3) | (mod<<6);
		}
		else {
			*o++ = 6 | (regval << 3) | (mod<<6);
		}
	}

	if (mod == 1)
		*o++ = (uint8_t)(a->memref_base);
	else if (mod == 2 || (mod == 0 && memref)) {
		if (a->memregsz == 4) {
			*((uint32_t*)o) = (uint32_t)(a->memref_base);
			o += 4;
		}
		else {
			*((uint16_t*)o) = (uint16_t)(a->memref_base);
			o += 2;
		}
	}

	return o;
}

void minx86enc_encodeall(struct minx86enc_state *est,struct minx86dec_instruction *ins) {
	minx86_write_ptr_t o = est->write_ip;
	est->started_here = o;

	switch (ins->opcode) {
		case MXOP_JMP: { /*====================NEAR JMP===================*/
			struct minx86dec_argv *a = &ins->argv[0];
			if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0xFF; *o++ = (3<<6) | (4<<3) | a->reg;	/* mod=3 reg=4 rm=reg */
			}
			else if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0xFF; o = minx86enc_encode_memreg(a,o,4);
			}
			else if (a->regtype == MX86_RT_IMM) { /* hope you set est->ip_value! encoding is RELATIVE! */
				int32_t delta = (int32_t)(a->value - est->ip_value),extra = (int32_t)(o - est->started_here);
				/* if it's small enough, encode as single-byte JMP */
				if ((delta-(2+extra)) >= -0x80 && (delta-(2+extra)) < 0x80)
					{ /*o = minx86enc_32_overrides(a,est,o,1);*/ *o++ = 0xEB; *o++ = (uint8_t)(delta-(2+extra)); }
				/* if the encoding is for 32-bit mode, OR the delta is too large for 16-bit mode: */
				else if (est->addr32 || !((delta-(5+extra)) >= -0x8000 && (delta-(5+extra)) < 0x8000))
					{ if (!est->addr32) { *o++ = 0x66; extra++; }; *o++ = 0xE9; *((uint32_t*)o) = (uint32_t)(delta-(5+extra)); o += 4; }
				else
					{ *o++ = 0xE9; *((uint16_t*)o) = (uint16_t)(delta-(3+extra)); o += 2; }
			}
		} break;
		case MXOP_JMP_FAR: { /*=====================FAR JMP====================*/
			struct minx86dec_argv *ofs=&ins->argv[0];
			if (ofs->regtype == MX86_RT_IMM) { /* easy: seg:off values are absolute */
				if (est->addr32 || (ofs->value & 0xFFFF0000)) {
					if (!est->addr32) *o++ = 0x66;
					*o++ = 0xEA;
					*((uint32_t*)o) = (uint32_t)ofs->value; o += 4;
				}
				else {
					*o++ = 0xEA;
					*((uint16_t*)o) = (uint16_t)ofs->value; o += 2;
				}

				/* segment */
				*((uint16_t*)o) = (uint16_t)ofs->segval; o += 2;
			}
			else if (ofs->regtype == MX86_RT_NONE) {
				o = minx86enc_32_overrides_far(ofs,est,o,1);
				*o++ = 0xFF; o = minx86enc_encode_memreg_far(ofs,o,5);
			}
		} break;
		case MXOP_CALL: { /*====================NEAR CALL===================*/
			struct minx86dec_argv *a = &ins->argv[0];
			if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0xFF; *o++ = (3<<6) | (2<<3) | a->reg;	/* mod=3 reg=6 rm=reg */
			}
			else if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0xFF; o = minx86enc_encode_memreg(a,o,2);
			}
			else if (a->regtype == MX86_RT_IMM) { /* hope you set est->ip_value! encoding is RELATIVE! */
				int32_t delta = (int32_t)(a->value - est->ip_value),extra = (int32_t)(o - est->started_here);
				/* no such thing as single-byte CALL. */
				/* if the encoding is for 32-bit mode, OR the delta is too large for 16-bit mode: */
				if (est->addr32 || !((delta-(5+extra)) >= -0x8000 && (delta-(5+extra)) < 0x8000))
					{ if (!est->addr32) { *o++ = 0x66; extra++;}; *o++ = 0xE8; *((uint32_t*)o) = (uint32_t)(delta-(5+extra)); o += 4; }
				else
					{ *o++ = 0xE8; *((uint16_t*)o) = (uint16_t)(delta-(3+extra)); o += 2; }
			}
		} break;
		case MXOP_CALL_FAR: { /*=====================FAR CALL====================*/
			struct minx86dec_argv *ofs=&ins->argv[0];
			if (ofs->regtype == MX86_RT_IMM) { /* easy: seg:off values are absolute */
				if (est->addr32 || (ofs->value & 0xFFFF0000)) {
					if (!est->addr32) *o++ = 0x66;
					*o++ = 0x9A;
					*((uint32_t*)o) = (uint32_t)ofs->value; o += 4;
				}
				else {
					*o++ = 0x9A;
					*((uint16_t*)o) = (uint16_t)ofs->value; o += 2;
				}

				/* segment */
				*((uint16_t*)o) = (uint16_t)ofs->segval; o += 2;
			}
			else if (ofs->regtype == MX86_RT_NONE) {
				o = minx86enc_32_overrides_far(ofs,est,o,1);
				*o++ = 0xFF; o = minx86enc_encode_memreg_far(ofs,o,3);
			}
		} break;
		case MXOP_NOP: { /*====================NOP====================*/
			struct minx86dec_argv *a=&ins->argv[0];
			if (ins->lock) *o++ = 0xF0;

			if (ins->argc == 0) {
				*o++ = 0x90;
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0x1F;
				*o++ = (3 << 6) | (0 << 3) | a->reg;
			}
			else if (a->regtype == MX86_RT_NONE && a->memregs > 0) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0x1F;
				o = minx86enc_encode_memreg(a,o,0);
			}
			else {
				*o++ = 0x90;
			}
		} break;
		case MXOP_XCHG: { /*========================XCHG======================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; }
			if (ins->lock) *o++ = 0xF0;
			o = minx86enc_32_overrides(a,est,o,word);

			if (a->regtype == MX86_RT_REG) {
				if (word && a->reg == MX86_REG_AX)
					*o++ = 0x90 + b->reg;
				else if (word && b->reg == MX86_REG_AX)
					*o++ = 0x90 + a->reg;
				else {
					*o++ = 0x86 + word;
					*o++ = (3 << 6) | (b->reg << 3) | a->reg;
				}
			}
			else { /* xchg r/m, reg */
				*o++ = 0x86 + word; o = minx86enc_encode_memreg_far(a,o,b->reg);
			}
		} break;
		case MXOP_MOV: { /*====================MOV=====================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			/* it doesn't matter if it's reg-reg, reg-r/m, r/m-reg, etc
			 * instruction encoding covers them all */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x88+word; *o++ = (3<<6) | (b->reg<<3) | (a->reg);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_IMM) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0xB0+(word<<3)+a->reg;
				if (word) {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)(b->value); o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)(b->value); o += 2;
					}
				}
				else {
					*o++ = (uint8_t)(b->value);
				}
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_IMM) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0xC6 + word; o = minx86enc_encode_memreg(a,o,0);
				if (word) {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)(b->value); o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)(b->value); o += 2;
					}
				}
				else {
					*o++ = (uint8_t)(b->value);
				}
			}
			else if (a->regtype == MX86_RT_NONE && a->memregs == 0 && b->regtype == MX86_RT_REG && b->reg == MX86_REG_AX) {
				/* aka: MOV [memaddr],A */
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0xA2 + word;
				if (a->memregsz == 4) {
					*((uint32_t*)o) = (uint32_t)(a->memref_base); o += 4;
				}
				else {
					*((uint16_t*)o) = (uint16_t)(a->memref_base); o += 2;
				}
			}
			else if (b->regtype == MX86_RT_NONE && b->memregs == 0 && a->regtype == MX86_RT_REG && a->reg == MX86_REG_AX) {
				/* aka: MOV A,[memaddr] */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,word);
				*o++ = 0xA0 + word;
				if (b->memregsz == 4) {
					*((uint32_t*)o) = (uint32_t)(b->memref_base); o += 4;
				}
				else {
					*((uint16_t*)o) = (uint16_t)(b->memref_base); o += 2;
				}
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x88 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			else if (b->regtype == MX86_RT_NONE && a->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,word);
				*o++ = 0x8A + word; o = minx86enc_encode_memreg(b,o,a->reg);
			}
			else if (a->regtype == MX86_RT_SREG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(b,est,o,1);
				*o++ = 0x8E; *o++ = (3 << 6) | (a->reg << 3) | b->reg;
			}
			else if (a->regtype == MX86_RT_SREG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,1);
				*o++ = 0x8E; o = minx86enc_encode_memreg(b,o,a->reg);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_SREG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x8C; *o++ = (3 << 6) | (b->reg << 3) | a->reg;
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_SREG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x8C; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* control regs: apparently there is no valid encoding for mov cr0,r/m or such */
			else if (a->regtype == MX86_RT_CR && b->regtype == MX86_RT_REG) {
				*o++ = 0x0F; *o++ = 0x22; *o++ = (3 << 6) | (a->reg << 3) | b->reg;
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_CR) {
				*o++ = 0x0F; *o++ = 0x20; *o++ = (3 << 6) | (b->reg << 3) | a->reg;
			}
			else if (a->regtype == MX86_RT_DR && b->regtype == MX86_RT_REG) {
				*o++ = 0x0F; *o++ = 0x23; *o++ = (3 << 6) | (a->reg << 3) | b->reg;
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_DR) {
				*o++ = 0x0F; *o++ = 0x21; *o++ = (3 << 6) | (b->reg << 3) | a->reg;
			}
			else if (a->regtype == MX86_RT_TR && b->regtype == MX86_RT_REG) {
				*o++ = 0x0F; *o++ = 0x26; *o++ = (3 << 6) | (a->reg << 3) | b->reg;
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_TR) {
				*o++ = 0x0F; *o++ = 0x24; *o++ = (3 << 6) | (b->reg << 3) | a->reg;
			}
		} break;
		case MXOP_PUSH: { /*====================PUSH=====================*/
			struct minx86dec_argv *a=&ins->argv[0];

			if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x50+a->reg;
			}
			else if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0xFF; o = minx86enc_encode_memreg(a,o,6);
			}
			else if (a->regtype == MX86_RT_IMM) {
				if (a->value & 0xFFFF0000UL) { /* need 32-bit encoding */
					if (!est->data32) *o++ = 0x66;
					*o++ = 0x68;
					*((uint32_t*)o) = (uint32_t)(a->value); o += 4;
				}
				else if (a->value & 0xFFFFFF00UL) { /* need 16-bit encoding */
					if (est->data32) *o++ = 0x66;
					*o++ = 0x68;
					*((uint16_t*)o) = (uint16_t)(a->value); o += 2;
				}
				else {
					*o++ = 0x6A; *o++ = (uint8_t)(a->value);
				}
			}
			else if (a->regtype == MX86_RT_SREG) {
				switch (a->reg) {
					case MX86_SEG_CS:	*o++ = 0x0E;	break;
					case MX86_SEG_DS:	*o++ = 0x1E;	break;
					case MX86_SEG_ES:	*o++ = 0x06;	break;
					case MX86_SEG_FS:	*o++ = 0x0F; *o++ = 0xA0; break;
					case MX86_SEG_GS:	*o++ = 0x0F; *o++ = 0xA8; break;
					case MX86_SEG_SS:	*o++ = 0x16;	break;
				}
			}
		} break;
		case MXOP_POP: { /*====================POP=====================*/
			struct minx86dec_argv *a=&ins->argv[0];

			if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x58+a->reg;
			}
			else if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x8F; o = minx86enc_encode_memreg(a,o,0);
			}
			else if (a->regtype == MX86_RT_SREG) {
				switch (a->reg) {
					case MX86_SEG_CS:	*o++ = 0x0F;	break;
					case MX86_SEG_DS:	*o++ = 0x1F;	break;
					case MX86_SEG_ES:	*o++ = 0x07;	break;
					case MX86_SEG_FS:	*o++ = 0x0F; *o++ = 0xA1; break;
					case MX86_SEG_GS:	*o++ = 0x0F; *o++ = 0xA9; break;
					case MX86_SEG_SS:	*o++ = 0x17;	break;
				}
			}
		} break;
		case MXOP_INC: { /*====================INC=====================*/
			struct minx86dec_argv *a=&ins->argv[0];
			unsigned char word = (a->size >= 2) ? 1 : 0;

			if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				if (word) {
					/* NTS: 32-bit only encoding. This encoding is not valid in x86-64 */
					*o++ = 0x40+a->reg;
				}
				else {
					*o++ = 0xFE;
					o = minx86enc_encode_rm_reg(a,0/*INC*/,a->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_NONE) {
				/* NTS: You must also use this encoding in x86_64, or with BYTE register references */
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0xFE + word; o = minx86enc_encode_memreg(a,o,0);
			}
		} break;
		case MXOP_DEC: { /*====================DEC=====================*/
			struct minx86dec_argv *a=&ins->argv[0];
			unsigned char word = (a->size >= 2) ? 1 : 0;

			if (a->regtype == MX86_RT_REG) {
				if (word) {
					/* NTS: 32-bit only encoding. This encoding is not valid in x86-64 */
					o = minx86enc_32_overrides(a,est,o,1);
					*o++ = 0x48+a->reg;
				}
				else {
					*o++ = 0xFE;
					o = minx86enc_encode_rm_reg(a,1/*DEC*/,a->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_NONE) {
				/* NTS: You must also use this encoding in x86_64, or with BYTE register references */
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0xFE + word; o = minx86enc_encode_memreg(a,o,1);
			}
		} break;
		case MXOP_IDIV: { /*====================IDIV=====================*/
			struct minx86dec_argv *a=&ins->argv[0];
			unsigned char word = (a->size >= 2) ? 1 : 0;

			if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0xF6 + word;
				o = minx86enc_encode_rm_reg(a,7/*IDIV*/,a->reg,o);
			}
			else if (a->regtype == MX86_RT_NONE) {
				/* NTS: You must also use this encoding in x86_64, or with BYTE register references */
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0xF6 + word; o = minx86enc_encode_memreg(a,o,7);
			}
		} break;
		case MXOP_IMUL: { /*====================IMUL=====================*/
			struct minx86dec_argv *a=&ins->argv[0];
			unsigned char word = (a->size >= 2) ? 1 : 0;

			if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0xF6 + word;
				o = minx86enc_encode_rm_reg(a,5/*IMUL*/,a->reg,o);
			}
			else if (a->regtype == MX86_RT_NONE) {
				/* NTS: You must also use this encoding in x86_64, or with BYTE register references */
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0xF6 + word; o = minx86enc_encode_memreg(a,o,5);
			}
		} break;
		case MXOP_LEA: { /*=====================LEA========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,1);
				*o++ = 0x8D; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_TEST: { /*=====================TEST========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			/* NTS: TEST only ANDs the two operands together and updates the flags, it doesn't matter the order.
			 *      so most assemblers like NASM will encode the one [mem],reg form no matter what order you put the operands in */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; }

			/* TEST [mem],reg or TEST reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x84 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x84 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX) {
					*o++ = 0xA8 + (word&1);
				}
				else {
					*o++ = 0xF6 + (word&1);
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,0,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,0);
				}

				if (!word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}

		} break;
		case MXOP_ADD: { /*=====================ADD========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;

			/* lock prefix */
			if (ins->lock) *o++ = 0xF0;

			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x00 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x00 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				unsigned char shorthand = 0;
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (word && (b->value >= 0xFFFFFF80 || (b->value >= 0xFF80 && a->size == 2) || b->value < 0x80)) shorthand = 2;
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX && !shorthand) {
					*o++ = 0x04 + (word&1);
				}
				else {
					*o++ = 0x80 + (word&1) + shorthand;
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,0,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,0);
				}

				if (shorthand || !word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}
		} break;
		case MXOP_ADC: { /*=====================ADC========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x10 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x10 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				unsigned char shorthand = 0;
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (word && (b->value >= 0xFFFFFF80 || (b->value >= 0xFF80 && a->size == 2) || b->value < 0x80)) shorthand = 2;
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX && !shorthand) {
					*o++ = 0x14 + (word&1);
				}
				else {
					*o++ = 0x80 + (word&1) + shorthand;
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,2,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,2);
				}

				if (shorthand || !word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}
		} break;
		case MXOP_AND: { /*=====================AND========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x20 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x20 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				unsigned char shorthand = 0;
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (word && (b->value >= 0xFFFFFF80 || (b->value >= 0xFF80 && a->size == 2) || b->value < 0x80)) shorthand = 2;
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX && !shorthand) {
					*o++ = 0x24 + (word&1);
				}
				else {
					*o++ = 0x80 + (word&1) + shorthand;
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,4,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,4);
				}

				if (shorthand || !word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}
		} break;
		case MXOP_CMP: { /*=====================CMP========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x38 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x38 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				unsigned char shorthand = 0;
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (word && (b->value >= 0xFFFFFF80 || (b->value >= 0xFF80 && a->size == 2) || b->value < 0x80)) shorthand = 2;
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX && !shorthand) {
					*o++ = 0x3C + (word&1);
				}
				else {
					*o++ = 0x80 + (word&1) + shorthand;
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,7,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,7);
				}

				if (shorthand || !word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}
		} break;
		case MXOP_SBB: { /*=====================SBB========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x18 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x18 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				unsigned char shorthand = 0;
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (word && (b->value >= 0xFFFFFF80 || (b->value >= 0xFF80 && a->size == 2) || b->value < 0x80)) shorthand = 2;
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX && !shorthand) {
					*o++ = 0x1C + (word&1);
				}
				else {
					*o++ = 0x80 + (word&1) + shorthand;
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,3,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,3);
				}

				if (shorthand || !word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}
		} break;
		case MXOP_SUB: { /*=====================SUB========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x28 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x28 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				unsigned char shorthand = 0;
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (word && (b->value >= 0xFFFFFF80 || (b->value >= 0xFF80 && a->size == 2) || b->value < 0x80)) shorthand = 2;
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX && !shorthand) {
					*o++ = 0x2C + (word&1);
				}
				else {
					*o++ = 0x80 + (word&1) + shorthand;
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,5,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,5);
				}

				if (shorthand || !word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}
		} break;
		case MXOP_XOR: { /*=====================XOR========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x30 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x30 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			/* CMP reg/rm,imm */
			else if ((a->regtype == MX86_RT_NONE || a->regtype == MX86_RT_REG) && b->regtype == MX86_RT_IMM) {
				unsigned char shorthand = 0;
				if (a->regtype == MX86_RT_NONE) o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				if (word && (b->value >= 0xFFFFFF80 || (b->value >= 0xFF80 && a->size == 2) || b->value < 0x80)) shorthand = 2;
				if (a->regtype == MX86_RT_REG && a->reg == MX86_REG_EAX && !shorthand) {
					*o++ = 0x34 + (word&1);
				}
				else {
					*o++ = 0x80 + (word&1) + shorthand;
					if (a->regtype == MX86_RT_REG)
						o = minx86enc_encode_rm_reg(a,6,a->reg,o);
					else
						o = minx86enc_encode_memreg(a,o,6);
				}

				if (shorthand || !word) {
					*o++ = (unsigned char)(b->value);
				}
				else {
					if (a->size == 4) {
						*((uint32_t*)o) = (uint32_t)b->value; o += 4;
					}
					else {
						*((uint16_t*)o) = (uint16_t)b->value; o += 2;
					}
				}
			}
		} break;
		case MXOP_CMPXCHG: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x0F; *o++ = 0xB0 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x0F; *o++ = 0xB0 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
		} break;
		case MXOP_CMPXCHG8B: {
			struct minx86dec_argv *a=&ins->argv[0];

			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0xC7; o = minx86enc_encode_memreg(a,o,1);
			}
		} break;
		case MXOP_SYSCALL: {
			*o++ = 0x0F;
			*o++ = 0x05;
		} break;
		case MXOP_SYSRET: {
			*o++ = 0x0F;
			*o++ = 0x07;
		} break;
		case MXOP_SYSENTER: {
			*o++ = 0x0F;
			*o++ = 0x34;
		} break;
		case MXOP_SYSEXIT: {
			*o++ = 0x0F;
			*o++ = 0x35;
		} break;
		case MXOP_WRMSR: {
			*o++ = 0x0F;
			*o++ = 0x30;
		} break;
		case MXOP_RDMSR: {
			*o++ = 0x0F;
			*o++ = 0x32;
		} break;
		case MXOP_FWAIT: {
			*o++ = 0x9B;
		} break;
		case MXOP_WBINVD: {
			*o++ = 0x0F;
			*o++ = 0x09;
		} break;
		case MXOP_VMCALL: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xC1;
		} break;
		case MXOP_VMLAUNCH: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xC2;
		} break;
		case MXOP_VMRESUME: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xC3;
		} break;
		case MXOP_VMXOFF: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xC4;
		} break;
		case MXOP_GETSEC: {
			*o++ = 0x0F;
			*o++ = 0x37;
		} break;

		case MXOP_SALC: {
			*o++ = 0xD6;
		} break;
		case MXOP_ICEBP: {
			*o++ = 0xF1;
		} break;
		case MXOP_XLAT: {
			*o++ = 0xD7;
		} break;
		case MXOP_XGETBV: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xD0;
		} break;
		case MXOP_XSETBV: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xD1;
		} break;
		case MXOP_SWAPGS: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xF8;
		} break;
		case MXOP_UD2: {
			*o++ = 0x0F;
			*o++ = 0x0B;
		} break;
		case MXOP_XSHA1: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA6;
			*o++ = 0xC8;
		} break;
		case MXOP_XSHA256: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA6;
			*o++ = 0xD0;
		} break;
		case MXOP_UMC_IDENT: {
			*o++ = 0x64;
			*o++ = 0xD6;
		} break;
		case MXOP_MONTMUL: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA6;
			*o++ = 0xC0;
		} break;
		case MXOP_XSTORE: {
			if (ins->rep == MX86_REPNE) *o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA7;
			*o++ = 0xC0;
		} break;
		case MXOP_XCRYPTECB: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA7;
			*o++ = 0xC8;
		} break;
		case MXOP_XCRYPTCBC: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA7;
			*o++ = 0xD0;
		} break;
		case MXOP_XCRYPTCTR: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA7;
			*o++ = 0xD8;
		} break;
		case MXOP_XCRYPTCFB: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA7;
			*o++ = 0xE0;
		} break;
		case MXOP_XCRYPTOFB: {
			*o++ = 0xF3;
			*o++ = 0x0F;
			*o++ = 0xA7;
			*o++ = 0xE8;
		} break;
		case MXOP_FEMMS: {
			*o++ = 0x0F;
			*o++ = 0x0E;
		} break;
		case MXOP_EMMS: {
			*o++ = 0x0F;
			*o++ = 0x77;
		} break;
		case MXOP_SFENCE: {
			*o++ = 0x0F;
			*o++ = 0xAE;
			*o++ = 0xF8;
		} break;
		case MXOP_MFENCE: {
			*o++ = 0x0F;
			*o++ = 0xAE;
			*o++ = 0xF0;
		} break;
		case MXOP_LFENCE: {
			*o++ = 0x0F;
			*o++ = 0xAE;
			*o++ = 0xE8;
		} break;
		case MXOP_MONITOR: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xC8;
		} break;
		case MXOP_LEAVE: {
			*o++ = 0xC9;
		} break;
		case MXOP_IRET: {
			if (est->data32) *o++ = 0x66;
			*o++ = 0xCF;
		} break;
		case MXOP_IRETD: {
			if (!est->data32) *o++ = 0x66;
			*o++ = 0xCF;
		} break;
		case MXOP_LAHF: {
			*o++ = 0x9F;
		} break;
		case MXOP_SAHF: {
			*o++ = 0x9E;
		} break;
		case MXOP_RSM: {
			*o++ = 0x0F;
			*o++ = 0xAA;
		} break;
		case MXOP_STC: {
			*o++ = 0xF9;
		} break;
		case MXOP_STD: {
			*o++ = 0xFD;
		} break;
		case MXOP_STI: {
			*o++ = 0xFB;
		} break;
		case MXOP_CLI: {
			*o++ = 0xFA;
		} break;
		case MXOP_CLTS: {
			*o++ = 0x0F;
			*o++ = 0x06;
		} break;
		case MXOP_CMC: {
			*o++ = 0xF5;
		} break;
		case MXOP_CLD: {
			*o++ = 0xFC;
		} break;
		case MXOP_CLC: {
			*o++ = 0xF8;
		} break;
		case MXOP_CBW: {
			*o++ = 0x98;
		} break;
		case MXOP_CWDE: {
			*o++ = 0x66;
			*o++ = 0x98;
		} break;
		case MXOP_AAS: {
			*o++ = 0x3F;
		} break;
		case MXOP_DAA: {
			*o++ = 0x27;
		} break;
		case MXOP_DAS: {
			*o++ = 0x2F;
		} break;
		case MXOP_CWD: {
			*o++ = 0x99;
		} break;
		case MXOP_CDQ: {
			*o++ = 0x66;
			*o++ = 0x99;
		} break;
		case MXOP_AAA: {
			*o++ = 0x37;
		} break;
		case MXOP_AAD: {
			struct minx86dec_argv *a=&ins->argv[0];
			*o++ = 0xD5;
			*o++ = (unsigned char)(a->value & 0xFF);
		} break;
		case MXOP_AAM: {
			struct minx86dec_argv *a=&ins->argv[0];
			*o++ = 0xD4;
			*o++ = (unsigned char)(a->value & 0xFF);
		} break;
		case MXOP_MWAIT: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xC9;
		} break;
		case MXOP_POPA: {
			*o++ = 0x61;
		} break;
		case MXOP_POPAD: {
			*o++ = 0x66;
			*o++ = 0x61;
		} break;
		case MXOP_PUSHA: {
			*o++ = 0x60;
		} break;
		case MXOP_PUSHAD: {
			*o++ = 0x66;
			*o++ = 0x60;
		} break;
		case MXOP_PUSHF: {
			*o++ = 0x9C;
		} break;
		case MXOP_PUSHFD: {
			*o++ = 0x66;
			*o++ = 0x9C;
		} break;
		case MXOP_POPF: {
			*o++ = 0x9D;
		} break;
		case MXOP_POPFD: {
			*o++ = 0x66;
			*o++ = 0x9D;
		} break;
		case MXOP_RDTSC: {
			*o++ = 0x0F;
			*o++ = 0x31;
		} break;
		case MXOP_RDTSCP: {
			*o++ = 0x0F;
			*o++ = 0x01;
			*o++ = 0xF9;
		} break;
		case MXOP_RDPMC: {
			*o++ = 0x0F;
			*o++ = 0x33;
		} break;
		case MXOP_LOADALL_286: {
			*o++ = 0x0F;
			*o++ = 0x05;
		} break;
		case MXOP_LOADALL_386: {
			*o++ = 0x0F;
			*o++ = 0x07;
		} break;
		case MXOP_RET: {
			if (ins->argc == 1) {
				struct minx86dec_argv *a=&ins->argv[0];
				*o++ = 0xC2;
				if (a->size == 4) {
					*((uint32_t*)o) = (uint32_t)a->value; o += 4;
				}
				else {
					*((uint16_t*)o) = (uint16_t)a->value; o += 2;
				}
			}
			else {
				*o++ = 0xC3;
			}
		} break;
		case MXOP_RETF: {
			if (ins->argc == 1) {
				struct minx86dec_argv *a=&ins->argv[0];
				*o++ = 0xCA;
				if (a->size == 4) {
					*((uint32_t*)o) = (uint32_t)a->value; o += 4;
				}
				else {
					*((uint16_t*)o) = (uint16_t)a->value; o += 2;
				}
			}
			else {
				*o++ = 0xCB;
			}
		} break;
		case MXOP_CPUID: {
			*o++ = 0x0F;
			*o++ = 0xA2;
		} break;
		case MXOP_JO:	case MXOP_JNO:	case MXOP_JB:	case MXOP_JNB:
		case MXOP_JZ:	case MXOP_JNZ:	case MXOP_JBE:	case MXOP_JA:
		case MXOP_JS:	case MXOP_JNS:	case MXOP_JP:	case MXOP_JNP:
		case MXOP_JL:	case MXOP_JGE:	case MXOP_JLE:	case MXOP_JG: {
			struct minx86dec_argv *a=&ins->argv[0];
			int32_t delta = (int32_t)(a->value - est->ip_value),extra = (int32_t)(o - est->started_here);
			/* if it's small enough, encode as single-byte JMP */
			if ((delta-(2+extra)) >= -0x80 && (delta-(2+extra)) < 0x80)
				{ /*o = minx86enc_32_overrides(a,est,o,1);*/ *o++ = 0x70+(ins->opcode-MXOP_JO); *o++ = (uint8_t)(delta-(2+extra)); }
			/* if the encoding is for 32-bit mode, OR the delta is too large for 16-bit mode: */
			else if (est->addr32 || !((delta-(5+extra)) >= -0x8000 && (delta-(5+extra)) < 0x8000))
				{ if (!est->addr32) { *o++ = 0x66; extra++; }; *o++ = 0x0F; *o++ = 0x80+(ins->opcode-MXOP_JO); *((uint32_t*)o) = (uint32_t)(delta-(6+extra)); o += 4; }
			else
				{ *o++ = 0x0F; *o++ = 0x80+(ins->opcode-MXOP_JO); *((uint16_t*)o) = (uint16_t)(delta-(4+extra)); o += 2; }
		} break;
		case MXOP_JCXZ: {
			struct minx86dec_argv *a=&ins->argv[0];
			int32_t delta = (int32_t)(a->value - est->ip_value),extra = (int32_t)(o - est->started_here);
			if ((delta-(2+extra)) >= -0x80 && (delta-(2+extra)) < 0x80)
				{ o = minx86enc_32_overrides(a,est,o,1); *o++ = 0xE3; *o++ = (uint8_t)(delta-(2+extra)); }
		} break;
		case MXOP_LOOP: {
			struct minx86dec_argv *a=&ins->argv[0];
			int32_t delta = (int32_t)(a->value - est->ip_value),extra = (int32_t)(o - est->started_here);
			if ((delta-(2+extra)) >= -0x80 && (delta-(2+extra)) < 0x80)
				{ o = minx86enc_32_overrides(a,est,o,1); *o++ = 0xE2; *o++ = (uint8_t)(delta-(2+extra)); }
		} break;
		case MXOP_LOOPE: {
			struct minx86dec_argv *a=&ins->argv[0];
			int32_t delta = (int32_t)(a->value - est->ip_value),extra = (int32_t)(o - est->started_here);
			if ((delta-(2+extra)) >= -0x80 && (delta-(2+extra)) < 0x80)
				{ o = minx86enc_32_overrides(a,est,o,1); *o++ = 0xE1; *o++ = (uint8_t)(delta-(2+extra)); }
		} break;
		case MXOP_LOOPNE: {
			struct minx86dec_argv *a=&ins->argv[0];
			int32_t delta = (int32_t)(a->value - est->ip_value),extra = (int32_t)(o - est->started_here);
			if ((delta-(2+extra)) >= -0x80 && (delta-(2+extra)) < 0x80)
				{ o = minx86enc_32_overrides(a,est,o,1); *o++ = 0xE0; *o++ = (uint8_t)(delta-(2+extra)); }
		} break;
		case MXOP_UMOV: { /*=====================UMOV========================*/
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = (a->size >= 2) ? 1 : 0;
			
			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word += 2; }

			/* ADD [mem],reg or ADD reg,[mem] */
			if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x0F; *o++ = 0x10 + word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			/* TEST reg,reg or TEST reg,reg */
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,word);
				*o++ = 0x0F; *o++ = 0x10 + word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
		} break;
		case MXOP_MOVUPD:
		case MXOP_MOVUPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = 0;

			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word++; }

			if (b->regtype == MX86_RT_SSE && a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_sse_mod(a,est,o,ins->opcode == MXOP_MOVUPD);
				*o++ = 0x0F; *o++ = 0x11 - word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			else if (b->regtype == MX86_RT_SSE && a->regtype == MX86_RT_SSE) {
				o = minx86enc_32_sse_mod(a,est,o,ins->opcode == MXOP_MOVUPD);
				*o++ = 0x0F; *o++ = 0x11 - word; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
		} break;
		case MXOP_MOVLPD:
		case MXOP_MOVLPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			unsigned char word = 0;

			/* make sure a is r/m and b is reg. ASSUME: both are the same datasize */
			if (b->regtype == MX86_RT_NONE) { struct minx86dec_argv *t = a; a = b; b = t; word++; }

			if (b->regtype == MX86_RT_SSE && a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_sse_mod(a,est,o,ins->opcode == MXOP_MOVLPD);
				*o++ = 0x0F; *o++ = 0x13 - word; o = minx86enc_encode_memreg(a,o,b->reg);
			}
		} break;
		case MXOP_CMPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (b->regtype == MX86_RT_NONE && a->regtype == MX86_RT_NONE) {
				if (ins->rep == MX86_REPE) *o++ = 0xF2;
				else if (ins->rep == MX86_REPNE) *o++ = 0xF3;
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0xA6 + (a->size>=2?1:0);
			}
		} break;
		case MXOP_SCAS: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				if (ins->rep == MX86_REPE) *o++ = 0xF2;
				else if (ins->rep == MX86_REPNE) *o++ = 0xF3;
				o = minx86enc_seg_overrides_es(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0xAE + (a->size>=2?1:0);
			}
		} break;
		case MXOP_LODS: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				if (ins->rep == MX86_REPE) *o++ = 0xF2;
				else if (ins->rep == MX86_REPNE) *o++ = 0xF3;
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0xAC + (a->size>=2?1:0);
			}
		} break;
		case MXOP_STOS: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				if (ins->rep == MX86_REPE) *o++ = 0xF2;
				else if (ins->rep == MX86_REPNE) *o++ = 0xF3;
				o = minx86enc_seg_overrides_es(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0xAA + (a->size>=2?1:0);
			}
		} break;
		case MXOP_MOVS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (b->regtype == MX86_RT_NONE && a->regtype == MX86_RT_NONE) {
				if (ins->rep == MX86_REPE) *o++ = 0xF2;
				else if (ins->rep == MX86_REPNE) *o++ = 0xF3;
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0xA4 + (a->size>=2?1:0);
			}
		} break;
		case MXOP_OUTS: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				if (ins->rep == MX86_REPE) *o++ = 0xF2;
				else if (ins->rep == MX86_REPNE) *o++ = 0xF3;
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x6E + (a->size>=2?1:0);
			}
		} break;
		case MXOP_INS: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				if (ins->rep == MX86_REPE) *o++ = 0xF2;
				else if (ins->rep == MX86_REPNE) *o++ = 0xF3;
				o = minx86enc_seg_overrides_es(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x6C + (a->size>=2?1:0);
			}
		} break;
		case MXOP_BSF: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBC; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,1);
				*o++ = 0x0F; *o++ = 0xBC; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_BSR: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBD; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,1);
				*o++ = 0x0F; *o++ = 0xBD; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_ENTER: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_IMM && b->regtype == MX86_RT_IMM) {
				*o++ = 0xC8;
				*((uint16_t*)o) = (uint16_t)a->value; o += 2;
				*o++ = (uint8_t)b->value;
			}
		} break;
		case MXOP_ARPL: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x63; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x63; o = minx86enc_encode_memreg(a,o,b->reg);
			}
		} break;
		case MXOP_BOUND: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				if ((b->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
				if ((a->size>>2)^(est->data32?1:0)) *o++ = 0x66;
				*o++ = 0x62; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_LGDT: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_memreg(a,o,2);
			}
		} break;
		case MXOP_LIDT: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_memreg(a,o,3);
			}
		} break;
		case MXOP_LLDT: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_memreg(a,o,2);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_rm_reg(a,2,a->reg,o);
			}
		} break;
		case MXOP_LMSW: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_memreg(a,o,6);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_rm_reg(a,6,a->reg,o);
			}
		} break;
		case MXOP_LTR: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_memreg(a,o,3);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_rm_reg(a,3,a->reg,o);
			}
		} break;
		case MXOP_SGDT: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_memreg(a,o,0);
			}
		} break;
		case MXOP_SIDT: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_memreg(a,o,1);
			}
		} break;
		case MXOP_SLDT: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_memreg(a,o,0);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_rm_reg(a,0,a->reg,o);
			}
		} break;
		case MXOP_STR: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_memreg(a,o,1);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_rm_reg(a,1,a->reg,o);
			}
		} break;
		case MXOP_SMSW: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_memreg(a,o,4);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_rm_reg(a,4,a->reg,o);
			}
		} break;
		case MXOP_VERR: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_memreg(a,o,4);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_rm_reg(a,4,a->reg,o);
			}
		} break;
		case MXOP_VERW: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_memreg(a,o,5);
			}
			else if (a->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,a->size>=2?1:0);
				*o++ = 0x0F; *o++ = 0x00; o = minx86enc_encode_rm_reg(a,5,a->reg,o);
			}
		} break;
		case MXOP_LSL: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0x03; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,1);
				*o++ = 0x0F; *o++ = 0x03; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_LAR: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0x02; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,1);
				*o++ = 0x0F; *o++ = 0x02; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_LDS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				if ((b->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
				if ((a->size>>2)^(est->data32?1:0)) *o++ = 0x66;
				*o++ = 0xC5; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_LES: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				if ((b->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
				if ((a->size>>2)^(est->data32?1:0)) *o++ = 0x66;
				*o++ = 0xC4; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_LSS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				if ((b->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
				if ((a->size>>2)^(est->data32?1:0)) *o++ = 0x66;
				*o++ = 0x0F; *o++ = 0xB2; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_LFS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				if ((b->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
				if ((a->size>>2)^(est->data32?1:0)) *o++ = 0x66;
				*o++ = 0x0F; *o++ = 0xB4; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_LGS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				if ((b->memregsz>>2)^(est->addr32?1:0)) *o++ = 0x67;
				if ((a->size>>2)^(est->data32?1:0)) *o++ = 0x66;
				*o++ = 0x0F; *o++ = 0xB5; o = minx86enc_encode_memreg(b,o,a->reg);
			}
		} break;
		case MXOP_BSWAP: {
			struct minx86dec_argv *a=&ins->argv[0];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG) {
				if ((a->size>>2)^(est->data32?1:0)) *o++ = 0x66;
				*o++ = 0x0F; *o++ = 0xC8 + (a->reg&7);
			}
		} break;
		case MXOP_CLFLUSH: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0xAE; o = minx86enc_encode_memreg(a,o,7);
			}
		} break;
		case MXOP_LDMXCSR: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0xAE; o = minx86enc_encode_memreg(a,o,2);
			}
		} break;
		case MXOP_STMXCSR: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0xAE; o = minx86enc_encode_memreg(a,o,3);
			}
		} break;
		case MXOP_INVLPG: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x01; o = minx86enc_encode_memreg(a,o,7);
			}
		} break;
		case MXOP_HLT: {
			*o++ = 0xF4;
		} break;
		case MXOP_INTO: {
			*o++ = 0xCE;
		} break;
		case MXOP_INVD: {
			*o++ = 0x0F; *o++ = 0x08;
		} break;
		case MXOP_SETO: case MXOP_SETNO: case MXOP_SETB:  case MXOP_SETNB:
		case MXOP_SETZ: case MXOP_SETNZ: case MXOP_SETBE: case MXOP_SETA:
		case MXOP_SETS: case MXOP_SETNS: case MXOP_SETP:  case MXOP_SETNP:
		case MXOP_SETL: case MXOP_SETGE: case MXOP_SETLE: case MXOP_SETG: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0x90+(ins->opcode-MXOP_SETO); o = minx86enc_encode_memreg(a,o,0);
			}
			else if (a->regtype == MX86_RT_REG) {
				*o++ = 0x0F; *o++ = 0x90+(ins->opcode-MXOP_SETO); o = minx86enc_encode_rm_reg(a,0,a->reg,o);
			}
		} break;
		case MXOP_PREFETCH: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x0D; o = minx86enc_encode_memreg(a,o,0);
			}
		} break;
		case MXOP_PREFETCHW: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x0D; o = minx86enc_encode_memreg(a,o,1);
			}
		} break;
		case MXOP_PREFETCHNTA: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x18; o = minx86enc_encode_memreg(a,o,0);
			}
		} break;
		case MXOP_PREFETCHT0: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x18; o = minx86enc_encode_memreg(a,o,1);
			}
		} break;
		case MXOP_PREFETCHT1: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x18; o = minx86enc_encode_memreg(a,o,2);
			}
		} break;
		case MXOP_PREFETCHT2: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0x0F; *o++ = 0x18; o = minx86enc_encode_memreg(a,o,3);
			}
		} break;
		case MXOP_VMXON: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,ins->data32);
				*o++ = 0xF3; *o++ = 0x0F; *o++ = 0xC7; o = minx86enc_encode_memreg(a,o,6);
			}
		} break;
		case MXOP_VMCLEAR: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x66; *o++ = 0x0F; *o++ = 0xC7; o = minx86enc_encode_memreg(a,o,6);
			}
		} break;
		case MXOP_XRSTOR: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0xAE; o = minx86enc_encode_memreg(a,o,5);
			}
		} break;
		case MXOP_XSAVE: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0xAE; o = minx86enc_encode_memreg(a,o,4);
			}
		} break;
		case MXOP_VMPTRLD: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0xC7; o = minx86enc_encode_memreg(a,o,6);
			}
		} break;
		case MXOP_VMPTRST: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0x0F; *o++ = 0xC7; o = minx86enc_encode_memreg(a,o,7);
			}
		} break;
		case MXOP_CMOVO: case MXOP_CMOVNO: case MXOP_CMOVC:  case MXOP_CMOVNC:
		case MXOP_CMOVZ: case MXOP_CMOVNZ: case MXOP_CMOVBE: case MXOP_CMOVA:
		case MXOP_CMOVS: case MXOP_CMOVNS: case MXOP_CMOVP:  case MXOP_CMOVNP:
		case MXOP_CMOVL: case MXOP_CMOVNL: case MXOP_CMOVNG: case MXOP_CMOVG: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = 0x0F; *o++ = 0x40+(ins->opcode-MXOP_CMOVO); o = minx86enc_encode_memreg(b,o,a->reg);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				*o++ = 0x0F; *o++ = 0x40+(ins->opcode-MXOP_CMOVO); o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
			}
		} break;
		case MXOP_F2XM1: {
			*o++ = 0xD9; *o++ = 0xF0;
		} break;
		case MXOP_FABS: {
			*o++ = 0xD9; *o++ = 0xE1;
		} break;
		case MXOP_FCHS: {
			*o++ = 0xD9; *o++ = 0xE0;
		} break;
		case MXOP_FCLEX: {
			if (ins->fwait) *o++ = 0x9B;
			*o++ = 0xDB; *o++ = 0xE2;
		} break;
		case MXOP_FCOS: {
			*o++ = 0xD9; *o++ = 0xFF;
		} break;
		case MXOP_FDECSTP: {
			*o++ = 0xD9; *o++ = 0xF6;
		} break;
		case MXOP_FDISI: {
			if (ins->fwait) *o++ = 0x9B;
			*o++ = 0xDB; *o++ = 0xE1;
		} break;
		case MXOP_FENI: {
			if (ins->fwait) *o++ = 0x9B;
			*o++ = 0xDB; *o++ = 0xE0;
		} break;
		case MXOP_FSETPM: {
			*o++ = 0xDB; *o++ = 0xE4;
		} break;
		case MXOP_FINCSTP: {
			*o++ = 0xD9; *o++ = 0xF7;
		} break;
		case MXOP_FINIT: {
			if (ins->fwait) *o++ = 0x9B;
			*o++ = 0xDB; *o++ = 0xE3;
		} break;
		case MXOP_FLD1: {
			*o++ = 0xD9; *o++ = 0xE8;
		} break;
		case MXOP_FLDL2T: {
			*o++ = 0xD9; *o++ = 0xE9;
		} break;
		case MXOP_FLDL2E: {
			*o++ = 0xD9; *o++ = 0xEA;
		} break;
		case MXOP_FLDPI: {
			*o++ = 0xD9; *o++ = 0xEB;
		} break;
		case MXOP_FLDLG2: {
			*o++ = 0xD9; *o++ = 0xEC;
		} break;
		case MXOP_FLDLN2: {
			*o++ = 0xD9; *o++ = 0xED;
		} break;
		case MXOP_FLDZ: {
			*o++ = 0xD9; *o++ = 0xEE;
		} break;
		case MXOP_FNOP: {
			*o++ = 0xD9; *o++ = 0xD0;
		} break;
		case MXOP_FPATAN: {
			*o++ = 0xD9; *o++ = 0xF3;
		} break;
		case MXOP_FPREM: {
			*o++ = 0xD9; *o++ = 0xF8;
		} break;
		case MXOP_FPREM1: {
			*o++ = 0xD9; *o++ = 0xF5;
		} break;
		case MXOP_FPTAN: {
			*o++ = 0xD9; *o++ = 0xF2;
		} break;
		case MXOP_FRNDINT: {
			*o++ = 0xD9; *o++ = 0xFC;
		} break;
		case MXOP_FSCALE: {
			*o++ = 0xD9; *o++ = 0xFD;
		} break;
		case MXOP_FSIN: {
			*o++ = 0xD9; *o++ = 0xFE;
		} break;
		case MXOP_FSINCOS: {
			*o++ = 0xD9; *o++ = 0xFB;
		} break;
		case MXOP_FSQRT: {
			*o++ = 0xD9; *o++ = 0xFA;
		} break;
		case MXOP_FTST: {
			*o++ = 0xD9; *o++ = 0xE4;
		} break;
		case MXOP_FUCOMPP: {
			*o++ = 0xDA; *o++ = 0xE9;
		} break;
		case MXOP_FXAM: {
			*o++ = 0xD9; *o++ = 0xE5;
		} break;
		case MXOP_FXTRACT: {
			*o++ = 0xD9; *o++ = 0xF4;
		} break;
		case MXOP_FYL2X: {
			*o++ = 0xD9; *o++ = 0xF1;
		} break;
		case MXOP_FYL2XP1: {
			*o++ = 0xD9; *o++ = 0xF9;
		} break;
		case MXOP_PAUSE: {
			*o++ = 0xF3; *o++ = 0x90;
		} break;
		case MXOP_FLD: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				if (a->size == 10) {
					*o++ = 0xDB;
					o = minx86enc_encode_memreg(a,o,5);
				}
				else if (a->size == 8) {
					*o++ = 0xDD;
					o = minx86enc_encode_memreg(a,o,0);
				}
				else {
					*o++ = 0xD9;
					o = minx86enc_encode_memreg(a,o,0);
				}
			}
			else if (a->regtype == MX86_RT_ST) {
				*o++ = 0xD9;
				o = minx86enc_encode_rm_reg(a,0,a->reg,o);
			}
		} break;
		case MXOP_FADD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,0,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,0,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,0);
			}
		} break;
		case MXOP_FMUL: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,1,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,1,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,1);
			}
		} break;
		case MXOP_FCOM: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,2,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,2,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,2);
			}
		} break;
		case MXOP_FCOMP: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,3,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,3,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,3);
			}
		} break;
		case MXOP_FSUB: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,4,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,4,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,4);
			}
		} break;
		case MXOP_FSUBR: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,5,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,5,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,5);
			}
		} break;
		case MXOP_FDIV: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,6,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,6,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,6);
			}
		} break;
		case MXOP_FDIVR: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				/* NTS: You can fadd st0,st<N> or fadd st<N>,st0 but you can't do any other combination */
				if (a->reg != 0) {
					*o++ = 0xDC; o = minx86enc_encode_rm_reg(a,7,a->reg,o);
				}
				else if (b->reg != 0) {
					*o++ = 0xD8; o = minx86enc_encode_rm_reg(a,7,b->reg,o);
				}
			}
			else if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_NONE) {
				/* NTS: The decoder puts st(0) in the first argv for explanatory reasons, else
				 *      you would be wondering what FADD is adding to */
				o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(b,est,o,0);
				*o++ = (b->size >= 8 ? 0xDC : 0xD8); o = minx86enc_encode_memreg(b,o,7);
			}
		} break;
		case MXOP_FST: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				if (a->size == 10) {
					/* NTS: There IS NO 10-byte version?!? */
				}
				else if (a->size == 8) {
					*o++ = 0xDD;
					o = minx86enc_encode_memreg(a,o,2);
				}
				else {
					*o++ = 0xD9;
					o = minx86enc_encode_memreg(a,o,2);
				}
			}
			else if (a->regtype == MX86_RT_ST) {
				*o++ = 0xDD;
				o = minx86enc_encode_rm_reg(a,2,a->reg,o);
			}
		} break;
		case MXOP_FSTP: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				if (a->size == 10) {
					*o++ = 0xDB;
					o = minx86enc_encode_memreg(a,o,7);
				}
				else if (a->size == 8) {
					*o++ = 0xDD;
					o = minx86enc_encode_memreg(a,o,3);
				}
				else {
					*o++ = 0xD9;
					o = minx86enc_encode_memreg(a,o,3);
				}
			}
			else if (a->regtype == MX86_RT_ST) {
				*o++ = 0xDD;
				o = minx86enc_encode_rm_reg(a,3,a->reg,o);
			}
		} break;
		case MXOP_FLDENV: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0xD9; o = minx86enc_encode_memreg(a,o,4);
			}
		} break;
		case MXOP_FLDCW: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0xD9; o = minx86enc_encode_memreg(a,o,5);
			}
		} break;
		case MXOP_FSTENV: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0xD9; o = minx86enc_encode_memreg(a,o,6);
			}
		} break;
		case MXOP_FSTCW: {
			struct minx86dec_argv *a=&ins->argv[0];
			if (a->regtype == MX86_RT_NONE) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,0);
				*o++ = 0xD9; o = minx86enc_encode_memreg(a,o,7);
			}
		} break;
		case MXOP_FADDP: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				*o++ = 0xDE; o = minx86enc_encode_rm_reg(a,0,a->reg,o);
			}
		} break;
		case MXOP_FDIVP: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				*o++ = 0xDE; o = minx86enc_encode_rm_reg(a,6,a->reg,o);
			}
		} break;
		case MXOP_FDIVRP: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				*o++ = 0xDE; o = minx86enc_encode_rm_reg(a,7,a->reg,o);
			}
		} break;
		case MXOP_FMULP: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				*o++ = 0xDE; o = minx86enc_encode_rm_reg(a,1,a->reg,o);
			}
		} break;
		case MXOP_FSUBRP: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				*o++ = 0xDE; o = minx86enc_encode_rm_reg(a,5,a->reg,o);
			}
		} break;
		case MXOP_FSUBP: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (a->regtype == MX86_RT_ST && b->regtype == MX86_RT_ST) {
				*o++ = 0xDE; o = minx86enc_encode_rm_reg(a,4,a->reg,o);
			}
		} break;
		case MXOP_ADDPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,1);
					*o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ADDPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=0 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC0+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC0+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ADDSD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ADDSS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC2+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC2+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x58; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0xF3; *o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0xF3; *o++ = 0x0F; *o++ = 0x58; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ADDSUBPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xD0; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xD0; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0xD0; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0xD0; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ADDSUBPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xD0; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xD0; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0xD0; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0xD0; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ANDPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x54; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x54; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0x54; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0x54; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ANDNPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x55; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x55; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0x55; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0x55; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ANDPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x54; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x54; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0x0F; *o++ = 0x54; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x0F; *o++ = 0x54; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_ANDNPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) { /* Intel AVX form using ymm1, etc. */
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					/* NTS: VEX pp=1 l=<YMM/XMM> w=1 v=b->reg */
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x55; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x55; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					*o++ = 0x0F; *o++ = 0x55; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x0F; *o++ = 0x55; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_BLENDPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x0D; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x0D; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->value);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,1);
					*o++ = 0x0F; *o++ = 0x3A; *o++ = 0x0D; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
					*o++ = (unsigned char)(c->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,1);
					*o++ = 0x0F; *o++ = 0x3A; *o++ = 0x0D; o = minx86enc_encode_memreg(b,o,a->reg);
					*o++ = (unsigned char)(c->value);
				}
			}
		} break;
		case MXOP_BLENDPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x0C; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x0C; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->value);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,1);
					*o++ = 0x0F; *o++ = 0x3A; *o++ = 0x0C; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
					*o++ = (unsigned char)(c->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,1);
					*o++ = 0x0F; *o++ = 0x3A; *o++ = 0x0C; o = minx86enc_encode_memreg(b,o,a->reg);
					*o++ = (unsigned char)(c->value);
				}
			}
		} break;
		case MXOP_BLENDVPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x4B; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->reg << 4);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_SSE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,1);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x4B; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->reg << 4);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && c->reg == 0) {
					o = minx86enc_32_overrides(a,est,o,1);
					*o++ = 0x0F; *o++ = 0x38; *o++ = 0x15; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_SSE && c->reg == 0) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,1);
					*o++ = 0x0F; *o++ = 0x38; *o++ = 0x15; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_BLENDVPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x4A; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->reg << 4);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_SSE) {
					o = minx86enc_seg_overrides(c,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(c,est,o,1);
					*o++ = 0xC4; *o++ = 0xE3; *o++ = 0x41+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x4A; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->reg << 4);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && c->reg == 0) {
					o = minx86enc_32_overrides(a,est,o,1);
					*o++ = 0x0F; *o++ = 0x38; *o++ = 0x14; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_SSE && c->reg == 0) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,1);
					*o++ = 0x0F; *o++ = 0x38; *o++ = 0x14; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_BT: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xA3; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xA3; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_IMM) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBA; o = minx86enc_encode_rm_reg(b,4,a->reg,o);
				*o++ = (unsigned char)(b->value);
			}
		} break;
		case MXOP_BTC: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBB; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBB; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_IMM) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBA; o = minx86enc_encode_rm_reg(b,7,a->reg,o);
				*o++ = (unsigned char)(b->value);
			}
		} break;
		case MXOP_BTR: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xB3; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xB3; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_IMM) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBA; o = minx86enc_encode_rm_reg(b,6,a->reg,o);
				*o++ = (unsigned char)(b->value);
			}
		} break;
		case MXOP_BTS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			/* there is no byte-size versionm and there is no version that writes the result to memory */

			if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_REG) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xAB; o = minx86enc_encode_rm_reg(b,b->reg,a->reg,o);
			}
			else if (a->regtype == MX86_RT_NONE && b->regtype == MX86_RT_REG) {
				o = minx86enc_seg_overrides(a,est,o,ins->segment >= 0);
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xAB; o = minx86enc_encode_memreg(a,o,b->reg);
			}
			else if (a->regtype == MX86_RT_REG && b->regtype == MX86_RT_IMM) {
				o = minx86enc_32_overrides(a,est,o,1);
				*o++ = 0x0F; *o++ = 0xBA; o = minx86enc_encode_rm_reg(b,5,a->reg,o);
				*o++ = (unsigned char)(b->value);
			}
		} break;
		case MXOP_CMPPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->value);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,1);
					*o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
					*o++ = (unsigned char)(c->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,1);
					*o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_memreg(b,o,a->reg);
					*o++ = (unsigned char)(c->value);
				}
			}
		} break;
		case MXOP_CMPPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC0+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC0+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->value);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
					*o++ = (unsigned char)(c->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_memreg(b,o,a->reg);
					*o++ = (unsigned char)(c->value);
				}
			}
		} break;
		case MXOP_CMPSD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->value);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
					*o++ = (unsigned char)(c->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_memreg(b,o,a->reg);
					*o++ = (unsigned char)(c->value);
				}
			}
		} break;
		case MXOP_CMPSS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1],*c=&ins->argv[2];
			if (ins->argc == 4) {
				struct minx86dec_argv *d=&ins->argv[3];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC2+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
					*o++ = (unsigned char)(d->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE && d->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC2+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0xC2; o = minx86enc_encode_memreg(c,o,a->reg);
					*o++ = (unsigned char)(d->value);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xF3; *o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
					*o++ = (unsigned char)(c->value);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE && c->regtype == MX86_RT_IMM) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0xF3; *o++ = 0x0F; *o++ = 0xC2; o = minx86enc_encode_memreg(b,o,a->reg);
					*o++ = (unsigned char)(c->value);
				}
			}
		} break;
		case MXOP_DIVSS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) {
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC2+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC2+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xF3; *o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0xF3; *o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_DIVSD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) {
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC3+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0xF2; *o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;

		case MXOP_DIVPS: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) {
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC0+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC0+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
		case MXOP_DIVPD: {
			struct minx86dec_argv *a=&ins->argv[0],*b=&ins->argv[1];
			if (ins->argc == 3) {
				struct minx86dec_argv *c=&ins->argv[2];
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,c->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE && c->regtype == MX86_RT_NONE) {
					o = minx86enc_32_overrides(c,est,o,0);
					*o++ = 0xC5; *o++ = 0xC1+((b->reg^7)<<3)+(a->size==32?4:0);
					*o++ = 0x5E; o = minx86enc_encode_memreg(c,o,a->reg);
				}
			}
			else {
				if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_SSE) {
					o = minx86enc_32_overrides(a,est,o,0);
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_rm_reg(a,a->reg,b->reg,o);
				}
				else if (a->regtype == MX86_RT_SSE && b->regtype == MX86_RT_NONE) {
					o = minx86enc_seg_overrides(b,est,o,ins->segment >= 0);
					o = minx86enc_32_overrides(b,est,o,0);
					*o++ = 0x66; *o++ = 0x0F; *o++ = 0x5E; o = minx86enc_encode_memreg(b,o,a->reg);
				}
			}
		} break;
	}

	est->write_ip = o;
}

