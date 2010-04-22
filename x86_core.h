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
 * datatype is present. That way, compiling this code is possible even on a target that has no
 * 64-bit native support (such as: older DOS compilers!)
 *
 * ready? here we go! */
#define data32wordsize (isdata32 ? 4 : 2)
#define addr32wordsize (isaddr32 ? 4 : 2)
#define seg_can_override(x) (ins->segment >= 0 ? ins->segment : (x))

/* additional controls:
   
   no_salc             disable decoding of SALC undocumented instruction
   no_icebp            disable decoding to ICEBP undocumented ins.
   no_umov             disable decoding of UMOV undocumented ins.
   do_necv20           enable decoding of NEC V20/V30 undocumented ins.
   everything          decode everything, unless opcodes overlap lesser known opcodes in older revisions

 */

#define COVER_4(x) case (x): case (x+1): case (x+2): case (x+3)
#define COVER_2(x) case (x): case (x+1)

#define COVER_8(x) COVER_4(x): COVER_4(x+4)
#define COVER_ROW(x) COVER_8(x): COVER_8(x+8)

/* defaults */
#ifndef core_level
#  define core_level 0
/* 0 = 8086
 * 1 = 80186
 * 2 = 286
 * 3 = 386
 * 4 = 486
 * 5 = pentium or higher
 * 6 = pentium II or higher */
#endif

#ifndef fpu_level
#  define fpu_level -1
/* 0 = 8087
 * 2 = 287
 * 3 = 387
 * 4 = 487
 * 5 = pentium or higher
 * 6 = pentium II or higher */
#endif

#ifndef amd_3dnow
#  define amd_3dnow -1
/* 1 = 3dnow!
 * 2 = 3dnow2!
 * 3 = Geode 3dnow extensions */
#endif

#ifndef cyrix_level
#  define cyrix_level -1
/* 6 = cyrix 6x86
 * 5 = cyrix 5x86
 * 7 = MII
 * 131 = Via C3 Samuel
 * 132 = Via C3 Samuel 2
 * 133 = Via C3 Erza
 * 134 = Via C3 Nehemiah */
#endif

#ifndef sse_level
#  define sse_level -1
/* 1 = SSE
 * 2 = SSE2
 * 3 = SSE3
 * 4 = SSE4 */
#endif

#ifndef pentium
#  define pentium 0
/* 1 = pentium/ppro
 * 2 = pentium II
 * 3 = pentium III
 * 4 = pentium 4 */
#endif

#ifdef pentiumpro
/* pentium == 1 -> pentium pro */
#endif

#if defined(do_necv20) && core_level != 1
#  undef do_necv20
#endif

/* did we encounter FWAIT? (another odd prefix tacked on by Intel to instructions, yech!!) */
int fwait = 0;

ins->lock = 0;
ins->argv[0].segment = ins->argv[1].segment = ins->argv[2].segment = ins->argv[3].segment = ins->argv[4].segment = MX86_SEG_DS;
#if defined(vex_level)
ins->vex.raw = 0;
#endif
ins->rep = MX86_REP_NONE;
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
				set_immediate(imm,(first_byte & 1) ? (isdata32 ? fetch_u32() : fetch_u16()) : fetch_u8());
				set_register(reg,MX86_REG_AX);
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
				set_register(reg,mrm.f.reg);
				decode_rm(mrm,rm,isaddr32);
			} break;

		COVER_4(0x80):	/* immediate group 1 */
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[0];
				struct minx86dec_argv *imm = &ins->argv[1];
				ins->opcode = MXOP_ADD+mrm.f.reg;
				rm->size = imm->size = (first_byte & 1) ? data32wordsize : 1;
				decode_rm(mrm,rm,isaddr32);
				imm->regtype = MX86_RT_IMM;
				if (first_byte == 0x83)		imm->value = (uint32_t)((char)fetch_u8());
				else if (first_byte == 0x81)	imm->value = (isdata32 ? fetch_u32() : fetch_u16());
				else				imm->value = fetch_u8();
			} break;

		COVER_2(0x84):	/* TEST */
			ins->opcode = MXOP_TEST;
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[0];
				struct minx86dec_argv *reg = &ins->argv[1];
				rm->size = reg->size = (first_byte & 1) ? data32wordsize : 1;
				set_register(reg,mrm.f.reg);
				decode_rm(mrm,rm,isaddr32);
			} break;

		COVER_2(0x86):	/* XCHG */
			ins->opcode = MXOP_XCHG;
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[0];
				struct minx86dec_argv *reg = &ins->argv[1];
				rm->size = reg->size = (first_byte & 1) ? data32wordsize : 1;
				set_register(reg,mrm.f.reg);
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
				set_register(reg,mrm.f.reg);
				decode_rm(mrm,rm,isaddr32);
			} break;

		COVER_2(0xA4):
			string_instruction_typical(MXOP_MOVS);	/* <- warning: macro */
			break;

		COVER_2(0xA6):
			string_instruction_typical(MXOP_CMPS);	/* <- warning: macro */
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
			} break;

		case 0x8C: case 0x8E: /* mov r/m, seg reg */
			ins->opcode = MXOP_MOV;
			ins->argc = 2; {
				const int which = (first_byte>>1)&1;
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *rm = &ins->argv[which];
				struct minx86dec_argv *reg = &ins->argv[which^1];
				rm->size = reg->size = 2;
				set_segment_register(reg,mrm.f.reg);
				decode_rm(mrm,rm,isaddr32);
			} break;

		case 0x8D: /* LEA reg,mem */
			ins->opcode = MXOP_LEA;
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *reg = &ins->argv[0];
				struct minx86dec_argv *rm = &ins->argv[1];
				rm->size = reg->size = data32wordsize;
				set_register(reg,mrm.f.reg);
				decode_rm(mrm,rm,isaddr32);
			} break;

		case 0xE8:
			ins->opcode = MXOP_CALL;
			ins->argc = 1; {
				struct minx86dec_argv *mref = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				mref->size = mref->memregsz = data32wordsize;
				if (isdata32)	set_immediate(mref,(fetch_u32() + curp + 4) & 0xFFFFFFFFUL);
				else		set_immediate(mref,(fetch_u16() + curp + 2) & 0x0000FFFFUL);
			} break;

		case 0xE9:
			ins->opcode = MXOP_JMP;
			ins->argc = 1; {
				struct minx86dec_argv *mref = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				mref->size = mref->memregsz = data32wordsize;
				if (isdata32)	set_immediate(mref,(fetch_u32() + curp + 4) & 0xFFFFFFFFUL);
				else		set_immediate(mref,(fetch_u16() + curp + 2) & 0x0000FFFFUL);
			} break;

		case 0xF8:
			ins->opcode = MXOP_CLC;
			ins->argc = 0;
			break;

		case 0xF9:
			ins->opcode = MXOP_STC;
			ins->argc = 0;
			break;

		case 0xFC:
			ins->opcode = MXOP_CLD;
			ins->argc = 0;
			break;

		case 0xFD:
			ins->opcode = MXOP_STD;
			ins->argc = 0;
			break;

		case 0xF0:
			ins->lock = 1;
			goto decode_next;
#if !defined(no_icebp) && (core_level >= 3)
		case 0xF1:
			ins->opcode = MXOP_ICEBP;
			ins->argc = 0;
			break;
#endif
		case 0x9E:
			ins->opcode = MXOP_SAHF;
			ins->argc = 0;
			break;

		COVER_2(0xA8):
			ins->opcode = MXOP_TEST;
			ins->argc = 2; {
				struct minx86dec_argv *rm = &ins->argv[0];
				struct minx86dec_argv *imm = &ins->argv[1];
				rm->size = imm->size = data32wordsize;
				set_register(rm,MX86_REG_EAX);
				if (isdata32) set_immediate(imm,fetch_u32());
				else if (first_byte & 1) set_immediate(imm,fetch_u16());
				else set_immediate(imm,fetch_u8());
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
				set_segment_register(a,first_byte >> 3);
				a->size = data32wordsize;
			} break;

		case 0x27: case 0x2F: case 0x37: case 0x3F: /* DAA/DAS/AAA/AAS */
			ins->opcode = MXOP_DAA+((first_byte>>3)&3);
			ins->argc = 0;
			break;

		COVER_2(0xC0):
			ins->argc = 2; {
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *imm = &ins->argv[1];
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_ROL; break;
					case 1:	ins->opcode = MXOP_ROR; break;
					case 2:	ins->opcode = MXOP_RCL; break;
					case 3:	ins->opcode = MXOP_RCR; break;
					case 4:	ins->opcode = MXOP_SHL; break;
					case 5:	ins->opcode = MXOP_SHR; break;
					case 7:	ins->opcode = MXOP_SAR; break;
				};
				d->size = (first_byte & 1) ? data32wordsize : 1;
				decode_rm(mrm,d,isaddr32);
				imm->size = 1;
				set_immediate(imm,fetch_u8());
			} break;

		COVER_2(0xC2):
			ins->opcode = MXOP_RET;
			ins->argc = 0;
			if ((first_byte & 1) == 0) {
				struct minx86dec_argv *imm = &ins->argv[ins->argc++];
				imm->size = 2;
				set_immediate(imm,fetch_u16());
			} break;

		COVER_2(0xCA):
			ins->opcode = MXOP_RETF;
			ins->argc = 0;
			if ((first_byte & 1) == 0) {
				struct minx86dec_argv *imm = &ins->argv[ins->argc++];
				imm->size = 2;
				set_immediate(imm,fetch_u16());
			} break;

		COVER_4(0xD0):
			ins->argc = 2; {
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *imm = &ins->argv[1];
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_ROL; break;
					case 1:	ins->opcode = MXOP_ROR; break;
					case 2:	ins->opcode = MXOP_RCL; break;
					case 3:	ins->opcode = MXOP_RCR; break;
					case 4:	ins->opcode = MXOP_SHL; break;
					case 5:	ins->opcode = MXOP_SHR; break;
					case 7:	ins->opcode = MXOP_SAR; break;
				};
				d->size = (first_byte & 1) ? data32wordsize : 1;
				decode_rm(mrm,d,isaddr32);
				imm->size = 1;
				if (first_byte & 2) set_register(imm,MX86_REG_CL);
				else set_immediate(imm,1);
			} break;

		COVER_2(0xD4):
			ins->opcode = MXOP_AAM+(first_byte&1);
			ins->argc = 1; {
				struct minx86dec_argv *im = &ins->argv[0];
				set_immediate(im,fetch_u8());
				im->size = 1;
			} break;
#if !defined(no_salc)
		case 0xD6:
# if defined(umc) || defined(everything)
			if (ins->segment == MX86_SEG_FS) {	/* UMC identification */
				ins->opcode = MXOP_UMC_IDENT;
				ins->argc = 0;
			} else
# endif
			{
				ins->opcode = MXOP_SALC;
				ins->argc = 0;
			}
			break;
#endif

		case 0x8F:
			if (*cip >= 0x08) {
#  if defined(vex_level)
				union minx86dec_vex v;
				v.raw = fetch_u16() ^ 0xF8E0;

				ins->vex = v;
				switch (v.f.pp) {
					case 0x1: ins->data32 ^= 1; dataprefix32++; break;	/* as if 0x66 prefix */
					case 0x2: ins->rep = MX86_REPNE; break;			/* as if 0xF3 prefix */
					case 0x3: ins->rep = MX86_REPE; break;			/* as if 0xF2 prefix */
				};

				unsigned int vector_size = 16 << (v.f.l?1:0);
				const unsigned char opcode = *cip++;
				ins->oes = opcode & 3;

//				printf("v=0x%04X oes=%u\n",v.raw,ins->oes);

				switch (v.f.m) {
					case 0x8: {
						switch (opcode) {
							case 0x85:
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPMACSSWW;
									ins->argc = 4;
									d->size = s1->size = s2->size = s3->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									unsigned char c = fetch_u8();
									set_sse_register(s3,c>>4);
								} break;
							case 0x86:
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPMACSSWD;
									ins->argc = 4;
									d->size = s1->size = s2->size = s3->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									unsigned char c = fetch_u8();
									set_sse_register(s3,c>>4);
								} break;
							case 0x8E:
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPMACSSDD;
									ins->argc = 4;
									d->size = s1->size = s2->size = s3->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									unsigned char c = fetch_u8();
									set_sse_register(s3,c>>4);
								} break;
							case 0x95:
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPMACSWW;
									ins->argc = 4;
									d->size = s1->size = s2->size = s3->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									unsigned char c = fetch_u8();
									set_sse_register(s3,c>>4);
								} break;
							case 0x96:
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPMACSWD;
									ins->argc = 4;
									d->size = s1->size = s2->size = s3->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									unsigned char c = fetch_u8();
									set_sse_register(s3,c>>4);
								} break;
							COVER_2(0x9E):
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPMACSDD + (opcode & 1);
									ins->argc = 4;
									s3->size = 1;
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_immediate(s3,fetch_u8());
								} break;
							case 0xA2:
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPCMOV;
									ins->argc = 4;
									d->size = s1->size = s2->size = s3->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									unsigned char c = fetch_u8();
									set_sse_register(s3,c>>4);
								} break;
							COVER_4(0xCC):
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPCOMB + ins->oes;
									ins->argc = 4;
									s3->size = 1;
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_immediate(s3,fetch_u8());
								} break;
							COVER_4(0xEC):
								if (v.f.pp == 0) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *s3 = &ins->argv[3];
									ins->opcode = MXOP_VPCOMUB + ins->oes;
									ins->argc = 4;
									s3->size = 1;
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									set_sse_register(s1,v.f.v);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_immediate(s3,fetch_u8());
								} break;
						}
					} break;
					case 0x9: {
						switch (opcode) {
							COVER_2(0x80):
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VFRCZPS + (opcode & 1);
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							COVER_2(0x82):
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VFRCZSS + (opcode & 1);
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							COVER_4(0xC0):
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										if (opcode == 0xC0) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VPHADDBW + (opcode & 3) - 1;
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							COVER_2(0xC6):
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										if (opcode == 0xD0) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VPHADDWD + (opcode & 1);
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							case 0xCB:
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VPHADDDQ;
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							COVER_4(0xD0):
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										if (opcode == 0xD0) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VPHADDUBW + (opcode & 3) - 1;
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							COVER_2(0xD6):
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										if (opcode == 0xD0) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VPHADDUWD + (opcode & 1);
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							case 0xDB:
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VPHADDUDQ;
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
							COVER_4(0xE0):
								if (v.f.pp == 0) {
									if (v.f.v == 0) {
										if (v.f.l) break;
										if (opcode == 0xE0) break;
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										union x86_mrm mrm = fetch_modregrm();
										ins->opcode = MXOP_VPHSUBBW + (opcode & 3) - 1;
										ins->argc = 2;
										d->size = s->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
								} break;
						}
					} break;
				}
#  endif
			}
			else {
				struct minx86dec_argv *d = &ins->argv[0];
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						/* FIXME: is size specified or implied? */
						ins->opcode = MXOP_POP;
						d->size = data32wordsize;
						ins->argc = 1;
						decode_rm(mrm,d,isaddr32);
						break;
				}
			} break;

		case 0x9B:
			/* hold on... check next opcode */
			if (cip[0] == 0xD9) {
				if (cip[1] >= 0xC0) {
				}
				else {
					int reg = (cip[1] >> 3) & 7;
					if (reg == 6 || reg == 7) {
						fwait++;
						goto decode_next;
					}
				}
			}
			else if (cip[0] == 0xDB && (cip[1] == 0xE0 || cip[1] == 0xE1 || cip[1] == 0xE2 || cip[1] == 0xE3)) {
				fwait++;
				goto decode_next;
			}
			else if (cip[0] == 0xDD) {
				if (cip[1] >= 0xC0) {
				}
				else {
					int reg = (cip[1] >> 3) & 7;
					if (reg == 4 || reg == 6 || reg == 7) {
						fwait++;
						goto decode_next;
					}
				}
			}
			else if (cip[0] == 0xDF) {
				if (cip[1] >= 0xC0) {
					if (cip[1] == 0xE0) {
						fwait++;
						goto decode_next;
					}
				}
				else {
					int reg = (cip[1] >> 3) & 7;
					if (0) {
						fwait++;
						goto decode_next;
					}
				}
			}
			ins->opcode = MXOP_FWAIT;
			ins->argc = 0;
			break;

		case 0x26: case 0x2E: case 0x36: case 0x3E: /* segment overrides */
			ins->argv[0].segment = ins->argv[1].segment =
			ins->argv[2].segment = ins->argv[3].segment =
			ins->segment = (first_byte >> 3) & 3;
			if (--patience) goto decode_next;
			break;

		/* INC/DEC register */
		COVER_ROW(0x40):
			ins->opcode = MXOP_INC+((first_byte>>3)&1);
			ins->argc = 1; {
				struct minx86dec_argv *reg = &ins->argv[0];
				set_register(reg,first_byte & 7);
				reg->size = data32wordsize;
			} break;

		/* PUSH/POP register */
		COVER_ROW(0x50):
			ins->opcode = MXOP_PUSH+((first_byte>>3)&1);
			ins->argc = 1; {
				struct minx86dec_argv *reg = &ins->argv[0];
				set_register(reg,first_byte & 7);
				reg->size = data32wordsize;
			} break;

		/* Jcc short */
		COVER_ROW(0x70):
			ins->opcode = MXOP_JO+(first_byte&0xF);
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				set_immediate(r,curp + 1 + ((uint32_t)((char)fetch_u8())));
				r->size = addr32wordsize;
			} break;

		/* JCXZ */
		case 0xE3:
			ins->opcode = MXOP_JCXZ;
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				set_immediate(r,curp + 1 + ((uint32_t)((char)fetch_u8())));
				r->size = addr32wordsize;
			} break;

		/* JMP */
		case 0xEB:
			ins->opcode = MXOP_JMP;
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				set_immediate(r,curp + 1 + ((uint32_t)((char)fetch_u8())));
				r->size = addr32wordsize;
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
				set_mem_ref_imm(mref,isaddr32 ? fetch_u32() : fetch_u16());
				set_register(areg,MX86_REG_AX);
			} break;

		/* NOP */
		case 0x90:
			if (ins->rep == MX86_REPNE) ins->opcode = MXOP_PAUSE;
			else ins->opcode = MXOP_NOP;
			break;

		/* XCHG AX,[reg] */
		COVER_4(0x91): COVER_2(0x95): case 0x97:
			ins->opcode = MXOP_XCHG;
			ins->argc = 2; {
				struct minx86dec_argv *a = &ins->argv[0];
				struct minx86dec_argv *r = &ins->argv[1];
				a->size = r->size = data32wordsize;
				set_register(r,first_byte & 7);
				set_register(a,MX86_REG_AX);
			} break;

		case 0x98:
			ins->opcode = isdata32 ? MXOP_CWDE : MXOP_CBW;
			ins->argc = 0;
			break;

		case 0x99:
			ins->opcode = MXOP_CWD + (isdata32 & 1);
			ins->argc = 0;
			break;

		case 0x9F:
			ins->opcode = MXOP_LAHF;
			ins->argc = 0;
			break;

		COVER_2(0xE4):
			ins->opcode = MXOP_IN;
			ins->argc = 2; {
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *s = &ins->argv[1];
				d->size = data32wordsize;
				set_register(d,MX86_REG_AX);
				set_immediate(s,fetch_u8());
			} break;

		COVER_2(0xEC):
			ins->opcode = MXOP_IN;
			ins->argc = 2; {
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *s = &ins->argv[1];
				d->size = data32wordsize;
				set_register(d,MX86_REG_AX);
				s->size = 2;	/* always DX, Intel chips can only address 65536 I/O ports */
				set_register(s,MX86_REG_DX);
			} break;

		case 0x9A:
			ins->opcode = MXOP_CALL_FAR;
			ins->argc = 1; {
				struct minx86dec_argv *mref = &ins->argv[0];
				mref->size = mref->memregsz = data32wordsize + 2;
				mref->regtype = MX86_RT_IMM;
				mref->segment = MX86_SEG_IMM;
				if (isdata32)	mref->value = fetch_u32();
				else		mref->value = fetch_u16();
				mref->segval = fetch_u16();
			} break;

		case 0x9C:
			ins->opcode = isdata32 ? MXOP_PUSHFD : MXOP_PUSHF;
			ins->argc = 0;
			break;

		case 0x9D:
			ins->opcode = isdata32 ? MXOP_POPFD : MXOP_POPF;
			ins->argc = 0;
			break;

		case 0xEA:
			ins->opcode = MXOP_JMP_FAR;
			ins->argc = 1; {
				struct minx86dec_argv *mref = &ins->argv[0];
				mref->size = mref->memregsz = data32wordsize + 2;
				mref->regtype = MX86_RT_IMM;
				mref->segment = MX86_SEG_IMM;
				if (isdata32)	mref->value = fetch_u32();
				else		mref->value = fetch_u16();
				mref->segval = fetch_u16();
			} break;

		COVER_2(0xF6): {
			ins->argc = 1;
			struct minx86dec_argv *where = &ins->argv[0];
			struct minx86dec_argv *imm = &ins->argv[1];
			union x86_mrm mrm = fetch_modregrm();
			where->size = where->memregsz = imm->size = (first_byte & 1) ? data32wordsize : 1;
			where->regtype = MX86_RT_NONE;
			decode_rm(mrm,where,isaddr32);
			static int map_f6[8] = {MXOP_TEST,MXOP_UD,MXOP_NOT,MXOP_NEG, MXOP_MUL,MXOP_IMUL,MXOP_DIV,MXOP_IDIV};
			ins->opcode = map_f6[mrm.f.reg]; ins->argc = 1;
			if (mrm.f.reg == 0) {
				ins->argc++;
				set_immediate(imm,(first_byte & 1) ? imm32bysize(ins) : fetch_u8());
			}
			break; }

		/* group 0xFE-0xFF */
		COVER_2(0xFE): {
			union x86_mrm mrm = fetch_modregrm();
			switch (mrm.f.reg) {
				case 0: {
					struct minx86dec_argv *where = &ins->argv[0];
					ins->argc = 1;
					ins->opcode = MXOP_INC;
					where->size = where->memregsz = (first_byte & 1) ? data32wordsize : 1;
					where->regtype = MX86_RT_NONE;
					decode_rm(mrm,where,isaddr32);
				} break;
				case 1: {
					struct minx86dec_argv *where = &ins->argv[0];
					ins->argc = 1;
					ins->opcode = MXOP_DEC;
					where->size = where->memregsz = (first_byte & 1) ? data32wordsize : 1;
					where->regtype = MX86_RT_NONE;
					decode_rm(mrm,where,isaddr32);
				} break;
				case 2: case 3: {
					struct minx86dec_argv *where = &ins->argv[0];
					const unsigned int sz = data32wordsize + ((mrm.f.reg & 1) ? 2 : 0);
					if (mrm.f.mod == 3 && (mrm.f.reg&1)) break; /* illegal encoding */
					ins->argc = 1;
					ins->opcode = MXOP_CALL + (mrm.f.reg & 1);
					where->size = where->memregsz = data32wordsize + ((mrm.f.reg & 1) ? 2 : 0);
					where->regtype = MX86_RT_NONE;
					decode_rm(mrm,where,isaddr32);
				} break;
				case 4: case 5: {
					struct minx86dec_argv *where = &ins->argv[0];
					const unsigned int sz = data32wordsize + ((mrm.f.reg & 1) ? 2 : 0);
					if (mrm.f.mod == 3 && (mrm.f.reg&1)) break; /* illegal encoding */
					ins->argc = 1;
					ins->opcode = MXOP_JMP + (mrm.f.reg & 1);
					where->size = where->memregsz = sz;
					where->regtype = MX86_RT_NONE;
					decode_rm(mrm,where,isaddr32);
				} break;
				case 6: case 7: {
					struct minx86dec_argv *where = &ins->argv[0];
					const unsigned int sz = data32wordsize + ((mrm.f.reg & 1) ? 2 : 0);
					if (mrm.f.mod == 3 && (mrm.f.reg&1)) break; /* illegal encoding */
					ins->argc = 1;
					ins->opcode = MXOP_PUSH + (mrm.f.reg & 1);
					where->size = where->memregsz = sz;
					where->regtype = MX86_RT_NONE;
					decode_rm(mrm,where,isaddr32);
				} break;
			}
			break; }

		COVER_2(0xC4): /* LDS/LES */
			if ((*cip & 0xC0) == 0xC0) { /* NOPE! AVX/VEX extensions (illegal encoding of LDS/LES) */
#  if defined(vex_level)
				union minx86dec_vex v;

				if (first_byte & 1) { /* 2-byte */
					v.raw = (fetch_u8() ^ 0xF8) << 8;
					v.f.r = v.f.w; v.f.w = 0; /* transpose bit 7 to bit 15 to convert 2 to 3 byte VEX */
					v.f.m = 1; /* "implied 0x0F prefix" */
				}
				else { /* 3-byte */
					v.raw = fetch_u16() ^ 0xF8E0;
				}

				ins->vex = v;
				switch (v.f.pp) {
					case 0x1: ins->data32 ^= 1; dataprefix32++; break;	/* as if 0x66 prefix */
					case 0x2: ins->rep = MX86_REPNE; break;			/* as if 0xF3 prefix */
					case 0x3: ins->rep = MX86_REPE; break;			/* as if 0xF2 prefix */
				};

				unsigned int vector_size = 16 << (v.f.l?1:0);
				switch (v.f.m) {
					/* as if 0x0F is first byte.
					 * since the instruction changes quite a bit, it's better to duplicate it
					 * than slow down decoding of the original with "if VEX is present blah blah" crap */
					case 1: {
						const uint8_t second_byte = *cip++;
						switch (second_byte) {
							COVER_2(0x12): { /* TODO clean this up */
								const unsigned int which = second_byte & 1;
								struct minx86dec_argv *d = &ins->argv[which];
								struct minx86dec_argv *s = &ins->argv[which^1];
								union x86_mrm mrm = fetch_modregrm();
								if (ins->rep == MX86_REPNE)
									ins->opcode = MXOP_MOVSLDUP;
								else if (ins->rep == MX86_REPE)
									ins->opcode = MXOP_MOVDDUP;
								else
									ins->opcode = dataprefix32 ? MXOP_MOVLPD : MXOP_MOVLPS;
								ins->argc = 2;
								d->size = vector_size;
								s->size = vector_size>>1;
								set_sse_register(d,mrm.f.reg);
								decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								/* Hm..... so the reg-reg version of MXOP_MOVLPS is MXOP_MOVHLPS? */
								if (ins->opcode == MXOP_MOVLPS && s->regtype != MX86_RT_NONE && !which) {
									struct minx86dec_argv *sl = &ins->argv[2];
									ins->opcode = MXOP_MOVHLPS;
									ins->argc = 3;
									*sl = *s;
									s->size = sl->size = vector_size;
									set_sse_register(s,v.f.v);
								}

								} break;
							COVER_2(0x28):
								ins->opcode = MXOP_MOVAPS - (dataprefix32 & 1);
								ins->argc = 2; {
									const unsigned int which = second_byte & 1;
									struct minx86dec_argv *re = &ins->argv[which];
									struct minx86dec_argv *rm = &ins->argv[which^1];
									union x86_mrm mrm = fetch_modregrm();
									re->size = rm->size = vector_size;
									set_sse_register(re,mrm.f.reg);
									decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
								} break;
							case 0x2A:
								if (ins->rep >= MX86_REPE) {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = ins->rep == MX86_REPNE ? MXOP_CVTSI2SS : MXOP_CVTSI2SD;
									ins->argc = 2;
									d->size = 16;
									set_sse_register(d,mrm.f.reg);
									s->size = 4; /* 128 bit = 16 bytes */
									decode_rm(mrm,s,isaddr32);
								}
								else {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = MXOP_CVTPI2PS + (dataprefix32 & 1);
									ins->argc = 2;
									d->size = 16;
									set_sse_register(d,mrm.f.reg);
									s->size = 8; /* 128 bit = 16 bytes */
									decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_MMX);
								}
								break;
							case 0x2C: {
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s = &ins->argv[1];
								union x86_mrm mrm = fetch_modregrm();
								if (ins->rep >= MX86_REPE)
									ins->opcode = MXOP_CVTTSD2SI + ins->rep - MX86_REPE;
								else
									ins->opcode = MXOP_CVTTPS2PI + (dataprefix32 & 1);
								ins->argc = 2;
								if (ins->rep >= MX86_REPE) {
									d->size = 4;
									d->regtype = MX86_RT_REG;
								}
								else {
									d->size = 8;
									d->regtype = MX86_RT_MMX;
								}
								s->size = 16; /* 128 bit = 16 bytes */
								d->reg = mrm.f.reg;
								s->regtype = MX86_RT_SSE;
								decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								break; }
							case 0x2D:
								if (ins->rep >= MX86_REPE) {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = ins->rep == MX86_REPNE ? MXOP_CVTSS2SI : MXOP_CVTSD2SI;
									ins->argc = 2;
									d->size = 4;
									set_register(d,mrm.f.reg);
									s->size = 16; /* 128 bit = 16 bytes */
									s->regtype = MX86_RT_SSE;
									decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								}
								else {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = MXOP_CVTPS2PI + (dataprefix32 & 1);
									ins->argc = 2;
									d->size = 8;
									set_mmx_register(d,mrm.f.reg);
									s->size = 16; /* 128 bit = 16 bytes */
									s->regtype = MX86_RT_SSE;
									decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								}
								break;
							case 0x2F:
								if (v.f.l) break; /* forms not supported with L=1 */
								if (v.f.v != 0) break; /* vvvv != 1111 is undefined */
								ins->opcode = MXOP_COMISS + (dataprefix32 ? 1 : 0);
								ins->argc = 2; {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									d->size = s->size = 16; /* 128 bit = 16 bytes */
									d->reg = mrm.f.reg;
									decode_rm_ex(mrm,s,isaddr32,d->regtype = MX86_RT_SSE);
								} break;
							COVER_2(0x54):
								ins->opcode = dataprefix32 + MXOP_ANDPS + ((second_byte & 1) << 1);
								ins->argc = 3; {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									union x86_mrm mrm = fetch_modregrm();
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
								} break;
							case 0x58:
								if (v.f.l && ins->rep != 0) break; /* forms involving ADDSS/ADDSD not supported with L=1 */
								ins->opcode = (ins->rep >= MX86_REPE ? (2 + ins->rep - MX86_REPE) : dataprefix32) + MXOP_ADDPS;
								ins->argc = 3; {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									union x86_mrm mrm = fetch_modregrm();
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
								} break;
							case 0x5A:
								if (ins->rep >= MX86_REPE) {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = ins->rep == MX86_REPNE ? MXOP_CVTSS2SD : MXOP_CVTSD2SS;
									ins->argc = 2;
									d->size = 16;
									set_sse_register(d,mrm.f.reg);
									s->size = 16; /* 128 bit = 16 bytes */
									decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_SSE);
								}
								else {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = MXOP_CVTPS2PD + (dataprefix32 & 1);
									ins->argc = 2;
									d->size = (dataprefix32&1) == 0 ? vector_size : 16;
									set_sse_register(d,mrm.f.reg);
									s->size = (dataprefix32&1) == 1 ? vector_size : 16;
									decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_SSE);
								}
								break;
							case 0x5B: {
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s = &ins->argv[1];
								union x86_mrm mrm = fetch_modregrm();
								if (ins->rep == MX86_REPNE) {
									ins->opcode = MXOP_CVTTPS2DQ;
								}
								else if (ins->rep == MX86_REPE) {
									/* not defined */
									break;
								}
								else {
									ins->opcode = MXOP_CVTDQ2PS + (dataprefix32 & 1);
								}
								ins->argc = 2;
								d->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								s->size = vector_size;
								decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_SSE);
								break; }
							case 0x5D:
								ins->opcode = (ins->rep >= MX86_REPE) ? (ins->rep + MXOP_MINSD - MX86_REPE) : (MXOP_MINPS - dataprefix32);
								ins->argc = 3; {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									union x86_mrm mrm = fetch_modregrm();
									s1->size = s2->size = d->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
								} break;
							case 0x5E: {
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								union x86_mrm mrm = fetch_modregrm();
								if (ins->rep >= MX86_REPE)
									ins->opcode = MXOP_DIVSD + ins->rep - MX86_REPE;
								else if (dataprefix32)
									ins->opcode = MXOP_DIVPD;
								else
									ins->opcode = MXOP_DIVPS;

								ins->argc = 2;
								d->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								s2->size = s1->size = vector_size;
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								set_sse_register(s1,v.f.v);
								} break;
							case 0x5F:
								if (v.f.l && ins->rep != 0) break; /* forms involving ADDSS/ADDSD not supported with L=1 */
								ins->opcode = (ins->rep >= MX86_REPE) ? (ins->rep + MXOP_MAXSD - MX86_REPE) : (MXOP_MAXPS - dataprefix32);
								ins->argc = 3; {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									union x86_mrm mrm = fetch_modregrm();
									s1->size = s2->size = d->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
								} break;

							case 0x6E:
							case 0x7E:
								if (ins->rep >= MX86_REPE) {
									if (second_byte == 0x7E && ins->rep == MX86_REPNE) {
										ins->opcode = MXOP_MOVQ;
										ins->argc = 2; {
											struct minx86dec_argv *re = &ins->argv[0];
											struct minx86dec_argv *rm = &ins->argv[1];
											union x86_mrm mrm = fetch_modregrm();
											rm->size = re->size = vector_size;
											set_sse_register(re,mrm.f.reg);
											decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
										}
									}
								}
								else {
									ins->opcode = MXOP_MOVD;
									ins->argc = 2; {
										const unsigned int which = (second_byte >> 4) & 1;
										struct minx86dec_argv *re = &ins->argv[which];
										struct minx86dec_argv *rm = &ins->argv[which^1];
										union x86_mrm mrm = fetch_modregrm();
										rm->size = 4;
										if (dataprefix32) {
											re->size = vector_size;
											set_sse_register(re,mrm.f.reg);
											decode_rm(mrm,rm,isaddr32);
										}
										else {
											re->size = 8;
											set_mmx_register(re,mrm.f.reg);
											decode_rm(mrm,rm,isaddr32);
										}
									}
								} break;
							case 0x6F:
							case 0x7F:
								if (ins->rep == MX86_REPE) {
								}
								else {
									const unsigned int which = (second_byte >> 4) & 1;
									struct minx86dec_argv *d = &ins->argv[which];
									struct minx86dec_argv *s = &ins->argv[which^1];
									union x86_mrm mrm = fetch_modregrm();

									ins->argc = 2;
									ins->opcode = (ins->rep >= MX86_REPE) ? MXOP_MOVDQU :
										(dataprefix32 ? MXOP_MOVDQA : MXOP_MOVQ);

									if (dataprefix32 || ins->rep >= MX86_REPE) {
										s->size = d->size = vector_size;
										set_sse_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
									}
									else {
										s->size = d->size = 8;
										set_mmx_register(d,mrm.f.reg);
										decode_rm_ex(mrm,s,isaddr32,MX86_RT_MMX);
									}
								}
								break;
							COVER_2(0x7C):
								if (dataprefix32) {
									ins->argc = 3;
									ins->opcode = (second_byte & 1) ? MXOP_HSUBPD : MXOP_HADDPD;
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									union x86_mrm mrm = fetch_modregrm();
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
								}
								else if (ins->rep == MX86_REPE) {
									ins->argc = 3;
									ins->opcode = (second_byte & 1) ? MXOP_HSUBPS : MXOP_HADDPS;
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									union x86_mrm mrm = fetch_modregrm();
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
								}
								break;
							case 0xC2:
								if (v.f.l && ins->rep != 0) break; /* forms involving CMPSS/CMPSD not supported with L=1 */
								ins->opcode = (ins->rep >= MX86_REPE) ? (MXOP_CMPSD + ins->rep - MX86_REPE) : (MXOP_CMPPS + (dataprefix32 ? 1 : 0));
								ins->argc = 4; {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									struct minx86dec_argv *i = &ins->argv[3];
									union x86_mrm mrm = fetch_modregrm();
									i->size = 8;
									d->size = s1->size = s2->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
									set_immediate(i,fetch_u8());
								} break;
							case 0xD0: {
#   define PAIR(d,r)  ((d) + ((r) << 2))
								   struct minx86dec_argv *d = &ins->argv[0];
								   struct minx86dec_argv *s1 = &ins->argv[1];
								   struct minx86dec_argv *s2 = &ins->argv[2];
								   unsigned char t = dataprefix32 + (ins->rep << 2);
								   unsigned char m = 0;
								   switch (t) {
									   case PAIR(1,MX86_REP_NONE):
										   ins->opcode = MXOP_ADDSUBPD;
										   m = 1;
										   break;
									   case PAIR(0,MX86_REPE):
										   ins->opcode = MXOP_ADDSUBPS;
										   m = 1;
										   break;
								   };
								   switch (m) {
									   case 1: { /* ADDSUBPD/S */
											   union x86_mrm mrm = fetch_modregrm();
											   ins->argc = 3;
											   d->size = s1->size = s2->size = vector_size;
											   set_sse_register(d,mrm.f.reg);
											   decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
											   set_sse_register(s1,v.f.v);
										   } break;
									   }
#   undef PAIR
								} break;
							case 0xD6:
								if (ins->rep >= MX86_REPE) {
									const unsigned int which = ins->rep - MX86_REPE;
									struct minx86dec_argv *mm = &ins->argv[which^1];
									struct minx86dec_argv *xmm = &ins->argv[which];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = which ? MXOP_MOVQ2DQ : MXOP_MOVDQ2Q;
									ins->argc = 2;
									mm->size = 8;
									xmm->size = 16;
									set_mmx_register(mm,mrm.f.reg);
									decode_rm_ex(mrm,xmm,isaddr32,MX86_RT_SSE);
								}
								else if (dataprefix32) {
									struct minx86dec_argv *d = &ins->argv[1];
									struct minx86dec_argv *s = &ins->argv[0];
									union x86_mrm mrm = fetch_modregrm();
									ins->opcode = MXOP_MOVQ;
									ins->argc = 2;
									d->size = 8;
									s->size = 16;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								}
								break;
							case 0xF0:
								ins->opcode = MXOP_LDDQU;
								ins->argc = 2; {
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s = &ins->argv[1];
									union x86_mrm mrm = fetch_modregrm();
									d->size = s->size = vector_size;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								} break;
							case 0xE6: {
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s = &ins->argv[1];
								union x86_mrm mrm = fetch_modregrm();
								d->size = s->size = vector_size;
								if (ins->rep >= MX86_REPE) {
									ins->opcode = MXOP_CVTPD2DQ + ins->rep - MX86_REPE;
									if (ins->rep == MX86_REPE) d->size >>= 1;
									else s->size >>= 1;
								}
								else if (dataprefix32)
									ins->opcode = MXOP_CVTTPD2DQ;
								else
									break;

								ins->argc = 2;
								set_sse_register(d,mrm.f.reg);
								decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								} break;
						} break;
					}
					case 2: { /* 0x0F 0x38 */
						/* some instruction decoding is very picky on purpose
						 * because Intel says that certain fields must be a
						 * certain way or #UD will occur. This suggests future
						 * additions to AVX/VEX extensions will differentiate
						 * by these values */
						const uint8_t third_byte = *cip++;
						switch (third_byte) {
							case 0x18: { 
								if (dataprefix32) {
									if (v.f.v == 0) {/* VBROADCASTSS */
										union x86_mrm mrm = fetch_modregrm();
										struct minx86dec_argv *d = &ins->argv[0];
										struct minx86dec_argv *s = &ins->argv[1];
										ins->opcode = MXOP_BROADCASTSS;
										d->size = vector_size;
										s->size = 4;
										ins->argc = 2;
										set_sse_register(d,mrm.f.reg);
										decode_rm(mrm,s,isaddr32);
									}
								}
							} break;
							case 0x19: { 
								if (dataprefix32) {
									if (v.f.l) {
										if (v.f.v == 0) {/* VBROADCASTSD */
											union x86_mrm mrm = fetch_modregrm();
											struct minx86dec_argv *d = &ins->argv[0];
											struct minx86dec_argv *s = &ins->argv[1];
											ins->opcode = MXOP_BROADCASTSD;
											d->size = 32;
											s->size = 8;
											ins->argc = 2;
											set_sse_register(d,mrm.f.reg);
											decode_rm(mrm,s,isaddr32);
										}
									}
								}
							} break;
							case 0x1A: { 
								if (dataprefix32) {
									if (v.f.l) {
										if (v.f.v == 0) {/* VBROADCASTF128 */
											union x86_mrm mrm = fetch_modregrm();
											struct minx86dec_argv *d = &ins->argv[0];
											struct minx86dec_argv *s = &ins->argv[1];
											ins->opcode = MXOP_BROADCASTSD;
											d->size = 32;
											s->size = 16;
											ins->argc = 2;
											set_sse_register(d,mrm.f.reg);
											decode_rm(mrm,s,isaddr32);
										}
									}
								}
							} break;
							case 0xDB: { /* AES IMC */
								if (dataprefix32) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s2 = &ins->argv[1];
									ins->opcode = MXOP_AESIMC;
									d->size = s2->size = 16;
									ins->argc = 2;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								}
							} break;
							COVER_4(0xDC): { /* AES ENC/DEC */
								if (dataprefix32) {
									union x86_mrm mrm = fetch_modregrm();
									struct minx86dec_argv *d = &ins->argv[0];
									struct minx86dec_argv *s1 = &ins->argv[1];
									struct minx86dec_argv *s2 = &ins->argv[2];
									ins->opcode = MXOP_AESENC + (third_byte & 3);
									d->size = s1->size = s2->size = vector_size;
									ins->argc = 3;
									set_sse_register(d,mrm.f.reg);
									decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
									set_sse_register(s1,v.f.v);
								}
							} break;
						}
						break;
					}
					case 3: { /* 0x0F 0x3A */
						const uint8_t third_byte = *cip++;
						switch (third_byte) {
							COVER_2(0x0C): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *i = &ins->argv[3];
								ins->opcode = MXOP_BLENDPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								i->size = 1;
								set_immediate(i,fetch_u8());
							} break;

							COVER_2(0x40): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *i = &ins->argv[3];
								ins->opcode = MXOP_DPPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = 16;
								set_sse_register(d,mrm.f.reg);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								i->size = 1;
								set_immediate(i,fetch_u8());
								set_sse_register(s1,v.f.v);
							} break;

							COVER_2(0x48): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								struct minx86dec_argv *s4 = &ins->argv[4];
								unsigned char imm8;
								ins->opcode = MXOP_VPERMIL2PS + (third_byte & 1);
								ins->argc = 5;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								imm8 = fetch_u8();
								set_sse_register(s3,imm8>>4);
								set_sse_register(s4,imm8&0xF);
							} break;

							COVER_2(0x4A): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								unsigned char imm8;
								ins->opcode = MXOP_BLENDVPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								imm8 = fetch_u8();
								set_sse_register(s3,imm8>>4);
							} break;

							COVER_2(0x5C): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFMADDSUBPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							COVER_2(0x5E): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFMSUBADDPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							COVER_2(0x68): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFMADDPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							COVER_2(0x6A): {
								if (v.f.l) break; /* long is illegal */
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFMADDSS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							COVER_2(0x6C): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFMSUBPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							COVER_2(0x6E): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFMSUBSS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							COVER_2(0x78): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFNMADDPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							COVER_2(0x7C): {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s1 = &ins->argv[1];
								struct minx86dec_argv *s2 = &ins->argv[2];
								struct minx86dec_argv *s3 = &ins->argv[3];
								ins->opcode = MXOP_VFNMSUBPS + (third_byte & 1);
								ins->argc = 4;
								d->size = s1->size = s2->size = s3->size = vector_size;
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
								decode_rm_ex(mrm,s2,isaddr32,MX86_RT_SSE);
								unsigned char c = fetch_u8();
								set_sse_register(s3,c>>4);
							} break;

							case 0xDF: {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s = &ins->argv[1];
								struct minx86dec_argv *i = &ins->argv[2];
								ins->opcode = MXOP_AESKEYGENASSIST;
								ins->argc = 3;
								d->size = s->size = 16;
								set_sse_register(d,mrm.f.reg);
								decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
								i->size = 1;
								set_immediate(i,fetch_u8());
							} break;
						}
						break;
					}
				}
#  else
				break; /* illegal encoding */
#  endif
			}
			else {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *s = &ins->argv[1];
				d->size = data32wordsize;
				s->size = data32wordsize + 2;
				set_register(d,mrm.f.reg);
				decode_rm(mrm,s,isaddr32);
				ins->opcode = (first_byte & 1) ? MXOP_LDS : MXOP_LES;
				ins->argc = 2;
			}
			break;

		COVER_2(0xC6): {
			union x86_mrm mrm = fetch_modregrm();
			switch (mrm.f.reg) {
				case 0: {
					ins->argc = 2;
					ins->opcode = MXOP_MOV;
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					s->size = d->size = (first_byte & 1) ? data32wordsize : 1;
					decode_rm(mrm,d,isaddr32);
					switch (d->size) {
						case 1:	set_immediate(s,fetch_u8()); break;
						case 2:	set_immediate(s,fetch_u16()); break;
						case 4:	set_immediate(s,fetch_u32()); break;
					};
				} break;
			} } break;

		/* INT 3 */
		case 0xCC:
			ins->opcode = MXOP_INT;
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				set_immediate(r,3);
			} break;

		/* INT N */
		case 0xCD:
			ins->opcode = MXOP_INT;
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				set_immediate(r,fetch_u8());
			} break;

		/* INTO */
		case 0xCE:
			ins->opcode = MXOP_INTO;
			ins->argc = 0;
			break;

		/* IRET */
		case 0xCF:
			ins->opcode = isdata32 ? MXOP_IRETD : MXOP_IRET;
			ins->argc = 0;
			break;

		/* XLAT */
		case 0xD7:
			ins->opcode = MXOP_XLAT;
			ins->argc = 0;
			break;

		/* LOOPNE */
		case 0xE0:
			ins->opcode = MXOP_LOOPNE;
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				r->size = data32wordsize;
				if (isdata32)	set_immediate(r,((char)fetch_u8() + curp + 1) & 0xFFFFFFFFUL);
				else		set_immediate(r,((char)fetch_u8() + curp + 1) & 0x0000FFFFUL);
			} break;

		/* LOOPE */
		case 0xE1:
			ins->opcode = MXOP_LOOPE;
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				r->size = data32wordsize;
				if (isdata32)	set_immediate(r,((char)fetch_u8() + curp + 1) & 0xFFFFFFFFUL);
				else		set_immediate(r,((char)fetch_u8() + curp + 1) & 0x0000FFFFUL);
			} break;

		/* LOOP */
		case 0xE2:
			ins->opcode = MXOP_LOOP;
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				r->size = data32wordsize;
				if (isdata32)	set_immediate(r,((char)fetch_u8() + curp + 1) & 0xFFFFFFFFUL);
				else		set_immediate(r,((char)fetch_u8() + curp + 1) & 0x0000FFFFUL);
			} break;

		COVER_2(0xE6):
			ins->opcode = MXOP_OUT;
			ins->argc = 2; {
				struct minx86dec_argv *ioport = &ins->argv[0];
				struct minx86dec_argv *reg = &ins->argv[1];
				set_immediate(ioport,fetch_u8());
				reg->size = (first_byte & 1) ? data32wordsize : 1;
				set_register(reg,MX86_REG_AX);
			} break;

		COVER_2(0xEE):
			ins->opcode = MXOP_OUT;
			ins->argc = 2; {
				struct minx86dec_argv *rdx = &ins->argv[0];
				struct minx86dec_argv *rax = &ins->argv[1];
				rax->size = (first_byte & 1) ? data32wordsize : 1;
				rdx->size = 2;
				set_register(rax,MX86_REG_AX);
				set_register(rdx,MX86_REG_DX);
			} break;

		/* REP/REPE/REPNE */
		COVER_2(0xF2):
			ins->rep = (first_byte & 1) + MX86_REPE;
			goto decode_next;

		/* HLT */
		case 0xF4:
			ins->opcode = MXOP_HLT;
			ins->argc = 0;
			break;

		/* CMC */
		case 0xF5:
			ins->opcode = MXOP_CMC;
			ins->argc = 0;
			break;

		/* CLI */
		case 0xFA:
			ins->opcode = MXOP_CLI;
			ins->argc = 0;
			break;

		/* STI */
		case 0xFB:
			ins->opcode = MXOP_STI;
			ins->argc = 0;
			break;

# if defined(do_necv20) /* NEC V20/V30 */
		COVER_2(0x66): /* FP02 conflicts with 386+ address/data prefix */
			ins->opcode = MXOP_FP02;
			ins->argc = 2; {
				union x86_mrm mrm = fetch_modregrm();
				struct minx86dec_argv *s = &ins->argv[0];
				struct minx86dec_argv *d = &ins->argv[1];
				d->size = s->size = 2;
				set_immediate(d,mrm.f.reg | ((first_byte & 1) << 3));
				set_register(s,mrm.f.rm);
				decode_rm(mrm,s,0);
			} break;
		COVER_2(0x64): /* REPC/REPNC */
			ins->rep = (first_byte & 1) + MX86_REPNC;
			goto decode_next;
#endif

#if core_level >= 3
		case 0x64: case 0x65: /* segment overrides FS and GS */
			ins->segment = (first_byte & 1) + MX86_SEG_FS;
			if (--patience) goto decode_next;
			break;
#endif

		/* MOV a,imm */
		COVER_ROW(0xB0):
			ins->opcode = MXOP_MOV;
			ins->argc = 2; {
				struct minx86dec_argv *r = &ins->argv[0];
				struct minx86dec_argv *imm = &ins->argv[1];
				r->size = imm->size = (first_byte & 8) ? data32wordsize : 1;
				set_immediate(imm,(first_byte & 8) ? (isdata32 ? fetch_u32() : fetch_u16()) : fetch_u8());
				set_register(r,first_byte & 7);
			} break;
#if core_level > 0
		/* extended opcode escape */
		case 0x0F: {
			const uint8_t second_byte = *cip++;
			switch (second_byte) {
# if (core_level >= 3 && core_level <= 4) && !defined(no_umov)
				/* Intel 386/486 only, not on Pentium. Cyrix is said to treat this as a double NOOP (which ones? Cyrix 586? Or only pre-586?) */
				/* notice this opcode is reused later for SSE instructions MOVSS/MOVSD/MOVLPD/MOVPLS */
				COVER_4(0x10): /* UMOV */
					ins->opcode = MXOP_UMOV;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						const int which = (second_byte >> 1) & 1;
						struct minx86dec_argv *s = &ins->argv[which];
						struct minx86dec_argv *d = &ins->argv[which^1];
						d->size = s->size = second_byte & 1 ? data32wordsize : 1;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
# endif
# if defined(do_necv20) /* NEC V20/V30 */
				case 0x20: /* ADD4S. conflicts with 386 instruction mov reg,CRx */
					ins->opcode = MXOP_ADD4S;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = s->size = 2;
						d->memregsz = s->memregsz = 2;
						d->segment = MX86_SEG_ES;
						set_mem_ref_reg(s,MX86_REG_SI);
						set_mem_ref_reg(d,MX86_REG_DI);
					} break;
				case 0x22: /* SUB4S */
					ins->opcode = MXOP_SUB4S;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = s->size = 2;
						d->memregsz = s->memregsz = 2;
						d->segment = MX86_SEG_ES;
						set_mem_ref_reg(s,MX86_REG_SI);
						set_mem_ref_reg(d,MX86_REG_DI);
					} break;
				case 0x26: /* CMP4S */
					ins->opcode = MXOP_CMP4S;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = s->size = 2;
						d->memregsz = s->memregsz = 2;
						d->segment = MX86_SEG_ES;
						set_mem_ref_reg(s,MX86_REG_SI);
						set_mem_ref_reg(d,MX86_REG_DI);
					} break;
				case 0xFF: /* BRKEM */
					ins->opcode = MXOP_BRKEM;
					ins->argc = 1; {
						struct minx86dec_argv *r = &ins->argv[0];
						set_immediate(r,fetch_u8());
					} break;
				COVER_2(0x10): /* TEST1 r/m,CL */
				COVER_2(0x18): /* TEST1 r/m,imm */
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *imm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_TEST1; break;
						};
						d->size = (second_byte & 1) ? 2 : 1;
						decode_rm(mrm,d,0);
						imm->size = 1;
						if (second_byte & 8) set_immediate(imm,fetch_u8());
						else set_register(imm,MX86_REG_CL);
					} break;
				COVER_2(0x12): /* CLEAR1 r/m,CL conflicts with 386/486 UMOV and SSE MOVHPS */
				COVER_2(0x1A): /* CLEAR1 r/m,imm */
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *imm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_CLEAR1; break;
						};
						d->size = (second_byte & 1) ? 2 : 1;
						decode_rm(mrm,d,0);
						imm->size = 1;
						if (second_byte & 8) set_immediate(imm,fetch_u8());
						else set_register(imm,MX86_REG_CL);
					} break;
				COVER_2(0x14): /* SET1 r/m,CL */
				COVER_2(0x1C): /* SET1 r/m,imm */
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *imm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_SET1; break;
						};
						d->size = (second_byte & 1) ? 2 : 1;
						decode_rm(mrm,d,0);
						imm->size = 1;
						if (second_byte & 8) set_immediate(imm,fetch_u8());
						else set_register(imm,MX86_REG_CL);
					} break;
				COVER_2(0x16): /* NOT1 r/m,CL */
				COVER_2(0x1E): /* NOT1 r/m,imm */
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *imm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_NOT1; break;
						};
						d->size = (second_byte & 1) ? 2 : 1;
						decode_rm(mrm,d,0);
						imm->size = 1;
						if (second_byte & 8) set_immediate(imm,fetch_u8());
						else set_register(imm,MX86_REG_CL);
					} break;
				case 0x31: case 0x39:	/* INS */
				case 0x33: case 0x3B: { /* EXT */
					union x86_mrm mrm = fetch_modregrm();
					if (mrm.f.mod == 3) {
						struct minx86dec_argv *s = &ins->argv[2]; /* for display only! */
						struct minx86dec_argv *bit_start_pos = &ins->argv[0];
						struct minx86dec_argv *bit_length = &ins->argv[1];

						s->size = s->memregsz = 2;
						set_mem_ref_reg(s,MX86_REG_SI);

						ins->argc = 3;
						ins->opcode = (second_byte & 2) ? MXOP_EXT : MXOP_INS;
						bit_start_pos->size = bit_length->size = 1;
						bit_start_pos->memregsz = bit_length->memregsz = 1;
						if (second_byte & 8) set_immediate(bit_length,fetch_u8());
						else set_register(bit_length,mrm.f.reg);
						set_register(bit_start_pos,mrm.f.rm);
					}
					} break;
				case 0x28: /* ROL4 */
					ins->argc = 1; {
						struct minx86dec_argv *d = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_ROL4; break;
						};
						d->size = 1;
						decode_rm(mrm,d,0);
					} break;
				case 0x2A: /* ROR4 */
					ins->argc = 1; {
						struct minx86dec_argv *d = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_ROR4; break;
						};
						d->size = 1;
						decode_rm(mrm,d,0);
					} break;
# endif
# if core_level >= 2 /*------------- 286 or higher -----------------*/
				case 0x00: { /* LLDT */
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
					break; }
				case 0x01: { /* LGDT */
					/* yechh... */
					if (0) { /* this if() statement makes the else statement valid below even when not compiling else..if below us */
					}
# if core_level >= 6 /* these bizarre LGDT extensions pertain to Pentium II 686 class processors */
					else if (*cip == 0xC8) {
						ins->opcode = MXOP_MONITOR;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xC9) {
						ins->opcode = MXOP_MWAIT;
						ins->argc = 0;
						cip++;
					}
#  if sse_level >= 1 /* SSE instructions */
					else if (*cip == 0xD0) {
						/* extended control registers!?!? C'mon intel your damn standard has enough extensions of extensions! */
						ins->opcode = MXOP_XGETBV;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xD1) {
						/* extended control registers!?!? C'mon intel your damn standard has enough extensions of extensions! */
						ins->opcode = MXOP_XSETBV;
						ins->argc = 0;
						cip++;
					}
#  endif
					else if (*cip == 0xD8) {
						ins->opcode = MXOP_VMRUN;
						ins->argc = 1;
						ins->argv[0].size = 4;
						set_register(&ins->argv[0],MX86_REG_EAX);
						cip++;
					}
					else if (*cip == 0xD9) {
						ins->opcode = MXOP_VMMCALL;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xDA) {
						ins->opcode = MXOP_VMLOAD;
						ins->argc = 1;
						ins->argv[0].size = 4;
						set_register(&ins->argv[0],MX86_REG_EAX);
						cip++;
					}
					else if (*cip == 0xDB) {
						ins->opcode = MXOP_VMSAVE;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xDC) {
						ins->opcode = MXOP_STGI;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xDD) {
						ins->opcode = MXOP_CLGI;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xDE) {
						ins->opcode = MXOP_SKINIT;
						ins->argc = 1;
						ins->argv[0].size = 4;
						set_register(&ins->argv[0],MX86_REG_EAX);
						cip++;
					}
					else if (*cip == 0xDF) {
						ins->opcode = MXOP_INVLPGA;
						ins->argc = 2;
						ins->argv[0].size = ins->argv[1].size = 4;
						set_register(&ins->argv[0],MX86_REG_EAX);
						set_register(&ins->argv[1],MX86_REG_ECX);
						cip++;
					}
					else if (*cip == 0xF9) { /* FIXME: when did this get added to the instruction set? */
						ins->opcode = MXOP_RDTSCP;
						ins->argc = 0;
						cip++;
					}
					else if (*cip >= 0xC1 && *cip <= 0xC3) { /* FIXME: when did these VME instructions get added? */
						ins->opcode = MXOP_VMCALL + *cip - 0xC1;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xC4) { /* FIXME: when did VME extensions get added? */
						ins->opcode = MXOP_VMXOFF;
						ins->argc = 0;
						cip++;
					}
					else if (*cip == 0xF8) { /* we decode for completeness, but this is only valid in x86-64 mode */
						ins->opcode = MXOP_SWAPGS;
						ins->argc = 0;
						cip++;
					}
#endif
					else {
						union x86_mrm mrm = fetch_modregrm();
						switch (mrm.f.reg) {
							case 0: case 1:
							case 2: case 3:
								if (mrm.f.mod == 3) break; /* illegal encoding */
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
							case 7:
								if (mrm.f.mod == 3) break; /* illegal encoding */
								ins->opcode = MXOP_INVLPG;
								ins->argc = 1; {
									struct minx86dec_argv *m = &ins->argv[0];
									m->size = 4;
									decode_rm(mrm,m,isaddr32);
								} break;
						}
					} break; }
				case 0x02: /* LAR */
					ins->opcode = MXOP_LAR;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = s->size = data32wordsize;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
				case 0x03: /* LSL */
					ins->opcode = MXOP_LSL;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = s->size = data32wordsize;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
#  if core_level == 2 && !defined(everything)
				case 0x04: /* secret unknown opcode said to cause 286s to hang, LOADALL alias? */
					ins->opcode = MXOP_UNKNOWN_286_0F04;
					ins->argc = 0;
					break;
#  endif
#  if core_level == 2 || (defined(everything) && core_level > 2 && core_level <= 4)
				/* EWWWW this is a bit messy....
				   Once upon a time this was widely known as the LOADALL instruction for the 286.
				   386 and 486 systems used the invalid opcode exception to fake it, and then
				   it was forgotten, until re-used to become SYSCALL */
				case 0x05: /* LOADALL 286 */
					ins->opcode = MXOP_LOADALL_286;
					ins->argc = 0;
					break;
#  endif
				case 0x06: /* CLTS */
					ins->opcode = MXOP_CLTS;
					ins->argc = 0;
					break;
# endif
# if core_level >= 3 /* --------------------- 386 or higher ---------------------- */
#  if core_level == 3 || (defined(everything) && core_level == 4)
				/* this opcode suffers the same fate as the 286 version of LOADALL.
				   it was faked by 486 systems, forgotten around the Pentium era, and then
				   re-used for the SYSCALL/SYSENTER instruction. */
				case 0x07: /* LOADALL 386 */
					ins->opcode = MXOP_LOADALL_386;
					ins->argc = 0;
					break;
#  endif
#  if core_level == 3 && !defined(everything)
				case 0xA6:
					ins->opcode = MXOP_XBTS;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
				case 0xA7:
					ins->opcode = MXOP_IBTS;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[1];
						struct minx86dec_argv *s = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
#  endif
				case 0xAA:
					ins->opcode = MXOP_RSM;
					ins->argc = 0;
					break;
				COVER_2(0xBC):
					ins->opcode = MXOP_BSF + (second_byte & 1);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
				/* Jcc near */
				COVER_ROW(0x80):
					ins->opcode = MXOP_JO+(second_byte&0xF);
					ins->argc = 1; {
						struct minx86dec_argv *r = &ins->argv[0];
						uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
						if (isdata32)	set_immediate(r,curp + 4 + ((uint32_t)((int32_t)fetch_u32())));
						else		set_immediate(r,curp + 2 + ((uint32_t)((int16_t)fetch_u16())));
						r->size = addr32wordsize;
					} break;
				/* SETcc */
				COVER_ROW(0x90):
					ins->opcode = MXOP_SETO+(second_byte&0xF);
					ins->argc = 1; {
						struct minx86dec_argv *r = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						r->size = 1;
						decode_rm(mrm,r,isaddr32);
					} break;
				COVER_2(0xA0):
					ins->opcode = MXOP_PUSH + (second_byte & 1);
					ins->argc = 1; {
						struct minx86dec_argv *r = &ins->argv[0];
						set_segment_register(r,MX86_SEG_FS);
					} break;
				case 0xA3:
					ins->opcode = MXOP_BT;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
				COVER_2(0xA4): COVER_2(0xAC):
					ins->opcode = (second_byte & 8) ? MXOP_SHRD : MXOP_SHLD;
					ins->argc = 3; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						struct minx86dec_argv *imm = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						imm->size = 1;
						d->size = s->size = data32wordsize;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
						if (second_byte & 1) set_register(imm,MX86_REG_CL);
						else set_immediate(imm,fetch_u8());
					} break;
				COVER_2(0xA8):
					ins->opcode = MXOP_PUSH + (second_byte & 1);
					ins->argc = 1; {
						struct minx86dec_argv *r = &ins->argv[0];
						set_segment_register(r,MX86_SEG_GS);
					} break;
				case 0xAB:
					ins->opcode = MXOP_BTS;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
				case 0xB2:
					ins->opcode = MXOP_LSS;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* illegal */
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = data32wordsize;
						s->size = data32wordsize + 2;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
				case 0xB3:
					ins->opcode = MXOP_BTR;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
				case 0xB4:
					ins->opcode = MXOP_LFS;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* illegal */
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = data32wordsize;
						s->size = data32wordsize + 2;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
				case 0xB5:
					ins->opcode = MXOP_LGS;
					ins->argc = 2; {
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* illegal */
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->size = data32wordsize;
						s->size = data32wordsize + 2;
						set_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
					} break;
				COVER_2(0xB6):
					ins->opcode = MXOP_MOVZX;
					ins->argc = 2; {
						struct minx86dec_argv *re = &ins->argv[0];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						re->size = data32wordsize;
						rm->size = (second_byte & 1) + 1;
						set_register(re,mrm.f.reg);
						decode_rm(mrm,rm,isaddr32);
					} break;
				case 0xBA: {
					int m = -1;
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					switch (mrm.f.reg) {
						case 4: ins->opcode = MXOP_BT; m = 0; break;
						case 5: ins->opcode = MXOP_BTS; m = 0; break;
						case 6: ins->opcode = MXOP_BTR; m = 0; break;
						case 7: ins->opcode = MXOP_BTC; m = 0; break;
					}
					switch (m) {
						case 0:	ins->argc = 2;
							d->size = data32wordsize;
							decode_rm(mrm,d,isaddr32);
							set_immediate(s,fetch_u8());
							s->size = 1;
							break;
					};
					break; }
				case 0xBB:
					ins->opcode = MXOP_BTC;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
				COVER_2(0xBE):
					ins->opcode = MXOP_MOVSX;
					ins->argc = 2; {
						struct minx86dec_argv *re = &ins->argv[0];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						re->size = data32wordsize;
						rm->size = (second_byte & 1) + 1;
						set_register(re,mrm.f.reg);
						decode_rm(mrm,rm,isaddr32);
					} break;
				COVER_4(0x20):
					ins->opcode = MXOP_MOV;
					ins->argc = 2; {
						const int which = (second_byte >> 1) & 1;
						struct minx86dec_argv *ctrl = &ins->argv[which^1];
						struct minx86dec_argv *reg = &ins->argv[which];
						union x86_mrm mrm = fetch_modregrm();
						ctrl->size = reg->size = 4; /* x86: always 4 bytes */
						set_register(reg,mrm.f.rm);
						if (second_byte & 1)	set_debug_register(ctrl,mrm.f.reg);
						else			set_control_register(ctrl,mrm.f.reg);
					} break;
				COVER_4(0x24):
					if (second_byte & 1) {
					}
					else {
						ins->opcode = MXOP_MOV;
						ins->argc = 2; {
							const int which = (second_byte >> 1) & 1;
							struct minx86dec_argv *ctrl = &ins->argv[which^1];
							struct minx86dec_argv *reg = &ins->argv[which];
							union x86_mrm mrm = fetch_modregrm();
							ctrl->size = reg->size = 4; /* x86: always 4 bytes */
							set_register(reg,mrm.f.rm);
							set_test_register(ctrl,mrm.f.reg);
						}
					} break;
# endif
# if core_level == 4
				COVER_2(0xA6): /* the original CMPXCHG, which conflicts with 386 IBTS, and VIA padlock extensions */
					ins->opcode = MXOP_CMPXCHG;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = (second_byte & 1) ? data32wordsize : 1;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
# endif
# if core_level >= 4 /* --------------------- 486 or higher ---------------------- */
				COVER_2(0xB0):
					ins->opcode = MXOP_CMPXCHG;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = (second_byte & 1) ? data32wordsize : 1;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
				COVER_8(0xC8):
					ins->opcode = MXOP_BSWAP;
					ins->argc = 1; {
						struct minx86dec_argv *r = &ins->argv[0];
						set_register(r,second_byte & 7);
						r->size = data32wordsize;
					}
					break;
				case 0x08:
					ins->opcode = MXOP_INVD;
					ins->argc = 0;
					break;
				case 0x09:
					ins->opcode = MXOP_WBINVD;
					ins->argc = 0;
					break;
				COVER_2(0xC0):
					ins->opcode = MXOP_XADD;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = (second_byte & 1) ? data32wordsize : 1;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
				case 0xA2: /* early 486's did not have this, later ones did, so it is placed here */
					ins->opcode = MXOP_CPUID;
					ins->argc = 0;
					break;
# endif
# if core_level >= 5 /* --------------------- pentium or higher ------------------ */
				case 0x30:
					ins->opcode = MXOP_WRMSR;
					ins->argc = 0;
					break;
				case 0x31:	/* RDTSC */
				case 0x32:	/* RDMSR */
#  if defined(pentiumpro) || pentium >= 2 /* Pentium Pro or higher: RDPMC */
				case 0x33:
#  endif
					ins->opcode = MXOP_RDTSC + second_byte - 0x31;
					ins->argc = 0;
					break;
				case 0xC7: {
					struct minx86dec_argv *a = &ins->argv[0];
					union x86_mrm mrm = fetch_modregrm();
					switch (mrm.f.reg) {
						case 1:
							ins->opcode = MXOP_CMPXCHG8B;
							ins->argc = 1;
							a->size = 8;
							decode_rm(mrm,a,isaddr32);
							break;
						case 6: /* good lord, Intel, overloading one instruction here?!? */
							if (ins->rep == MX86_REPNE) ins->opcode = MXOP_VMXON;
							else ins->opcode = dataprefix32 ? MXOP_VMCLEAR : MXOP_VMPTRLD;
							ins->argc = 1;
							a->size = 8;
							decode_rm(mrm,a,isaddr32);
							break;
						case 7:
							ins->opcode = MXOP_VMPTRST;
							ins->argc = 1;
							a->size = 8;
							decode_rm(mrm,a,isaddr32);
							break;
					};
					} break;
#  if defined(pentiumpro) || pentium >= 2 /* Pentium Pro or higher */
				COVER_ROW(0x40): /* CMOVcc */
					ins->opcode = MXOP_CMOVO + second_byte - 0x40;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[1];
						struct minx86dec_argv *s = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = data32wordsize;
						set_register(s,mrm.f.reg);
						decode_rm(mrm,d,isaddr32);
					} break;
				case 0x1F: {
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					switch (mrm.f.reg) {
						case 0:
							ins->opcode = MXOP_NOP;
							ins->argc = 1;
							d->size = data32wordsize;
							decode_rm(mrm,d,isaddr32);
							break;
					}
					} break;
				case 0x6E:
				case 0x7E:
					if (ins->rep >= MX86_REPE) {
						if (second_byte == 0x7E && ins->rep == MX86_REPNE) {
							ins->opcode = MXOP_MOVQ;
							ins->argc = 2; {
								struct minx86dec_argv *re = &ins->argv[0];
								struct minx86dec_argv *rm = &ins->argv[1];
								union x86_mrm mrm = fetch_modregrm();
								rm->size = re->size = 16;
								set_sse_register(re,mrm.f.reg);
								decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
							}
						}
					}
					else {
						ins->opcode = MXOP_MOVD;
						ins->argc = 2; {
							const unsigned int which = (second_byte >> 4) & 1;
							struct minx86dec_argv *re = &ins->argv[which];
							struct minx86dec_argv *rm = &ins->argv[which^1];
							union x86_mrm mrm = fetch_modregrm();
							rm->size = 4;
							if (dataprefix32) {
								re->size = 16;
								set_sse_register(re,mrm.f.reg);
								decode_rm(mrm,rm,isaddr32);
							}
							else {
								re->size = 8;
								set_mmx_register(re,mrm.f.reg);
								decode_rm(mrm,rm,isaddr32);
							}
						}
					} break;
				case 0x05: /* FIXME: when did this appear? Said to appear on AMD K6-2 first, but I can't verify that */
					ins->opcode = MXOP_SYSCALL;
					ins->argc = 0;
					break;
				case 0x07: /* FIXME: when did this appear? Said to appear on AMD K6-2 first, but I can't verify that */
					ins->opcode = MXOP_SYSRET;
					ins->argc = 0;
					break;
#  endif
#  if cyrix_level == 6 && mmx == 1 /* cyrix MMX extensions, apparently only on the 6x86 */
				   /* even then you had to enable them through a control register */
				case 0x51: /* PADDSIW conflicts with SSE SQRTPS */
					ins->opcode = MXOP_PADDSIW;
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x50: /* PAVEB conflicts with MOVMSKPS */
					ins->opcode = MXOP_PAVEB;
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x52: /* PMAGW conflicts with RSQRTPS */
					ins->opcode = MXOP_PMAGW;
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x54: /* PDISTIB conflicts with ANDPS */
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* cannot be a register source reference */
						ins->opcode = MXOP_PDISTIB;
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x55: /* PDISTIB conflicts with ANDNPS */
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_PSUBSIW;
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x58: /* PMVZB conflicts with ADDPS */
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* cannot be a register source reference */
						ins->opcode = MXOP_PMVZB;
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x59: /* PMULHRW conflicts with MULPS */
					ins->opcode = MXOP_PMULHRWC;
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x5A: /* PMVNZB conflicts with CVTPS2PD */
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* cannot be a register source reference */
						ins->opcode = MXOP_PMVNZB;
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x5B: /* PMVLZB conflicts with CVTDQ2PS */
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* cannot be a register source reference */
						ins->opcode = MXOP_PMVLZB;
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x5C: /* PMVNZB conflicts with SUBPS */
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* cannot be a register source reference */
						ins->opcode = MXOP_PMVGEZB;
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x5D: /* PMULHRIW conflicts with MINPS */
					ins->opcode = MXOP_PMULHRIW;
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
				case 0x5E: /* PMACHRIW conflicts with DIVPS */
					ins->argc = 3; {
						struct minx86dec_argv *dst = &ins->argv[0];
						struct minx86dec_argv *s1 = &ins->argv[1];
						struct minx86dec_argv *s2 = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						if (mrm.f.mod == 3) break; /* cannot be a register source reference */
						ins->opcode = MXOP_PMACHRIW;
						dst->size = s1->size = s2->size = 8;
						set_mmx_register(s1,mrm.f.reg);
						set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
						decode_rm_ex(mrm,s2,isaddr32,MX86_RT_MMX);
					} break;
#  endif
#  if amd_3dnow >= 1 || defined(everything)
				case 0x0D: {
					struct minx86dec_argv *rm = &ins->argv[0];
					union x86_mrm mrm = fetch_modregrm();
					ins->argc = 1;
					if (mrm.f.mod == 3) break; /* register-only version is illegal */
					rm->size = 4;
					decode_rm(mrm,rm,isaddr32);
					switch (mrm.f.reg) {
						case 0: ins->opcode = MXOP_PREFETCH; break;
						case 1: ins->opcode = MXOP_PREFETCHW; break;
					}
					break; }
				case 0x0E:
					ins->opcode = MXOP_FEMMS;
					ins->argc = 0;
					break;
				case 0x0F: {
					/* AMD 3DNow! instructions are the only ones I know of that require you to decode mod/reg/rm 
					   FIRST before even beginning to know what instruction it is... */
					struct minx86dec_argv *reg = &ins->argv[0];
					struct minx86dec_argv *rm = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					ins->argc = 2;
					rm->size = reg->size = 8;
					decode_rm_ex(mrm,rm,isaddr32,MX86_RT_MMX);
					set_mmx_register(reg,mrm.f.reg);
					uint8_t suffix = *cip++;

					switch (suffix) {
						case 0xBF: ins->opcode = MXOP_PAVGUSB; break;
						case 0x1D: ins->opcode = MXOP_PF2ID; break;
						case 0xAE: ins->opcode = MXOP_PFACC; break;
						case 0x9E: ins->opcode = MXOP_PFADD; break;
						case 0xB0: ins->opcode = MXOP_PFCMPEQ; break;
						case 0x90: ins->opcode = MXOP_PFCMPGE; break;
						case 0xA0: ins->opcode = MXOP_PFCMPGT; break;
						case 0xA4: ins->opcode = MXOP_PFMAX; break;
						case 0x94: ins->opcode = MXOP_PFMIN; break;
						case 0xB4: ins->opcode = MXOP_PFMUL; break;
						case 0x96: ins->opcode = MXOP_PFRCP; break;
						case 0xA6: ins->opcode = MXOP_PFRCPIT1; break;
						case 0xB6: ins->opcode = MXOP_PFRCPIT2; break;
						case 0xA7: ins->opcode = MXOP_PFRSQIT1; break;
						case 0x97: ins->opcode = MXOP_PFRSQRT; break;
						case 0x9A: ins->opcode = MXOP_PFSUB; break;
						case 0xAA: ins->opcode = MXOP_PFSUBR; break;
						case 0x0D: ins->opcode = MXOP_PI2FD; break;
						case 0xB7: ins->opcode = MXOP_PMULHRWA; break;
#   if amd_3dnow >= 2 || defined(everything) /* 3dnow+ aka enhanced, etc. whatever you call it */
						case 0x1C: ins->opcode = MXOP_PF2IW; break;
						case 0x0C: ins->opcode = MXOP_PI2FW; break;
						case 0xBB: ins->opcode = MXOP_PSWAPD; break;
						case 0x8A: ins->opcode = MXOP_PFNACC; break;
						case 0x8E: ins->opcode = MXOP_PFPNACC; break;
#    if amd_3dnow >= 3 || defined(everything) /* Geode */
						case 0x86: ins->opcode = MXOP_PFRCPV; break;
						case 0x87: ins->opcode = MXOP_PFRSQRTV; break;
#    endif
#   endif
					} break; }
#  endif
#  if core_level >= 6 /* Pentium II or higher */
				case 0x34:
					ins->opcode = MXOP_SYSENTER;
					ins->argc = 0;
					break;
				case 0x35:
					ins->opcode = MXOP_SYSEXIT;
					ins->argc = 0;
					break;
#  endif
#  if cyrix_level >= 134 || defined(everything) /* VIA Nehemiah Padlock extensions */
				case 0xA6:
					if (ins->rep == MX86_REPNE && *cip == 0xC0) {
						ins->opcode = MXOP_MONTMUL;
						ins->argc = 0;
						cip++;
					}
					else if (ins->rep == MX86_REPNE && *cip == 0xC8) {
						ins->opcode = MXOP_XSHA1;
						ins->argc = 0;
						cip++;
					}
					else if (ins->rep == MX86_REPNE && *cip == 0xD0) {
						ins->opcode = MXOP_XSHA256;
						ins->argc = 0;
						cip++;
					}
					break;
				case 0xA7:
					if (ins->rep != MX86_REPE && *cip == 0xC0) {
						ins->opcode = MXOP_XSTORE;
						ins->argc = 0;
						cip++;
					}
					else if (ins->rep == MX86_REPNE && *cip == 0xC8) {
						ins->opcode = MXOP_XCRYPTECB;
						ins->argc = 0;
						cip++;
					}
					else if (ins->rep == MX86_REPNE && *cip == 0xD0) {
						ins->opcode = MXOP_XCRYPTCBC;
						ins->argc = 0;
						cip++;
					}
					else if (ins->rep == MX86_REPNE && *cip == 0xD8) {
						ins->opcode = MXOP_XCRYPTCTR;
						ins->argc = 0;
						cip++;
					}
					else if (ins->rep == MX86_REPNE && *cip == 0xE0) {
						ins->opcode = MXOP_XCRYPTCFB;
						ins->argc = 0;
						cip++;
					}
					else if (ins->rep == MX86_REPNE && *cip == 0xE8) {
						ins->opcode = MXOP_XCRYPTOFB;
						ins->argc = 0;
						cip++;
					}
					break;
#  endif
#  if sse_level >= 1 /* SSE instructions */
				case 0xEF:
					ins->opcode = MXOP_PXOR;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = dataprefix32 ? 16 : 8; /* 128 bit = 16 bytes */
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;
				case 0x0B:
					ins->opcode = MXOP_UD2;
					ins->argc = 0;
					break;
				COVER_2(0x52):
					if (ins->rep == MX86_REPNE) {
						if (second_byte & 1) ins->opcode = MXOP_RCPSS;
						else ins->opcode = MXOP_RSQRTSS;
					}
					else if (second_byte & 1) ins->opcode = MXOP_RCPPS;
					else ins->opcode = MXOP_RSQRTPS;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				COVER_2(0x14): /* MXOP_UNPCKLPS, MXOP_UNPCKLPD, MXOP_UNPCKHPS, MXOP_UNPCKHPD */
					ins->opcode = MXOP_UNPCKLPS + ((second_byte & 1) << 1) + (dataprefix32 & 1);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				case 0x77:
					ins->opcode = MXOP_EMMS;
					ins->argc = 0;
					break;
				case 0xF0:
					ins->opcode = MXOP_LDDQU;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				COVER_2(0x7C):
					if (dataprefix32) {
						ins->argc = 2;
						ins->opcode = (second_byte & 1) ? MXOP_HSUBPD : MXOP_HADDPD;
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					else if (ins->rep == MX86_REPE) {
						ins->argc = 2;
						ins->opcode = (second_byte & 1) ? MXOP_HSUBPS : MXOP_HADDPS;
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					break;
				COVER_2(0x54):
					ins->opcode = dataprefix32 + MXOP_ANDPS + ((second_byte & 1) << 1);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				case 0x58:
					ins->opcode = (ins->rep >= MX86_REPE ? (2 + ins->rep - MX86_REPE) : dataprefix32) + MXOP_ADDPS;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				case 0x5C:
					if (dataprefix32) ins->opcode = MXOP_SUBPD;
					else ins->opcode = MXOP_SUBPS + ins->rep;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				case 0xC2:
					ins->opcode = (ins->rep >= MX86_REPE) ? (MXOP_CMPSD + ins->rep - MX86_REPE) : (MXOP_CMPPS + (dataprefix32 ? 1 : 0));
					ins->argc = 3; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						struct minx86dec_argv *i = &ins->argv[2];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
						set_immediate(i,fetch_u8());
					} break;
				case 0x2A:
					if (ins->rep >= MX86_REPE) {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = ins->rep == MX86_REPNE ? MXOP_CVTSI2SS : MXOP_CVTSI2SD;
						ins->argc = 2;
						d->size = 16;
						set_sse_register(d,mrm.f.reg);
						s->size = 4; /* 128 bit = 16 bytes */
						decode_rm(mrm,s,isaddr32);
					}
					else {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_CVTPI2PS + (dataprefix32 & 1);
						ins->argc = 2;
						d->size = 16;
						set_sse_register(d,mrm.f.reg);
						s->size = 8; /* 128 bit = 16 bytes */
						decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_MMX);
					}
					break;
				case 0x2C: {
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					if (ins->rep >= MX86_REPE)
						ins->opcode = MXOP_CVTTSD2SI + ins->rep - MX86_REPE;
					else
						ins->opcode = MXOP_CVTTPS2PI + (dataprefix32 & 1);
					ins->argc = 2;
					if (ins->rep >= MX86_REPE) {
						d->size = 4;
						d->regtype = MX86_RT_REG;
					}
					else {
						d->size = 8;
						d->regtype = MX86_RT_MMX;
					}
					s->size = 16; /* 128 bit = 16 bytes */
					d->reg = mrm.f.reg;
					s->regtype = MX86_RT_SSE;
					decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					break; }
				case 0x2D:
					if (ins->rep >= MX86_REPE) {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = ins->rep == MX86_REPNE ? MXOP_CVTSS2SI : MXOP_CVTSD2SI;
						ins->argc = 2;
						d->size = 4;
						set_register(d,mrm.f.reg);
						s->size = 16; /* 128 bit = 16 bytes */
						s->regtype = MX86_RT_SSE;
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					else {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_CVTPS2PI + (dataprefix32 & 1);
						ins->argc = 2;
						d->size = 8;
						set_mmx_register(d,mrm.f.reg);
						s->size = 16; /* 128 bit = 16 bytes */
						s->regtype = MX86_RT_SSE;
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					break;
				case 0x2E:
					ins->opcode = MXOP_UCOMISS + (dataprefix32 ? 1 : 0);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						d->reg = mrm.f.reg;
						decode_rm_ex(mrm,s,isaddr32,d->regtype = MX86_RT_SSE);
					} break;
				case 0x2F:
					ins->opcode = MXOP_COMISS + (dataprefix32 ? 1 : 0);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						d->size = s->size = 16; /* 128 bit = 16 bytes */
						d->reg = mrm.f.reg;
						decode_rm_ex(mrm,s,isaddr32,d->regtype = MX86_RT_SSE);
					} break;
				case 0x5A:
					if (ins->rep >= MX86_REPE) {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = ins->rep == MX86_REPNE ? MXOP_CVTSS2SD : MXOP_CVTSD2SS;
						ins->argc = 2;
						d->size = 16;
						set_sse_register(d,mrm.f.reg);
						s->size = 16; /* 128 bit = 16 bytes */
						decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_SSE);
					}
					else {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_CVTPS2PD + (dataprefix32 & 1);
						ins->argc = 2;
						d->size = 16;
						set_sse_register(d,mrm.f.reg);
						s->size = 16; /* 128 bit = 16 bytes */
						decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_SSE);
					}
					break;
				case 0x5B: {
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					if (ins->rep == MX86_REPNE) {
						ins->opcode = MXOP_CVTTPS2DQ;
					}
					else if (ins->rep == MX86_REPE) {
						/* not defined */
						break;
					}
					else {
						ins->opcode = MXOP_CVTDQ2PS + (dataprefix32 & 1);
					}
					ins->argc = 2;
					d->size = 16;
					set_sse_register(d,mrm.f.reg);
					s->size = 16; /* 128 bit = 16 bytes */
					decode_rm_ex(mrm,s,isaddr32,s->regtype = MX86_RT_SSE);
					break; }
				case 0xD0: {
#   define PAIR(d,r)  ((d) + ((r) << 2))
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					unsigned char t = dataprefix32 + (ins->rep << 2);
					unsigned char m = 0;
					switch (t) {
						case PAIR(1,MX86_REP_NONE):
							ins->opcode = MXOP_ADDSUBPD;
							m = 1;
							break;
						case PAIR(0,MX86_REPE):
							ins->opcode = MXOP_ADDSUBPS;
							m = 1;
							break;
					};
					switch (m) {
						case 1: { /* ADDSUBPD/S */
							union x86_mrm mrm = fetch_modregrm();
							ins->argc = 2;
							d->size = s->size = 16; /* 128 bit = 16 bytes */
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							} break;
					}
#   undef PAIR
					break; }
#  endif
#  if sse_level >= 2 /* SSE2 */
				case 0x37:
					ins->opcode = MXOP_GETSEC;
					ins->argc = 0;
					break;
				COVER_2(0xDC): {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PADDUSB + (second_byte & 1);
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				COVER_2(0xEC): {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PADDSB + (second_byte & 1);
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD4: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PADDQ;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xDB: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PAND;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xDF: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PANDN;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xE0: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PAVGB;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xE3: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PAVGW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xE4: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PMULHUW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD5: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PMULLW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xE5: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PMULHW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD7: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PMOVMSKB;
					ins->argc = 2;
					d->size = 4;
					s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_register(d,mrm.f.reg);
					else set_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xF4: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PMULUDQ;
					ins->argc = 2;
					d->size = 4;
					s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_register(d,mrm.f.reg);
					else set_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0x71: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					switch (mrm.f.reg) {
						case 2:	ins->opcode = MXOP_PSRLW;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;

						case 4:	ins->opcode = MXOP_PSRAW;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;

						case 6:	ins->opcode = MXOP_PSLLW;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;
					}
				} break;

				case 0x72: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					switch (mrm.f.reg) {
						case 2:	ins->opcode = MXOP_PSRLD;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;

						case 4:	ins->opcode = MXOP_PSRAD;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;

						case 6:	ins->opcode = MXOP_PSLLD;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;
					}
				} break;

				case 0x73: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					switch (mrm.f.reg) {
						case 2:	ins->opcode = MXOP_PSRLQ;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;

						case 3:	if (dataprefix32) {
								ins->opcode = MXOP_PSRLDQ;
								ins->argc = 2;
								d->size = 16;
								decode_rm_ex(mrm,d,isaddr32,MX86_RT_SSE);
								s->size = 1;
								set_immediate(s,fetch_u8());
							}
							break;

						case 6:	ins->opcode = MXOP_PSLLQ;
							ins->argc = 2;
							d->size = dataprefix32 ? 16 : 8;
							decode_rm_ex(mrm,d,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							s->size = 1;
							set_immediate(s,fetch_u8());
							break;

						case 7:	if (dataprefix32) {
								ins->opcode = MXOP_PSLLDQ;
								ins->argc = 2;
								d->size = 16;
								decode_rm_ex(mrm,d,isaddr32,MX86_RT_SSE);
								s->size = 1;
								set_immediate(s,fetch_u8());
							}
							break;
					}
				} break;

				case 0xE1: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSRAW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xE2: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSRAD;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xF1: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSLLW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xF2: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSLLD;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xF3: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSLLQ;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD1: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSRLW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD2: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSRLD;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD3: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSRLQ;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD8: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBUSB;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xD9: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBUSW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0x70:
					if (ins->rep >= MX86_REPE) {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						struct minx86dec_argv *i = &ins->argv[2];
						ins->opcode = MXOP_PSHUFLW + ins->rep - MX86_REPE;
						ins->argc = 3;
						d->size = s->size = 16;
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
						i->size = 1;
						set_immediate(i,fetch_u8());
					}
					else {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						struct minx86dec_argv *i = &ins->argv[2];
						ins->opcode = MXOP_PSHUFW;
						ins->argc = 3;
						d->size = s->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						i->size = 1;
						set_immediate(i,fetch_u8());
					}
					break;

				case 0xE8: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBSB;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xE9: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBSW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xF6: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSADBW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xF8: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBB;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xF9: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBW;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xFA: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBD;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0xFB: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PSUBQ;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				COVER_2(0xFC): case 0xFE: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PADDB + (second_byte & 3);
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				COVER_2(0x64): case 0x66: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PCMPGTB + (second_byte & 3);
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				case 0x67: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PACKUSWB;
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				COVER_2(0x74): case 0x76: {
					union x86_mrm mrm = fetch_modregrm();
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					ins->opcode = MXOP_PCMPEQB + (second_byte & 3);
					ins->argc = 2;
					d->size = s->size = dataprefix32 ? 16 : 8;
					if (dataprefix32) set_sse_register(d,mrm.f.reg);
					else set_mmx_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				} break;

				COVER_2(0x78):
					if (ins->rep == MX86_REPE) {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->argc = (second_byte & 1) ? 2 : 4;
						ins->opcode = MXOP_INSERTQ;
						d->size = s->size = 16;
						ins->argv[2].size = ins->argv[3].size = 1;
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
						set_sse_register(d,mrm.f.reg);
						if ((second_byte & 1) == 0) {
							set_immediate(&ins->argv[2],fetch_u8());
							set_immediate(&ins->argv[3],fetch_u8());
						}
					}
					else if (dataprefix32) {
						struct minx86dec_argv *d = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();

						if (second_byte & 1) {
							ins->opcode = MXOP_EXTRQ;
							ins->argc = 2;
							ins->argv[1].size = d->size = 16;
							decode_rm_ex(mrm,&ins->argv[1],isaddr32,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg);
						}
						else {
							switch (mrm.f.reg) {
								case 0: {
									d->size = 16;
									ins->opcode = MXOP_EXTRQ;
									ins->argc = 3;
									ins->argv[1].size = ins->argv[2].size = 1;
									decode_rm_ex(mrm,d,isaddr32,MX86_RT_SSE);
									set_immediate(&ins->argv[1],fetch_u8());
									set_immediate(&ins->argv[2],fetch_u8());
								}
								break;
							}
						}
					}
					else {
						ins->opcode = MXOP_VMREAD + (second_byte & 1);
						ins->argc = 2; {
							const int which = second_byte & 1;
							struct minx86dec_argv *s = &ins->argv[which  ];
							struct minx86dec_argv *d = &ins->argv[which^1];
							union x86_mrm mrm = fetch_modregrm();
							d->size = s->size = 4;
							set_register(d,mrm.f.reg);
							decode_rm(mrm,s,isaddr32);
						}
					} break;

				COVER_2(0x10): {
					const unsigned int which = second_byte & 1;
					struct minx86dec_argv *re = &ins->argv[which];
					struct minx86dec_argv *rm = &ins->argv[which^1];
					union x86_mrm mrm = fetch_modregrm();
					re->size = rm->size = 16;
					set_sse_register(re,mrm.f.reg);
					decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					ins->argc = 2;

					if (ins->rep >= MX86_REPE)
						ins->opcode = ins->rep == MX86_REPNE ? MXOP_MOVSS : MXOP_MOVSD;
					else
						ins->opcode = dataprefix32 ? MXOP_MOVUPD : MXOP_MOVUPS;
					} break;
				COVER_2(0x12): {
					const unsigned int which = second_byte & 1;
					struct minx86dec_argv *d = &ins->argv[which];
					struct minx86dec_argv *s = &ins->argv[which^1];
					union x86_mrm mrm = fetch_modregrm();
					if (ins->rep == MX86_REPNE)
						ins->opcode = MXOP_MOVSLDUP;
					else if (ins->rep == MX86_REPE)
						ins->opcode = MXOP_MOVDDUP;
					else
						ins->opcode = dataprefix32 ? MXOP_MOVLPD : MXOP_MOVLPS;
					ins->argc = 2;
					d->size = 16;
					s->size = 8;
					set_sse_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					/* Hm..... so the reg-reg version of MXOP_MOVLPS is MXOP_MOVHLPS? */
					if (ins->opcode == MXOP_MOVLPS && s->regtype != MX86_RT_NONE)
						ins->opcode = MXOP_MOVHLPS;
					} break;
				case 0x18: {
					struct minx86dec_argv *d = &ins->argv[0];
					union x86_mrm mrm = fetch_modregrm();
					d->size = 4;
					decode_rm(mrm,d,isaddr32);
					switch (mrm.f.reg) {
						case 0:	ins->opcode = MXOP_PREFETCHNTA; ins->argc = 1;
							break;
						case 1: case 2: case 3:
							ins->opcode = MXOP_PREFETCHT0 + mrm.f.reg - 1; ins->argc = 1;
							break;
					} } break;
				COVER_2(0x16): {
					const unsigned int which = second_byte & 1;
					struct minx86dec_argv *d = &ins->argv[which];
					struct minx86dec_argv *s = &ins->argv[which^1];
					union x86_mrm mrm = fetch_modregrm();
					ins->opcode = (ins->rep == MX86_REPNE) ? MXOP_MOVSHDUP : (dataprefix32 ? MXOP_MOVHPD : MXOP_MOVHPS);
					ins->argc = 2;
					d->size = 16;
					s->size = 8;
					set_sse_register(d,mrm.f.reg);
					decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					/* Hm..... so the reg-reg version of MXOP_MOVLPS is MXOP_MOVHLPS? */
					if (ins->opcode == MXOP_MOVHPS && s->regtype != MX86_RT_NONE)
						ins->opcode = MXOP_MOVLHPS;
					} break;
				COVER_2(0x28):
					ins->opcode = MXOP_MOVAPS - (dataprefix32 & 1);
					ins->argc = 2; {
						const unsigned int which = second_byte & 1;
						struct minx86dec_argv *re = &ins->argv[which];
						struct minx86dec_argv *rm = &ins->argv[which^1];
						union x86_mrm mrm = fetch_modregrm();
						re->size = rm->size = 16;
						set_sse_register(re,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					} break;
				case 0x50:
					ins->opcode = MXOP_MOVMSKPS - (dataprefix32 & 1);
					ins->argc = 2; {
						struct minx86dec_argv *reg = &ins->argv[0];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						reg->size = 4;
						rm->size = 16;
						set_register(reg,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
						/* FIXME: Intel docs imply that MXOP_MOVMSKPD applies to the r/m = reg form. what else is here? */
					} break;
				case 0x51:
					if (dataprefix32) {
						ins->opcode = MXOP_SQRTPD;
						ins->argc = 2; {
							struct minx86dec_argv *reg = &ins->argv[0];
							struct minx86dec_argv *rm = &ins->argv[1];
							union x86_mrm mrm = fetch_modregrm();
							reg->size = rm->size = 16;
							set_sse_register(reg,mrm.f.reg);
							decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);

						}
					}
					else {
						ins->opcode = MXOP_SQRTPS + ins->rep;
						ins->argc = 2; {
							struct minx86dec_argv *reg = &ins->argv[0];
							struct minx86dec_argv *rm = &ins->argv[1];
							union x86_mrm mrm = fetch_modregrm();
							reg->size = rm->size = 16;
							set_sse_register(reg,mrm.f.reg);
							decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);

						}
					}
					break;
				case 0x56:
					ins->opcode = MXOP_ORPS + (dataprefix32 & 1);
					ins->argc = 2; {
						struct minx86dec_argv *reg = &ins->argv[0];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						reg->size = rm->size = 16;
						set_sse_register(reg,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					} break;
				case 0x57:
					ins->opcode = MXOP_XORPS + (dataprefix32 & 1);
					ins->argc = 2; {
						struct minx86dec_argv *reg = &ins->argv[0];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						reg->size = rm->size = 16;
						set_sse_register(reg,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					} break;
				case 0x59:
					ins->argc = 2;
					ins->opcode = (ins->rep >= MX86_REPE) ?
						(MXOP_MULSD + ins->rep - MX86_REPE) :
						(MXOP_MULPS + (dataprefix32 & 1));
					{
						struct minx86dec_argv *reg = &ins->argv[0];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						reg->size = rm->size = 16;
						set_sse_register(reg,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					} break;
				case 0x5D:
					ins->opcode = (ins->rep >= MX86_REPE) ? (ins->rep + MXOP_MINSD - MX86_REPE) : (MXOP_MINPS - dataprefix32);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = 16;
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				case 0x5E: {
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					if (ins->rep >= MX86_REPE)
						ins->opcode = MXOP_DIVSD + ins->rep - MX86_REPE;
					else if (dataprefix32)
						ins->opcode = MXOP_DIVPD;
					else
						ins->opcode = MXOP_DIVPS;

					ins->argc = 2;
					d->size = 16;
					set_sse_register(d,mrm.f.reg);
					s->size = 16; /* 128 bit = 16 bytes */
					decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;
				case 0x5F:
					ins->opcode = (ins->rep >= MX86_REPE) ? (ins->rep + MXOP_MAXSD - MX86_REPE) : (MXOP_MAXPS - dataprefix32);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = 16;
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					} break;

				case 0x60:
					ins->opcode = MXOP_PUNPCKLBW;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;

				case 0x61:
					ins->opcode = MXOP_PUNPCKLWD;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;

				case 0x62:
					ins->opcode = MXOP_PUNPCKLDQ;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;

				case 0x63:
				case 0x6B:
					ins->opcode = MXOP_PACKSSWB + ((second_byte >> 3)&1);
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;

				case 0x6F:
				case 0x7F:
					if (ins->rep == MX86_REPE) {
					}
					else {
						const unsigned int which = (second_byte >> 4) & 1;
						struct minx86dec_argv *d = &ins->argv[which];
						struct minx86dec_argv *s = &ins->argv[which^1];
						union x86_mrm mrm = fetch_modregrm();

						ins->argc = 2;
						ins->opcode = (ins->rep >= MX86_REPE) ? MXOP_MOVDQU :
							(dataprefix32 ? MXOP_MOVDQA : MXOP_MOVQ);

						if (dataprefix32 || ins->rep >= MX86_REPE) {
							s->size = d->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
						}
						else {
							s->size = d->size = 8;
							set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_MMX);
						}
					}
					break;

				case 0x68:
					ins->opcode = MXOP_PUNPCKHBW;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;

				case 0x69:
					ins->opcode = MXOP_PUNPCKHWD;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;

				case 0x6A:
					ins->opcode = MXOP_PUNPCKHDQ;
					ins->argc = 2; {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						s->size = d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					} break;

				case 0x6C:
					if (dataprefix32) {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_PUNPCKLQDQ;
						ins->argc = 2;
						s->size = d->size = 16;
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					break;

				case 0x6D:
					if (dataprefix32) {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_PUNPCKHQDQ;
						ins->argc = 2;
						s->size = d->size = 16;
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					break;

				case 0xB8:
					ins->opcode = MXOP_POPCNT;
					ins->argc = 2; {
						struct minx86dec_argv *re = &ins->argv[0];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						re->size = rm->size = data32wordsize;
						set_register(re,mrm.f.reg);
						decode_rm(mrm,rm,isaddr32);
					} break;
				case 0xC3:
					if (dataprefix32) {
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[0];
						struct minx86dec_argv *reg = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_MOVNTI;
						ins->argc = 2;
						rm->size = reg->size = 4;
						set_register(reg,mrm.f.reg);
						decode_rm(mrm,rm,isaddr32);
					}
					break;
				case 0xC4: {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						struct minx86dec_argv *i = &ins->argv[2];
						ins->opcode = MXOP_PINSRW;
						ins->argc = 3;
						s->size = 4;
						d->size = dataprefix32 ? 16 : 8;
						if (dataprefix32) set_sse_register(d,mrm.f.reg);
						else set_mmx_register(d,mrm.f.reg);
						decode_rm(mrm,s,isaddr32);
						i->size = 1;
						set_immediate(i,fetch_u8());
					}
					break;
				case 0xC5: {
						union x86_mrm mrm = fetch_modregrm();
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						struct minx86dec_argv *i = &ins->argv[2];
						ins->opcode = MXOP_PEXTRW;
						ins->argc = 3;
						s->size = dataprefix32 ? 16 : 8;
						d->size = 4;
						set_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						i->size = 1;
						set_immediate(i,fetch_u8());
					}
					break;
				case 0xC6:
					if (ins->rep >= MX86_REPE) {
					}
					else {
						struct minx86dec_argv *reg = &ins->argv[0];
						struct minx86dec_argv *imm = &ins->argv[2];
						struct minx86dec_argv *rm = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_SHUFPS + dataprefix32;
						ins->argc = 3;
						imm->size = 1;
						rm->size = reg->size = 16;
						set_immediate(imm,fetch_u8());
						set_sse_register(reg,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					}
					break;
				case 0x2B:
					if (ins->rep >= MX86_REPE) {
						struct minx86dec_argv *rm = &ins->argv[0];
						struct minx86dec_argv *reg = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_MOVNTSD + ins->rep - MX86_REPE;
						ins->argc = 2;
						rm->size = reg->size = 16;
						set_sse_register(reg,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[0];
						struct minx86dec_argv *reg = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_MOVNTPS - (dataprefix32 & 1);
						ins->argc = 2;
						rm->size = reg->size = 16;
						set_sse_register(reg,mrm.f.reg);
						decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
					}
					break;
				case 0xEB:
					if (ins->rep >= MX86_REPE) {
						/* who wants to bet Intel will add instructions by overloading this one? >:( */
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[0];
						struct minx86dec_argv *reg = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_POR;
						ins->argc = 2;
						if (dataprefix32) {
							rm->size = reg->size = 16;
							set_sse_register(reg,mrm.f.reg);
							decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
						}
						else {
							rm->size = reg->size = 8;
							set_mmx_register(reg,mrm.f.reg);
							decode_rm_ex(mrm,rm,isaddr32,MX86_RT_MMX);
						}
					}
					break;

				case 0xDA:
					if (ins->rep >= MX86_REPE) {
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[1];
						struct minx86dec_argv *reg = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						ins->argc = 2;
						ins->opcode = MXOP_PMINUB;
						if (dataprefix32) set_sse_register(reg,mrm.f.reg);
						else set_mmx_register(reg,mrm.f.reg);
						rm->size = reg->size = dataprefix32 ? 16 : 8;
						decode_rm_ex(mrm,rm,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					}
					break;

				case 0xDE:
					if (ins->rep >= MX86_REPE) {
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[1];
						struct minx86dec_argv *reg = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						ins->argc = 2;
						ins->opcode = MXOP_PMAXUB;
						if (dataprefix32) set_sse_register(reg,mrm.f.reg);
						else set_mmx_register(reg,mrm.f.reg);
						rm->size = reg->size = dataprefix32 ? 16 : 8;
						decode_rm_ex(mrm,rm,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					}
					break;

				case 0xEA:
					if (ins->rep >= MX86_REPE) {
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[1];
						struct minx86dec_argv *reg = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						ins->argc = 2;
						ins->opcode = MXOP_PMINSW;
						if (dataprefix32) set_sse_register(reg,mrm.f.reg);
						else set_mmx_register(reg,mrm.f.reg);
						rm->size = reg->size = dataprefix32 ? 16 : 8;
						decode_rm_ex(mrm,rm,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					}
					break;

				case 0xEE:
					if (ins->rep >= MX86_REPE) {
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[1];
						struct minx86dec_argv *reg = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						ins->argc = 2;
						ins->opcode = MXOP_PMAXSW;
						if (dataprefix32) set_sse_register(reg,mrm.f.reg);
						else set_mmx_register(reg,mrm.f.reg);
						rm->size = reg->size = dataprefix32 ? 16 : 8;
						decode_rm_ex(mrm,rm,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					}
					break;

				case 0xF5:
					if (ins->rep >= MX86_REPE) {
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[1];
						struct minx86dec_argv *reg = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						ins->argc = 2;
						ins->opcode = MXOP_PMADDWD;
						if (dataprefix32) set_sse_register(reg,mrm.f.reg);
						else set_mmx_register(reg,mrm.f.reg);
						rm->size = reg->size = dataprefix32 ? 16 : 8;
						decode_rm_ex(mrm,rm,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
					}
					break;

				case 0xE7:
					if (ins->rep >= MX86_REPE) {
					}
					else {
						struct minx86dec_argv *rm = &ins->argv[0];
						struct minx86dec_argv *reg = &ins->argv[1];
						union x86_mrm mrm = fetch_modregrm();
						ins->argc = 2;
						if (dataprefix32) {
							ins->opcode = MXOP_MOVNTDQ;
							rm->size = reg->size = 16;
							set_sse_register(reg,mrm.f.reg);
							decode_rm_ex(mrm,rm,isaddr32,MX86_RT_SSE);
						}
						else {
							ins->opcode = MXOP_MOVNTQ;
							rm->size = reg->size = 8;
							set_mmx_register(reg,mrm.f.reg);
							decode_rm_ex(mrm,rm,isaddr32,MX86_RT_MMX);
						}
					}
					break;
				case 0xD6:
					if (ins->rep >= MX86_REPE) {
						const unsigned int which = ins->rep - MX86_REPE;
						struct minx86dec_argv *mm = &ins->argv[which^1];
						struct minx86dec_argv *xmm = &ins->argv[which];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = which ? MXOP_MOVQ2DQ : MXOP_MOVDQ2Q;
						ins->argc = 2;
						mm->size = 8;
						xmm->size = 16;
						set_mmx_register(mm,mrm.f.reg);
						decode_rm_ex(mrm,xmm,isaddr32,MX86_RT_SSE);
					}
					else if (dataprefix32) {
						struct minx86dec_argv *d = &ins->argv[1];
						struct minx86dec_argv *s = &ins->argv[0];
						union x86_mrm mrm = fetch_modregrm();
						ins->opcode = MXOP_MOVQ;
						ins->argc = 2;
						d->size = 8;
						s->size = 16;
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					break;
				case 0xF7: {
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					s->size = d->size = dataprefix32 ? 16 : 8;
					ins->argc = 2;
					ins->opcode = dataprefix32 ? MXOP_MASKMOVDQU : MXOP_MASKMOVQ;
					if (dataprefix32) {
						set_sse_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					}
					else {
						set_mmx_register(d,mrm.f.reg);
						decode_rm_ex(mrm,s,isaddr32,MX86_RT_MMX);
					}
					} break;
				case 0xE6: {
					struct minx86dec_argv *d = &ins->argv[0];
					struct minx86dec_argv *s = &ins->argv[1];
					union x86_mrm mrm = fetch_modregrm();
					if (ins->rep >= MX86_REPE)
						ins->opcode = MXOP_CVTPD2DQ + ins->rep - MX86_REPE;
					else if (dataprefix32)
						ins->opcode = MXOP_CVTTPD2DQ;
					else
						break;

					ins->argc = 2;
					d->size = 16;
					set_sse_register(d,mrm.f.reg);
					s->size = 16; /* 128 bit = 16 bytes */
					decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
					break; }
				case 0xAE: {
					int m = -1;
					struct minx86dec_argv *d = &ins->argv[0];
					union x86_mrm mrm = fetch_modregrm();
					switch (mrm.f.reg) {
						case 0:	ins->opcode = MXOP_FXSAVE;  m = 1; break;
						case 1:	ins->opcode = MXOP_FXRSTOR; m = 1; break;
						case 2:	ins->opcode = MXOP_LDMXCSR; m = 0; break;
						case 3:	ins->opcode = MXOP_STMXCSR; m = 0; break;
						case 4: if (isaddr32) {
							ins->opcode = MXOP_XSAVE; ins->argc = 0;
							} else {
							/* undefined */
							} break;
						case 5:	if (isaddr32) {
							ins->opcode = MXOP_XRSTOR; ins->argc = 0; /* Intel you oughta be ashamed of yourself. this is a sick way to extend your instruction set. Oh, and why doesn't your documentation mention the need for the 0x67 prefix? */
							} else {
							ins->opcode = MXOP_LFENCE; ins->argc = 0;
							} break;
						case 6:	ins->opcode = MXOP_MFENCE; ins->argc = 0; break;
						case 7: if (mrm.f.mod == 3) {
							ins->opcode = MXOP_SFENCE; /* hah, get it? CLFLUSH with non-memory ref would not make sense, so let's use it for SFENCE! Pfffh... --J.C. */
							} else {
							ins->opcode = MXOP_CLFLUSH; m = 0;
							} break;
					}
					switch (m) {
						case 0:	ins->argc = 1;
							d->size = data32wordsize;
							decode_rm(mrm,d,isaddr32);
							break;
						case 1: if (mrm.f.mod == 3) break; /* illegal */
							ins->argc = 1;
							d->size = 512;
							decode_rm(mrm,d,isaddr32);
							break;
						};
					} break;
				case 0x3A: {
					/* eugh it gets worse */
					uint8_t third_byte = fetch_u8();
					switch (third_byte) {
						COVER_2(0x0C): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_BLENDPS + (third_byte & 1);
							ins->argc = 3;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x0E: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PBLENDW;
							ins->argc = 3;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x0F: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PALIGNR;
							ins->argc = 3;
							d->size = s->size = dataprefix32 ? 16 : 8;
							if (dataprefix32) set_sse_register(d,mrm.f.reg);
							else set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x14: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PEXTRB;
							ins->argc = 3;
							d->size = 4;
							s->size = 16;
							set_sse_register(s,mrm.f.reg);
							decode_rm(mrm,d,isaddr32);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x16: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PEXTRD;
							ins->argc = 3;
							d->size = 4;
							s->size = 16;
							set_sse_register(s,mrm.f.reg);
							decode_rm(mrm,d,isaddr32);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x17: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *s = &ins->argv[0];
							struct minx86dec_argv *d = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_EXTRACTPS;
							ins->argc = 3;
							d->size = 16;
							set_sse_register(d,mrm.f.reg);
							s->size = 4;
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_REG);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x20: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PINSRB;
							ins->argc = 3;
							d->size = 16;
							s->size = 4;
							set_sse_register(d,mrm.f.reg);
							decode_rm(mrm,s,isaddr32);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x22: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PINSRD;
							ins->argc = 3;
							d->size = 16;
							s->size = 4;
							set_sse_register(d,mrm.f.reg);
							decode_rm(mrm,s,isaddr32);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x42: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_MPSADBW;
							ins->argc = 3;
							d->size = 16;
							set_sse_register(d,mrm.f.reg);
							s->size = 16;
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0x21: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_INSERTPS;
							ins->argc = 3;
							d->size = 16;
							set_sse_register(d,mrm.f.reg);
							s->size = 16;
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						COVER_4(0x08): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_ROUNDPS + (third_byte & 3); /* PS/PD/SS/SD */
							ins->argc = 3;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						COVER_2(0x40): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_DPPS + (third_byte & 1);
							ins->argc = 3;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						COVER_4(0x60): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PCMPESTRM + (third_byte & 3);
							ins->argc = 3;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
						case 0xDF: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_AESKEYGENASSIST;
							ins->argc = 3;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 1;
							set_immediate(i,fetch_u8());
						} break;
					};
					} break;
				case 0x38: {
					/* eugh yuck */
					uint8_t third_byte = fetch_u8();
					switch (third_byte) {
						COVER_8(0x00): COVER_4(0x08): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PSHUFB + (third_byte & 15);
							ins->argc = 2;
							d->size = s->size = dataprefix32 ? 16 : 8;
							if (dataprefix32) set_sse_register(d,mrm.f.reg);
							else set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						} break;
						case 0x10: if (dataprefix32) {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_PBLENDVB;
							ins->argc = 3;
							d->size = s->size = i->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							set_sse_register(i,MX86_XMM0);
						} break;
						COVER_2(0x14): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							struct minx86dec_argv *i = &ins->argv[2];
							ins->opcode = MXOP_BLENDVPS + (third_byte & 1);
							ins->argc = 3;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							i->size = 16;
							set_sse_register(i,MX86_XMM0);
						} break;
						case 0x17: if (dataprefix32) {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PTEST;
							ins->argc = 2;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
						} break;
						COVER_2(0x1C): case 0x1E: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PABSB + (third_byte & 3);
							ins->argc = 2;
							d->size = s->size = dataprefix32 ? 16 : 8;
							if (dataprefix32) set_sse_register(d,mrm.f.reg);
							else set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						} break;
						COVER_4(0x20): COVER_2(0x24): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PMOVSXBW + (third_byte & 7);
							ins->argc = 2;
							d->size = s->size = dataprefix32 ? 16 : 8;
							if (dataprefix32) set_sse_register(d,mrm.f.reg);
							else set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						} break;
						COVER_2(0x28): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PMULDQ + (third_byte & 1);
							ins->argc = 2;
							d->size = s->size = dataprefix32 ? 16 : 8;
							if (dataprefix32) set_sse_register(d,mrm.f.reg);
							else set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						} break;
						case 0x2A: {
							if (ins->rep >= MX86_REPE) {
							}
							else if (dataprefix32) {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s = &ins->argv[1];
								ins->opcode = MXOP_MOVNTDQA;
								ins->argc = 2;
								d->size = s->size = 16;
								set_sse_register(d,mrm.f.reg);
								decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
							}
							else {
							}
						} break;
						case 0x2B: {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PACKUSDW;
							ins->argc = 2;
							d->size = s->size = 16;
							set_sse_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,MX86_RT_SSE);
						} break;
						COVER_4(0x30): COVER_2(0x34): {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PMOVZXBW + (third_byte & 7);
							ins->argc = 2;
							d->size = s->size = dataprefix32 ? 16 : 8;
							if (dataprefix32) set_sse_register(d,mrm.f.reg);
							else set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						} break;
						case 0x37: COVER_8(0x38): COVER_2(0x40): if (dataprefix32) {
							union x86_mrm mrm = fetch_modregrm();
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							ins->opcode = MXOP_PCMPGTQ + third_byte - 0x37;
							ins->argc = 2;
							d->size = s->size = dataprefix32 ? 16 : 8;
							if (dataprefix32) set_sse_register(d,mrm.f.reg);
							else set_mmx_register(d,mrm.f.reg);
							decode_rm_ex(mrm,s,isaddr32,dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
						} break;
						COVER_2(0x80): {
							if (dataprefix32) {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *s = &ins->argv[0];
								struct minx86dec_argv *d = &ins->argv[1];
								ins->opcode = MXOP_INVEPT + (third_byte & 1);
								d->size = s->size = 4;
								ins->argc = 2;
								set_register(s,mrm.f.reg);
								decode_rm(mrm,d,isaddr32);
							}
						} break;
						case 0xDB: { /* AES IMC */
							if (dataprefix32) {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *s = &ins->argv[0];
								struct minx86dec_argv *d = &ins->argv[1];
								ins->opcode = MXOP_AESIMC;
								d->size = s->size = 16;
								ins->argc = 2;
								set_sse_register(s,mrm.f.reg);
								decode_rm_ex(mrm,d,isaddr32,MX86_RT_SSE);
							}
						} break;
						COVER_4(0xDC): { /* AES ENC/DEC */
							if (dataprefix32) {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *s = &ins->argv[0];
								struct minx86dec_argv *d = &ins->argv[1];
								ins->opcode = MXOP_AESENC + (third_byte & 3);
								d->size = s->size = 16;
								ins->argc = 2;
								set_sse_register(s,mrm.f.reg);
								decode_rm_ex(mrm,d,isaddr32,MX86_RT_SSE);
							}
						} break;
						COVER_2(0xF0):
							if (ins->rep == MX86_REPE) {
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *d = &ins->argv[0];
								struct minx86dec_argv *s = &ins->argv[1];
								ins->opcode = MXOP_CRC32;
								ins->argc = 2;
								d->size = 4;
								set_register(d,mrm.f.reg);
								s->size = (third_byte & 1) ? data32wordsize : 1;
								decode_rm(mrm,s,isaddr32);
							}
							else {
								const unsigned int which = (third_byte & 1);
								union x86_mrm mrm = fetch_modregrm();
								struct minx86dec_argv *re = &ins->argv[which];
								struct minx86dec_argv *rm = &ins->argv[which^1];
								ins->opcode = MXOP_MOVBE;
								ins->argc = 2;
								re->size = rm->size = data32wordsize;
								set_register(re,mrm.f.reg);
								decode_rm(mrm,rm,isaddr32);
							}
							break;
					};
					} break;
#  endif
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
				set_register(ai,mrm.f.reg);
				decode_rm(mrm,mp,isaddr32);
			}
			break;
		COVER_4(0x6C):
			ins->opcode = MXOP_INS + ((first_byte >> 1) & 1);
			ins->argc = 1; {
				struct minx86dec_argv *r = &ins->argv[0];
				r->size = (first_byte & 1) ? data32wordsize : 1;
				r->regtype = MX86_RT_NONE;
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
				set_immediate(a,fetch_u16());
				set_immediate(b,fetch_u8());
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
				set_register(s,mrm.f.reg);
				decode_rm(mrm,d,isaddr32);
			}
			break;
		case 0x68: /* FIXME: did this version of PUSH appear on the 286? */
			ins->opcode = MXOP_PUSH;
			ins->argc = 1; {
				struct minx86dec_argv *s = &ins->argv[0];
				s->size = data32wordsize;
				set_immediate(s,isdata32 ? fetch_u32() : fetch_u16());
			}
			break;
		case 0x6A: /* FIXME: did this version of PUSH appear on the 286? */
			ins->opcode = MXOP_PUSH;
			ins->argc = 1; {
				struct minx86dec_argv *s = &ins->argv[0];
				s->size = 1;
				set_immediate(s,fetch_u8());
			}
			break;
#endif
#if core_level >= 3
		/* 386+ instruction 32-bit prefixes */
		case 0x66: /* 32-bit data override */
			ins->data32 ^= 1;
			dataprefix32++;
			if (--patience) goto decode_next;
			break;

		case 0x67: /* 32-bit address override */
			ins->addr32 ^= 1;
			addrprefix32++;
			if (--patience) goto decode_next;
			break;
#endif
#if 1 /* TODO: #define for FPU support */
/* ---------------------------------- FPU instructions ---------------------------------- */
/* TODO: separate out FPU instructions according to whether we are decoding for 286, 386, etc.. */
		case 0xD8: {
			/* alright this is where it gets messy: second byte can be opcode OR a mod/reg/rm combination! */
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
					COVER_8(0xC0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FADD;
						ins->argc = 2;
					} break;
					COVER_8(0xC8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FMUL;
						ins->argc = 2;
					} break;
					COVER_8(0xD0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FCOM;
						ins->argc = 2;
					} break;
					COVER_8(0xD8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FCOMP;
						ins->argc = 2;
					} break;
					COVER_8(0xE0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->regtype = s->regtype = MX86_RT_ST;
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FSUB;
						ins->argc = 2;
					} break;
					COVER_8(0xE8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->regtype = s->regtype = MX86_RT_ST;
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FSUBR;
						ins->argc = 2;
					} break;
					COVER_8(0xF0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->regtype = s->regtype = MX86_RT_ST;
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FDIV;
						ins->argc = 2;
					} break;
					COVER_8(0xF8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						d->regtype = s->regtype = MX86_RT_ST;
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FDIVR;
						ins->argc = 2;
					} break;
				} break;
			}
			else {
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						ins->opcode = MXOP_FADD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 1:
						ins->opcode = MXOP_FMUL;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 2:
						ins->opcode = MXOP_FCOM;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 3:
						ins->opcode = MXOP_FCOMP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 4:
						ins->opcode = MXOP_FSUB;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 5:
						ins->opcode = MXOP_FSUBR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 6:
						ins->opcode = MXOP_FDIV;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 7:
						ins->opcode = MXOP_FDIVR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
				}
			}
			} break;
		case 0xD9: {
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
					COVER_8(0xC0): {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FLD;
						ins->argc = 1;
					} break;
					COVER_8(0xC8): {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FXCH;
						ins->argc = 1;
					} break;
					case 0xD0: {
						ins->opcode = MXOP_FNOP;
						ins->argc = 0;
					} break;
					case 0xE0: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCHS;
						ins->argc = 1;
					} break;
					case 0xE1: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FABS;
						ins->argc = 1;
					} break;
					case 0xE4: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FTST;
						ins->argc = 1;
					} break;
					case 0xE5: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FXAM;
						ins->argc = 1;
					} break;
					case 0xE8: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FLD1;
						ins->argc = 1;
					} break;
					case 0xE9: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FLDL2T;
						ins->argc = 1;
					} break;
					case 0xEA: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FLDL2E;
						ins->argc = 1;
					} break;
					case 0xEB: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FLDPI;
						ins->argc = 1;
					} break;
					case 0xEC: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FLDLG2;
						ins->argc = 1;
					} break;
					case 0xED: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FLDLN2;
						ins->argc = 1;
					} break;
					case 0xEE: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FLDZ;
						ins->argc = 1;
					} break;
					case 0xF0: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_F2XM1;
						ins->argc = 1;
					} break;
					case 0xF1: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FYL2X;
						ins->argc = 1;
					} break;
					case 0xF2: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FPTAN;
						ins->argc = 1;
					} break;
					case 0xF3: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FPATAN;
						ins->argc = 1;
					} break;
					case 0xF4: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FXTRACT;
						ins->argc = 1;
					} break;
#if fpu_level >= 3
					case 0xF5: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FPREM1;
						ins->argc = 1;
					} break;
#endif
					case 0xF6: {
						ins->opcode = MXOP_FDECSTP;
						ins->argc = 0;
					} break;
					case 0xF7: {
						ins->opcode = MXOP_FINCSTP;
						ins->argc = 0;
					} break;
					case 0xF8: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FPREM;
						ins->argc = 1;
					} break;
					case 0xF9: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FYL2XP1;
						ins->argc = 1;
					} break;
					case 0xFA: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FSQRT;
						ins->argc = 1;
					} break;
#if fpu_level >= 3
					case 0xFB: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FSINCOS;
						ins->argc = 1;
					} break;
#endif
					case 0xFC: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FRNDINT;
						ins->argc = 1;
					} break;
					case 0xFD: {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(1));
						ins->opcode = MXOP_FSCALE;
						ins->argc = 2;
					} break;
#if fpu_level >= 3
					case 0xFE: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FSIN;
						ins->argc = 1;
					} break;
					case 0xFF: {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCOS;
						ins->argc = 1;
					} break;
#endif
				}
			}
			else {
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						ins->opcode = MXOP_FLD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 2:
						ins->opcode = MXOP_FST;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 3:
						ins->opcode = MXOP_FSTP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 4:
						ins->opcode = MXOP_FLDENV;
						ins->argc = 1; {
							struct minx86dec_argv *s = &ins->argv[0];
							/* src */
							s->size = 14;	/* TODO: How big is this? */
							decode_rm(mrm,s,isaddr32);
						} break;
					case 5:
						ins->opcode = MXOP_FLDCW;
						ins->argc = 1; {
							struct minx86dec_argv *s = &ins->argv[0];
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 6:
						ins->opcode = fwait ? MXOP_FSTENV : MXOP_FNSTENV;
						ins->argc = 1; {
							struct minx86dec_argv *s = &ins->argv[0];
							/* src */
							s->size = 4;	/* TODO: How big is this? */
							decode_rm(mrm,s,isaddr32);
						} break;
					case 7:
						ins->opcode = fwait ? MXOP_FSTCW : MXOP_FNSTCW;
						ins->argc = 1; {
							struct minx86dec_argv *s = &ins->argv[0];
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
				}
			}
			} break;
		case 0xDA: {
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
#if fpu_level >= 6 || (fpu_level == 5 && defined(pentiumpro))
					COVER_8(0xC0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVB;
						ins->argc = 2;
					} break;
					COVER_8(0xC8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVE;
						ins->argc = 2;
					} break;
					COVER_8(0xD0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVBE;
						ins->argc = 2;
					} break;
					COVER_8(0xD8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVU;
						ins->argc = 2;
					} break;
#endif
#if fpu_level >= 3
					case 0xE9: {
						struct minx86dec_argv *s = &ins->argv[0];
						set_fpu_register(s,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FUCOMPP;
						ins->argc = 1;
					} break;
#endif
				}
			}
			else {
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						ins->opcode = MXOP_FIADD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 1:
						ins->opcode = MXOP_FIMUL;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 2:
						ins->opcode = MXOP_FICOM;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 3:
						ins->opcode = MXOP_FICOMP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 4:
						ins->opcode = MXOP_FISUB;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 5:
						ins->opcode = MXOP_FISUBR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 6:
						ins->opcode = MXOP_FIDIV;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 7:
						ins->opcode = MXOP_FIDIVR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
				}
			}
			} break;
		case 0xDB: {
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
#if fpu_level >= 6 || (fpu_level == 5 && defined(pentiumpro))
					COVER_8(0xC0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVNB;
						ins->argc = 2;
					} break;
					COVER_8(0xC8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVNE;
						ins->argc = 2;
					} break;
					COVER_8(0xD0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVNBE;
						ins->argc = 2;
					} break;
					COVER_8(0xD8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCMOVNU;
						ins->argc = 2;
					} break;
					COVER_8(0xE8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FUCOMI;
						ins->argc = 2;
					} break;
					COVER_8(0xF0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCOMI;
						ins->argc = 2;
					} break;
#endif
					case 0xE0: {
						ins->opcode = fwait ? MXOP_FNENI : MXOP_FENI;
						ins->argc = 0;
					} break;
					case 0xE1: {
						ins->opcode = fwait ? MXOP_FNDISI : MXOP_FDISI;
						ins->argc = 0;
					} break;
					case 0xE2: {	/* 0xDB 0xE2 or 0x9B 0xDB 0xE2 */
						ins->opcode = fwait ? MXOP_FCLEX : MXOP_FNCLEX;
						ins->argc = 0;
					} break;
					case 0xE3: {	/* 0xDB 0xE3 or 0x9B 0xDB 0xE3 */
						ins->opcode = fwait ? MXOP_FINIT : MXOP_FNINIT;
						ins->argc = 0;
					} break;
#if fpu_level >= 2 || defined(everything)
					case 0xE4: {
						ins->opcode = MXOP_FSETPM;
						ins->argc = 0;
					} break;
#endif
				} break;
			}
			else {
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						ins->opcode = MXOP_FILD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 1:
						ins->opcode = MXOP_FISTTP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 2:
						ins->opcode = MXOP_FIST;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 3:
						ins->opcode = MXOP_FISTP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 4;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 5:
						ins->opcode = MXOP_FLD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 10;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 7:
						ins->opcode = MXOP_FSTP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 10;
							decode_rm(mrm,s,isaddr32);
						} break;

				}
			} break; }
		case 0xDC: {
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
					COVER_8(0xC0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FADD;
						ins->argc = 2;
					} break;
					COVER_8(0xC8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FMUL;
						ins->argc = 2;
					} break;
					COVER_8(0xE0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FSUBR;
						ins->argc = 2;
					} break;
					COVER_8(0xE8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FSUB;
						ins->argc = 2;
					} break;
					COVER_8(0xF0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FDIVR;
						ins->argc = 2;
					} break;
					COVER_8(0xF8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FDIV;
						ins->argc = 2;
					} break;
				}
			}
			else {
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						ins->opcode = MXOP_FADD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 1:
						ins->opcode = MXOP_FMUL;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 2:
						ins->opcode = MXOP_FCOM;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 3:
						ins->opcode = MXOP_FCOMP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 4:
						ins->opcode = MXOP_FSUB;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 5:
						ins->opcode = MXOP_FSUBR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 6:
						ins->opcode = MXOP_FDIV;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 7:
						ins->opcode = MXOP_FDIVR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
				}
			} break; }
		case 0xDD: {
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
					COVER_8(0xC0): {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FFREE;
						ins->argc = 1;
					} break;
					COVER_8(0xD0): {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FST;
						ins->argc = 1;
					} break;
					COVER_8(0xD8): {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FSTP;
						ins->argc = 1;
					} break;
#if fpu_level >= 3
					COVER_8(0xE0): {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FUCOM;
						ins->argc = 1;
					} break;
					COVER_8(0xE8): {
						struct minx86dec_argv *d = &ins->argv[0];
						set_fpu_register(d,MX86_ST(second_byte&7));
						ins->opcode = MXOP_FUCOMP;
						ins->argc = 1;
					} break;
#endif
				}
			}
			else {
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						ins->opcode = MXOP_FLD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 1:
						ins->opcode = MXOP_FISTTP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 2:
						ins->opcode = MXOP_FST;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 3:
						ins->opcode = MXOP_FSTP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 8;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 4:
						ins->opcode = MXOP_FRSTOR;
						ins->argc = 1; {
							struct minx86dec_argv *s = &ins->argv[0];
							/* src */
							s->size = 94;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 6:
						ins->opcode = fwait ? MXOP_FSAVE : MXOP_FNSAVE;
						ins->argc = 1; {
							struct minx86dec_argv *s = &ins->argv[0];
							/* src */
							s->size = 94;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 7:
						ins->opcode = fwait ? MXOP_FSTSW : MXOP_FNSTSW;
						ins->argc = 1; {
							struct minx86dec_argv *s = &ins->argv[0];
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;

				}
			} break; }
		case 0xDE: {
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
					COVER_8(0xC0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FADDP;
						ins->argc = 2;
					} break;
					COVER_8(0xC8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FMULP;
						ins->argc = 2;
					} break;
					case 0xD9: {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(0));
						set_fpu_register(s,MX86_ST(1));
						ins->opcode = MXOP_FCOMPP;
						ins->argc = 2;
					} break;
					COVER_8(0xE0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FSUBRP;
						ins->argc = 2;
					} break;
					COVER_8(0xE8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FSUBP;
						ins->argc = 2;
					} break;
					COVER_8(0xF0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FDIVRP;
						ins->argc = 2;
					} break;
					COVER_8(0xF8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(d,MX86_ST(second_byte&7));
						set_fpu_register(s,MX86_ST(0));
						ins->opcode = MXOP_FDIVP;
						ins->argc = 2;
					} break;
				} break;
			}
			else {
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				switch (mrm.f.reg) {
					case 0:
						ins->opcode = MXOP_FIADD;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 1:
						ins->opcode = MXOP_FIMUL;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 2:
						ins->opcode = MXOP_FICOM;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 3:
						ins->opcode = MXOP_FICOMP;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 4:
						ins->opcode = MXOP_FISUB;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 5:
						ins->opcode = MXOP_FISUBR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 6:
						ins->opcode = MXOP_FIDIV;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
					case 7:
						ins->opcode = MXOP_FIDIVR;
						ins->argc = 2; {
							struct minx86dec_argv *d = &ins->argv[0];
							struct minx86dec_argv *s = &ins->argv[1];
							/* ST(0) */
							set_fpu_register(d,MX86_ST(0));
							/* src */
							s->size = 2;
							decode_rm(mrm,s,isaddr32);
						} break;
				}
			} break; }
		case 0xDF: {
			const uint8_t second_byte = *cip;
			if ((second_byte & 0xC0) == 0xC0) {
				cip++;	/* it was second byte of the opcode, step forward */
				switch (second_byte) {
					case 0xE0: {
						struct minx86dec_argv *d = &ins->argv[0];
						d->size = 2; set_register(d,MX86_REG_AX);
						ins->opcode = fwait ? MXOP_FSTSW : MXOP_FNSTSW;
						ins->argc = 1;
					} break;
#if fpu_level >= 6 || (fpu_level == 5 && defined(pentiumpro))
					COVER_8(0xE8): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FUCOMIP;
						ins->argc = 2;
					} break;
					COVER_8(0xF0): {
						struct minx86dec_argv *d = &ins->argv[0];
						struct minx86dec_argv *s = &ins->argv[1];
						set_fpu_register(s,MX86_ST(second_byte&7));
						set_fpu_register(d,MX86_ST(0));
						ins->opcode = MXOP_FCOMIP;
						ins->argc = 2;
					} break;
#endif
				} break;
			}
			else {
				/* all instructions follow a common pattern, we just have to list variances here */
				static const unsigned int map[8*2] = {
					MXOP_FILD,	2,
					MXOP_FISTTP,	2,
					MXOP_FIST,	2,
					MXOP_FISTP,	2,
					MXOP_FBLD,	10,
					MXOP_FILD,	8,
					MXOP_FBSTP,	10,
					MXOP_FISTP,	8
				};
				/* mod/reg/rm */
				union x86_mrm mrm = fetch_modregrm();
				const unsigned int b = mrm.f.reg << 1;
				struct minx86dec_argv *d = &ins->argv[0];
				struct minx86dec_argv *s = &ins->argv[1];
				set_fpu_register(d,MX86_ST(0));
				ins->opcode = map[b+0];
				s->size = map[b+1];
				ins->argc = 2;
				decode_rm(mrm,s,isaddr32);
			} break; }
#endif

		default:
			/* fall through */
			break;
	};
}

#undef data32wordsize

/* } */

