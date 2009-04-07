/* this is not complete code, this is a fragment included by the various "cores" provided.
 * context: inside a decode function.
 * 
 * void minx86dec_decode8086(struct minx86dec_state *state,struct minx86dec_instruction *ins) {
 * minx86_read_ptr_t *cip = state->read_ip;
 * #define core_level 0|1|2|3|4|5|6
 * #define isdata32 0 or state->data32
 * #define isaddr32 0 or state->addr32
 *
 * this code can be built to decompile like an 8086 (from core8086.c), or else like any 32-bit
 * processor (from other core sources). x86-64 decoding however is handled elsewhere, since
 * that is a different mode.
 *
 * This code assumes that the native "optimal" datatype is at least 16 bits (for 8086 decoding)
 * or 32 bits (for 386+ decoding). This is separated from x86_64 decoding which assumes a 64-bit
 * datatype is present.
 *
 * ready? here we go! */
#define data32wordsize (isdata32 ? 4 : 2)
#define addr32wordsize (isaddr32 ? 4 : 2)
#define seg_can_override(x) (ins->segment >= 0 ? ins->segment : (x))

#define COVER_4(x) case (x): case (x+1): case (x+2): case (x+3)
#define COVER_2(x) case (x): case (x+1)

#define COVER_8(x) COVER_4(x): COVER_4(x+4)
#define COVER_ROW(x) COVER_8(x): COVER_8(x+8)

decode_next:
{
	const uint8_t first_byte = *cip++;
	switch (first_byte) {
		/* group 0x00-0x3F opcodes */
		COVER_2(0x04):	/* ADD */	COVER_2(0x0C):	/* OR */
		COVER_2(0x14):	/* ADC */	COVER_2(0x1C):	/* SBB */
		COVER_2(0x24):	/* AND */	COVER_2(0x2C):	/* SUB */
		COVER_2(0x34):	/* XOR */	COVER_2(0x3C):	/* CMP */
			ins->opcode = MXOP_ADD+(first_byte>>3);
			ins->argc = 2; {
				struct minx86dec_argv *imm = &ins->argv[1];
				struct minx86dec_argv *reg = &ins->argv[0];
				imm->size = reg->size = (first_byte & 1) ? data32wordsize : 1;
				reg->regtype = MX86_RT_REG;
				reg->reg = MX86_REG_AX;
				imm->regtype = MX86_RT_IMM;
				imm->value = (first_byte & 1) ?
					(isdata32 ? fetch_u32() : fetch_u16()) : fetch_u8();
			} break;
		/* group 0x00-0x3F opcodes */
		COVER_4(0x00):	/* ADD */	COVER_4(0x08):	/* OR */
		COVER_4(0x10):	/* ADC */	COVER_4(0x18):	/* SBB */
		COVER_4(0x20):	/* AND */	COVER_4(0x28):	/* SUB */
		COVER_4(0x30):	/* XOR */	COVER_4(0x38):	/* CMP */
			ins->opcode = MXOP_ADD+(first_byte>>3);
			ins->argc = 2; {
				const int which = (first_byte>>1)&1;
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[which];
				struct minx86dec_argv *reg = &ins->argv[which^1];
				rm->size = reg->size = (first_byte & 1) ? data32wordsize : 1;
				reg->regtype = MX86_RT_REG;
				reg->reg = mrm.f.reg;
				rm->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,rm,isaddr32);
			} break;

		COVER_4(0x80):	/* immediate group 1 */
			{	union x86_mrm mrm = fetch_modregrm();
				ins->opcode = MXOP_ADD+mrm.f.reg;
				ins->argc = 2;
				struct minx86dec_argv *rm = &ins->argv[0];
				struct minx86dec_argv *imm = &ins->argv[1];
				rm->size = imm->size = (first_byte & 1) ? data32wordsize : 1;
				rm->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,rm,isaddr32);
				imm->regtype = MX86_RT_IMM;
				if (first_byte == 0x83)
					imm->value = (uint32_t)((char)fetch_u8());
				else if (first_byte == 0x81)
					imm->value = (isdata32 ? fetch_u32() : fetch_u16());
				else
					imm->value = fetch_u8();
			} break;

		COVER_2(0x84):	/* TEST */
			ins->opcode = MXOP_TEST;
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[0];
				struct minx86dec_argv *reg = &ins->argv[1];
				rm->size = reg->size = (first_byte & 1) ? data32wordsize : 1;
				reg->regtype = MX86_RT_REG;
				reg->reg = mrm.f.reg;
				rm->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,rm,isaddr32);
			} break;

		COVER_2(0x86):	/* XCHG */
			ins->opcode = MXOP_XCHG;
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[0];
				struct minx86dec_argv *reg = &ins->argv[1];
				rm->size = reg->size = (first_byte & 1) ? data32wordsize : 1;
				reg->regtype = MX86_RT_REG;
				reg->reg = mrm.f.reg;
				rm->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,rm,isaddr32);
			} break;

		COVER_4(0x88):	/* MOV */
			ins->opcode = MXOP_MOV;
			ins->argc = 2; {
				const int which = (first_byte>>1)&1;
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[which];
				struct minx86dec_argv *reg = &ins->argv[which^1];
				rm->size = reg->size = (first_byte & 1) ? data32wordsize : 1;
				reg->regtype = MX86_RT_REG;
				reg->reg = mrm.f.reg;
				rm->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,rm,isaddr32);
			} break;

		COVER_2(0xA4):
			ins->opcode = MXOP_MOVS;
			ins->argc = 2; {
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *s = &ins->argv[1];
				d->size = s->size = (first_byte & 1) ? data32wordsize : 1;
				d->regtype = s->regtype = MX86_RT_NONE;
				s->segment = seg_can_override(MX86_SEG_DS);
				d->segment = MX86_SEG_ES;
				d->scalar = s->scalar = 0;
				d->memregs = s->memregs = 1;
				d->memref_base = s->memref_base = 0;
				d->memregsz = s->memregsz = addr32wordsize;
				s->memreg[0] = MX86_REG_ESI;
				d->memreg[0] = MX86_REG_EDI;
			}
			break;

		COVER_4(0xAA): COVER_2(0xAE):
			ins->opcode = MXOP_STOS + ((first_byte - 0xAA) >> 1);
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				r->size = (first_byte & 1) ? data32wordsize : 1;
				r->regtype = MX86_RT_NONE;
				r->segment = first_byte & 2 ? MX86_SEG_ES : seg_can_override(MX86_SEG_DS);
				r->scalar = 0;
				r->memregs = 1;
				r->memref_base = 0;
				r->memregsz = addr32wordsize;
				r->memreg[0] = (first_byte & 2) ? MX86_REG_EDI : MX86_REG_ESI;
			}
			break;

		case 0x8C: case 0x8E: /* mov r/m, seg reg */
			ins->opcode = MXOP_MOV;
			ins->argc = 2; {
				const int which = (first_byte>>1)&1;
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[which];
				struct minx86dec_argv *reg = &ins->argv[which^1];
				rm->size = reg->size = 2;
				reg->regtype = MX86_RT_SREG;
				reg->reg = mrm.f.reg;
				rm->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,rm,isaddr32);
			} break;

		case 0x8D: /* LEA reg,mem */
			ins->opcode = MXOP_LEA;
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *reg = &ins->argv[0];
				struct minx86dec_argv *rm = &ins->argv[1];
				rm->size = reg->size = data32wordsize;
				reg->regtype = MX86_RT_REG;
				reg->reg = mrm.f.reg;
				rm->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,rm,isaddr32);
			} break;

		case 0x06: case 0x07: /* PUSH/POP ES */
		case 0x0E: /* PUSH/POP CS */
#if core_level == 0 /* 8086 only: 0x0F is POP CS (pop stack into CS!). Later CPUs use this as a prefix for other instructions */
		case 0x0F:
#endif
		case 0x16: case 0x17: /* PUSH/POP SS */
		case 0x1E: case 0x1F: /* PUSH/POP DS */
			ins->opcode = MXOP_PUSH+(first_byte&1);
			ins->argc = 1; {
				struct minx86dec_argv *a = &ins->argv[0];
				a->size = data32wordsize;
				a->regtype = MX86_RT_SREG;
				a->reg = first_byte >> 3;
			} break;

		case 0x27: case 0x2F: case 0x37: case 0x3F: /* DAA/DAS/AAA/AAS */
			ins->opcode = MXOP_DAA+((first_byte>>3)&3);
			ins->argc = 0;
			break;

		COVER_2(0xD4):
			ins->opcode = MXOP_AAM+(first_byte&1);
			ins->argc = 1; {
				struct minx86dec_argv *im = &ins->argv[0];
				im->size = 1;
				im->regtype = MX86_RT_IMM;
				im->value = fetch_u8();
			} break;

		case 0x26: case 0x2E: case 0x36: case 0x3E: /* segment overrides */
			ins->segment = (first_byte >> 3) & 3;
			if (--patience) goto decode_next;
			break;

		/* INC/DEC register */
		COVER_ROW(0x40):
			ins->opcode = MXOP_INC+((first_byte>>3)&1);
			ins->argc = 1; {
				struct minx86dec_argv *reg = &ins->argv[0];
				reg->size = data32wordsize;
				reg->regtype = MX86_RT_REG;
				reg->reg = first_byte & 7;
			} break;

		/* PUSH/POP register */
		COVER_ROW(0x50):
			ins->opcode = MXOP_PUSH+((first_byte>>3)&1);
			ins->argc = 1; {
				struct minx86dec_argv *reg = &ins->argv[0];
				reg->size = data32wordsize;
				reg->regtype = MX86_RT_REG;
				reg->reg = first_byte & 7;
			} break;

		/* Jcc short */
		COVER_ROW(0x70):
			ins->opcode = MXOP_JO+(first_byte&0xF);
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				r->size = addr32wordsize;
				r->regtype = MX86_RT_IMM;
				r->value = state->ip_value + 2 + ((uint32_t)((char)fetch_u8()));
			} break;

		/* MOV a,[memory addr] or
		 * MOV [memory addr],a */
		case 0xA0: case 0xA1: case 0xA2: case 0xA3:
			ins->opcode = MXOP_MOV;
			ins->argc = 2; {
				const int which = (first_byte >> 1) & 1;
				struct minx86dec_argv *areg = &ins->argv[which];
				struct minx86dec_argv *mref = &ins->argv[which^1];
				areg->size = mref->size = (first_byte & 1) ? data32wordsize : 1;
				areg->regtype = MX86_RT_REG;
				areg->reg = MX86_REG_AX;
				mref->regtype = MX86_RT_NONE;
				mref->segment = seg_can_override(MX86_SEG_DS);
				mref->memregs = 0;
				mref->memref_base = isaddr32 ? fetch_u32() : fetch_u16();
			} break;

		/* NOP */
		case 0x90:
			ins->opcode = MXOP_NOP;
			break;

		/* XCHG AX,[reg] */
		COVER_4(0x91): COVER_2(0x95): case 0x97:
			ins->opcode = MXOP_XCHG;
			ins->argc = 2; {
				struct minx86dec_argv *a = &ins->argv[0];
				struct minx86dec_argv *r = &ins->argv[1];
				a->size = r->size = data32wordsize;
				a->regtype = r->regtype = MX86_RT_REG;
				a->reg = MX86_REG_AX;
				r->reg = first_byte & 7;
			} break;

		/* MOV a,imm */
		COVER_ROW(0xB0):
			ins->opcode = MXOP_MOV;
			ins->argc = 2; {
				struct minx86dec_argv *r = &ins->argv[0];
				struct minx86dec_argv *imm = &ins->argv[1];
				r->size = imm->size = (first_byte & 8) ? data32wordsize : 1;
				r->regtype = MX86_RT_REG;
				r->reg = first_byte & 7;
				imm->regtype = MX86_RT_IMM;
				imm->value = (first_byte & 8) ?
					(isdata32 ? fetch_u32() : fetch_u16()) : fetch_u8();
			} break;
#if core_level > 0
		/* extended opcode escape */
		case 0x0F: {
			const uint8_t second_byte = *cip++;
			switch (second_byte) {
# if core_level >= 2
				case 0x00: /* LLDT */
					{
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_SLDT; break;
							case 1: ins->opcode = MXOP_STR; break;
							case 2: ins->opcode = MXOP_LLDT; break;
							case 3: ins->opcode = MXOP_LTR; break;
							case 4: case 5: ins->opcode = MXOP_VERR + mrm.f.reg - 4; break;
						}

						ins->argc = 1; {
							struct minx86dec_argv *m = &ins->argv[0];
							m->size = 2;
							decode_rm(mrm,m,isaddr32);
						}
					} break;
				case 0x01: /* LGDT */
					{
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0: case 1:
							case 2: case 3:
								ins->opcode = ((mrm.f.reg & 2) ? MXOP_LGDT : MXOP_SGDT) + (mrm.f.reg & 1);
								ins->argc = 1; {
									struct minx86dec_argv *m = &ins->argv[0];
									m->size = 6;	/* 16+32 */
									decode_rm(mrm,m,isaddr32);
								} break;
							case 4:
								ins->opcode = MXOP_SMSW;
								ins->argc = 1; {
									struct minx86dec_argv *m = &ins->argv[0];
									m->size = 2;
									decode_rm(mrm,m,isaddr32);
								} break;
							case 6:
								ins->opcode = MXOP_LMSW;
								ins->argc = 1; {
									struct minx86dec_argv *m = &ins->argv[0];
									m->size = 2;
									decode_rm(mrm,m,isaddr32);
								} break;
						}
					} break;
				case 0x02: /* LAR */
					ins->opcode = MXOP_LAR;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = s->size = data32wordsize;
						d->regtype = MX86_RT_REG;
						d->reg = mrm.f.reg;
						s->segment = seg_can_override(MX86_SEG_DS);
						decode_rm(mrm,s,isaddr32);
					} break;
				case 0x03: /* LSL */
					ins->opcode = MXOP_LSL;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = s->size = data32wordsize;
						d->regtype = MX86_RT_REG;
						d->reg = mrm.f.reg;
						s->segment = seg_can_override(MX86_SEG_DS);
						decode_rm(mrm,s,isaddr32);
					} break;
				case 0x06: /* CLTS */
					ins->opcode = MXOP_CLTS;
					ins->argc = 0;
					break;
# endif
# if core_level >= 3
				COVER_2(0xBC):
					ins->opcode = MXOP_BSF + (second_byte & 1);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						d->regtype = MX86_RT_REG;
						d->reg = mrm.f.reg;
						s->segment = seg_can_override(MX86_SEG_DS);
						decode_rm(mrm,s,isaddr32);
					}
					break;
# endif
				default:
					break;
			};
			} break;
#endif
#if core_level >= 1
		case 0x62:
			ins->opcode = MXOP_BOUND;
			ins->argc = 2; {
				struct minx86dec_argv *ai = &ins->argv[0];
				struct minx86dec_argv *mp = &ins->argv[1];
				union x86_mrm mrm = fetch_modregrm();
				ai->size = data32wordsize;
				mp->size = data32wordsize * 2;
				ai->regtype = MX86_RT_REG;
				ai->reg = mrm.f.reg;
				mp->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,mp,isaddr32);
			}
			break;
		COVER_4(0x6C):
			ins->opcode = MXOP_INS + ((first_byte >> 1) & 1);
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				r->size = (first_byte & 1) ? data32wordsize : 1;
				r->regtype = MX86_RT_NONE;
				r->segment = seg_can_override(MX86_SEG_DS);
				r->scalar = 0;
				r->memregs = 1;
				r->memref_base = 0;
				r->memregsz = addr32wordsize;
				r->memreg[0] = (first_byte & 2) ? MX86_REG_ESI : MX86_REG_EDI;
			}
			break;
		COVER_2(0xC8):
			ins->opcode = MXOP_ENTER + (first_byte & 1);
			ins->argc = first_byte == 0xC8 ? 2 : 0;
			if (ins->argc) { /* ENTER */
				struct minx86dec_argv *a = &ins->argv[0];
				struct minx86dec_argv *b = &ins->argv[1];
				a->size = 16;
				b->size = 8;
				a->regtype = b->regtype = MX86_RT_IMM;
				a->value = fetch_u16();
				b->value = fetch_u8();
			}
			break;
		COVER_2(0x60):
			ins->opcode = MXOP_PUSHA + (first_byte & 1) + (isdata32 ? 2 : 0);
			ins->argc = 0;
			break;
#endif
#if core_level >= 2
		case 0x63:
			ins->opcode = MXOP_ARPL;
			ins->argc = 2; {
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *s = &ins->argv[1];
				union x86_mrm mrm = fetch_modregrm();
				s->size = d->size = 2;
				s->regtype = MX86_RT_REG;
				s->reg = mrm.f.reg;
				d->segment = seg_can_override(MX86_SEG_DS);
				decode_rm(mrm,d,isaddr32);
			}
			break;
#endif
#if core_level >= 3
		/* 386+ instruction 32-bit prefixes */
		case 0x66: /* 32-bit data override */
			ins->data32 ^= 1;
			if (--patience) goto decode_next;
			break;

		case 0x67: /* 32-bit address override */
			ins->addr32 ^= 1;
			if (--patience) goto decode_next;
			break;
#endif

		default:
			/* fall through */
			break;
	};
}

#undef data32wordsize

/* } */

