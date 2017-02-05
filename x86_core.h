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
#ifndef isdata64
#define isdata64 (0)
#endif

/* TODO: there is code here to look for "defined(merced_itanium_x86)", make an itanium x86 core */

/* FIXME: A lot of MMX decoding is enabled for the 586pro core, when it turns out the Pentium Pro
          did *NOT* have MMX decoding. a 586mmx core was added instead (#define pentium_mmx).
          This code should be modified NOT to decode MMX unless the core supports it */

/* see [http://oopweb.com/Assembly/Documents/InterList/Volume/OPCODES.LST] */
/* TODO: IBM 486SLC core */
/* TODO: Intel Itanium x86 decoding including Merced "JMPX" instruction */
/* TODO: Cyrix 6x86 instructions you missed: PDISTIB PMACHRIW PMAGW PSUBIW */
/* TODO: Cyrix SMM instructions: RDSHR, WRSHR */
/* TODO: Cyrix/IBM/TI 486-compatible, instructions: RSDC, RSLDT, RSTS, SMI, SVDC, SVLDT, SVTS */

#define data32wordsize (isdata32 ? 4 : 2)
#define addr32wordsize (isaddr32 ? 4 : 2)
#define data64wordsize (isdata64 ? 8 : data32wordsize)

#ifdef x64_mode
#  define native_int_t		uint64_t
#  define decode_rm_		decode_rm_x64_reg
#  define decode_rm_ex_		decode_rm_x64
#  define datawordsize		data64wordsize
#  define addr64wordsize	(isaddr32 ? 4 : 8)
#  define addrwordsize		addr64wordsize
#  define stackwordsize		(dataprefix32 ? 2 : 8)
#  define ctrlwordsize		8
#  define ARGV			struct minx86dec_argv_x64
#  define INS_MRM		struct x64_mrm
#else
#  define native_int_t		uint32_t
#  define decode_rm_		decode_rm_x86_reg
#  define decode_rm_ex_		decode_rm_x86
#  define datawordsize		data32wordsize
#  define addrwordsize		addr32wordsize
#  define stackwordsize		data32wordsize
#  define ctrlwordsize		4
#  define ARGV			struct minx86dec_argv
#  define INS_MRM		union x86_mrm
#endif

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
#define COVER_2ROW(x) COVER_ROW(x): COVER_ROW(x+0x10)
#define COVER_4ROW(x) COVER_2ROW(x): COVER_2ROW(x+0x20)
#define COVER_8ROW(x) COVER_4ROW(x): COVER_4ROW(x+0x40)
#define COVER_16ROW(x) COVER_8ROW(x): COVER_8ROW(x+0x80)

#ifndef ibmslc_level
# define ibmslc_level 0
#endif

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

#if defined(vex_level)

/* template:
 * AMD 0x8F VEX instruction of the form
 * 
 * op    A,B,C,D
 * A = mrm.f.reg
 * B = v.f.v
 * C = r/m
 * D = top 4 bits of immediate byte
 * all registers are SSE xmm/ymm */
#  define typical_x86_amd_vex_m_v_rm_it4(op,vector_size)				\
{											\
	ARGV *d = &ins->argv[0],*s1 = &ins->argv[1];					\
	ARGV *s2 = &ins->argv[2],*s3 = &ins->argv[3];					\
	ins->argc = 4,ins->opcode = op;							\
	d->size = s1->size = s2->size = s3->size = vector_size;				\
	INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_SSE);	\
	set_sse_register(d,mrm.f.reg);							\
	set_sse_register(s1,v.f.v);							\
	unsigned char c = fetch_u8();							\
	set_sse_register(s3,c>>4);							\
}

/* template:
 * AMD 0x8F VEX instruction of the form
 * 
 * op    A,B,C,D
 * A = mrm.f.reg
 * B = v.f.v
 * C = r/m
 * D = 8-bit immediate
 * all registers are SSE xmm/ymm */
#  define typical_x86_amd_vex_m_v_rm_i8(op,vector_size)					\
{											\
	ARGV *d = &ins->argv[0],*s1 = &ins->argv[1];					\
	ARGV *s2 = &ins->argv[2],*s3 = &ins->argv[3];					\
	ins->argc = 4,ins->opcode = op;							\
	d->size = s1->size = s2->size = s3->size = vector_size;				\
	INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_SSE);	\
	set_sse_register(d,mrm.f.reg);							\
	set_sse_register(s1,v.f.v);							\
	set_immediate(s3,fetch_u8());							\
}

/* template:
 * AMD 0x8F VEX instruction of the form
 * 
 * op    A,B,C
 * A = mrm.f.reg
 * B = r/m
 * C = 8-bit immediate
 * all registers are SSE xmm/ymm */
#  define typical_x86_amd_vex_m_rm_i8(op,vector_size)					\
{											\
	ARGV *d = &ins->argv[0],*s1 = &ins->argv[1];					\
	ARGV *s2 = &ins->argv[2];							\
	ins->opcode = op,ins->argc = 3,s2->size = 1;					\
	d->size = s1->size = vector_size;						\
	INS_MRM mrm = decode_rm_ex_(s1,ins,s1->size,PLUSR_TRANSFORM,MX86_RT_SSE);	\
	set_sse_register(d,mrm.f.reg);							\
	set_immediate(s2,fetch_u8());							\
}

/* template:
 * AMD 0x8F VEX instruction of the form
 * 
 * op    A,B
 * A = mrm.f.reg
 * B = r/m
 * all registers are SSE xmm/ymm */
#  define typical_x86_amd_vex_m_rm(op,vector_size)					\
{											\
	ARGV *d = &ins->argv[0],*s = &ins->argv[1];					\
	ins->opcode = op,ins->argc = 2;							\
	d->size = s->size = vector_size;						\
	INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);		\
	set_sse_register(d,mrm.f.reg);							\
}

/* template:
 * AMD 0x8F VEX instruction of the form
 * 
 * op    A,B,C
 * A = mrm.f.reg
 * B = r/m
 * C = v.f.v
 * all registers are SSE xmm/ymm
 * v.f.w if set switches order of B & C */
#  define typical_x86_amd_vex_m_rm_v_wswap(op,vector_size)				\
{											\
	ARGV *d = &ins->argv[0],*s1 = &ins->argv[1+v.f.w];				\
	ARGV *s2 = &ins->argv[2-v.f.w];							\
	ins->opcode = op,ins->argc = 3;							\
	d->size = s1->size = s2->size = vector_size;					\
	INS_MRM mrm = decode_rm_ex_(s1,ins,s1->size,PLUSR_TRANSFORM,MX86_RT_SSE);	\
	set_sse_register(d,mrm.f.reg);							\
	set_sse_register(s2,v.f.v);							\
}

#endif

ins->fwait = 0; ins->lock = 0; ins->argc = 0;
ins->argv[0].memregs = ins->argv[1].memregs = ins->argv[2].memregs = ins->argv[3].memregs = ins->argv[4].memregs = 0;
//ins->argv[0].segment = ins->argv[1].segment = ins->argv[2].segment = ins->argv[3].segment = ins->argv[4].segment = MX86_SEG_DS;
#if defined(vex_level)
ins->vex.raw = 0;
#endif
#if defined(x64_mode)
ins->rex.raw = 0;
#endif
ins->rep = MX86_REP_NONE;
decode_next:
{
	const uint8_t first_byte = fetch_u8();
	switch (first_byte) {
		/* group 0x00-0x3F opcodes */
		COVER_2(0x04):	/* ADD */	COVER_2(0x0C):	/* OR */
		COVER_2(0x14):	/* ADC */	COVER_2(0x1C):	/* SBB */
		COVER_2(0x24):	/* AND */	COVER_2(0x2C):	/* SUB */
		COVER_2(0x34):	/* XOR */	COVER_2(0x3C):	/* CMP */ {
			ins->opcode = MXOP_ADD+(first_byte>>3); ins->argc = 2; /* NO MEM REF */
			ARGV *imm=&ins->argv[1],*reg=&ins->argv[0]; imm->size = reg->size = (first_byte&1)?datawordsize:1;
			set_immediate(imm,(first_byte&1)?imm32sbysize(ins):fetch_u8()); set_register(reg,MX86_REG_AX);
		} break;
		/* group 0x00-0x3F opcodes */
		COVER_4(0x00):	/* ADD */	COVER_4(0x08):	/* OR */
		COVER_4(0x10):	/* ADC */	COVER_4(0x18):	/* SBB */
		COVER_4(0x20):	/* AND */	COVER_4(0x28):	/* SUB */
		COVER_4(0x30):	/* XOR */	COVER_4(0x38):	/* CMP */ {
			ins->opcode = MXOP_ADD+(first_byte>>3); ins->argc = 2; const int which = (first_byte>>1)&1;
			ARGV *rm=&ins->argv[which],*reg=&ins->argv[which^1];rm->size=reg->size=(first_byte&1)?datawordsize:1;
			INS_MRM mrm = decode_rm_(rm,ins,reg->size,PLUSR_TRANSFORM);
			set_register(reg,plusr_transform(ins,reg->size,mrm.f.reg));
		} break;
#ifndef x64_mode /* not valid in 64-bit mode */
# if core_level == 0
		case 0x0F: /* POP CS */
# endif
		case 0x06: case 0x07: case 0x0E: case 0x16: case 0x17: case 0x1E: case 0x1F: {
			ins->opcode = MXOP_PUSH+(first_byte&1); ins->argc = 1;
			ARGV *a = &ins->argv[0]; a->size = data32wordsize; set_segment_register(a,first_byte >> 3);
		} break;
#endif
		case 0x26: case 0x2E: case 0x36: case 0x3E: { /* segment overrides */
			ins->argv[0].segment = ins->argv[1].segment = ins->argv[2].segment = ins->argv[3].segment =
			ins->segment = (first_byte >> 3) & 3; next_round();
		} break;
#ifndef x64_mode /* not valid in 64-bit mode */
		case 0x27: case 0x2F: case 0x37: case 0x3F: ins->opcode = MXOP_DAA+((first_byte>>3)&3); break;
#endif
#ifdef x64_mode
		/* REX prefix */
		COVER_ROW(0x40): {
			ins->data64 = (first_byte >> 3)&1; ins->rex.raw = first_byte; next_round();
		}
#else
		/* INC/DEC register */
		COVER_ROW(0x40): {
			ins->opcode = MXOP_INC+((first_byte>>3)&1); ins->argc = 1;
			ARGV *reg = &ins->argv[0]; reg->size = data32wordsize; set_register(reg,first_byte & 7);
		} break;
#endif
		/* PUSH/POP register */
		COVER_ROW(0x50): {
			ins->opcode = MXOP_PUSH+((first_byte>>3)&1); ins->argc = 1;
			ARGV *reg = &ins->argv[0]; reg->size = stackwordsize; set_register(reg,first_byte & 7);
		} break;
#if core_level >= 1
		COVER_2(0x60): ins->opcode = MXOP_PUSHA+(first_byte&1)+(isdata32?2:0); break;
#endif
#if core_level >= 1 && !defined(x64_mode)
		case 0x62: {
			ins->opcode = MXOP_BOUND; ins->argc = 2;
			ARGV *ai=&ins->argv[0],*mp=&ins->argv[1]; ai->size=data32wordsize; mp->size=data32wordsize*2;
			INS_MRM mrm = decode_rm_(mp,ins,mp->size,PLUSR_TRANSFORM); set_register(ai,mrm.f.reg);
		} break;
#endif
#if core_level >= 2 && !defined(x64_mode)
		case 0x63: {
			ins->opcode = MXOP_ARPL; ins->argc = 2;
			ARGV *d=&ins->argv[0],*s=&ins->argv[1]; s->size = d->size = 2;
			INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
		}
		break;
#endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
		case 0x63: { /* BRKN <int> */
			ins->opcode = MXOP_BRKN; ins->argc = 1;
			ARGV *r = &ins->argv[0]; set_immediate(r,fetch_u8());
		} break;
# endif
#if core_level >= 3
		COVER_2(0x64):
			ins->segment = (first_byte & 1) + MX86_SEG_FS; next_round();
#endif
#if defined(do_necv20) /* NEC V20/V30 */
		COVER_2(0x64):
			ins->rep = (first_byte & 1) + MX86_REPNC; next_round();
#endif
#if core_level >= 3
		case 0x66:
			ins->data32 ^= 1; dataprefix32++; next_round();
		case 0x67:
			ins->addr32 ^= 1; addrprefix32++; next_round();
#endif
#if defined(do_necv20) /* NEC V20/V30 */
		COVER_2(0x66): {
			ins->opcode = MXOP_FP02; ins->argc = 2;
			ARGV *s = &ins->argv[0],*d = &ins->argv[1]; d->size = s->size = 2;
			INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM);
			set_immediate(d,mrm.f.reg|((first_byte&1)<<3)); set_register(s,mrm.f.rm);
		} break;
#endif
#if core_level >= 2
		case 0x68: {
			ins->opcode = MXOP_PUSH; ins->argc = 1;
			ARGV *s = &ins->argv[0]; s->size = data32wordsize;
#if defined(x64_mode)
			set_immediate(s,(uint64_t)(isdata32 ? (int32_t)fetch_u32() : (int16_t)fetch_u16()));
#else
			set_immediate(s,isdata32 ? fetch_u32() : fetch_u16());
#endif
		} break;
#endif
#if core_level >= 1
		case 0x69: {
			ins->opcode = MXOP_IMUL; ins->argc = 3;
            ARGV *d = &ins->argv[0];
            ARGV *s = &ins->argv[1];
            ARGV *i = &ins->argv[2];
            d->size = data32wordsize;
            s->size = data32wordsize;
            i->size = data32wordsize;
            INS_MRM mrm=decode_rm_(s,ins,d->size,PLUSR_TRANSFORM);
            set_register(d,mrm.f.reg);
            set_immediate(i,isdata32 ? fetch_u32() : fetch_u16());
		} break;
#endif
#if core_level >= 2
		case 0x6A: {
			ins->opcode = MXOP_PUSH; ins->argc = 1;
			ARGV *s = &ins->argv[0]; s->size = 1; set_immediate(s,fetch_u8());
		} break;
#endif
#if core_level >= 1
		case 0x6B: {
			ins->opcode = MXOP_IMUL; ins->argc = 3;
            ARGV *d = &ins->argv[0];
            ARGV *s = &ins->argv[1];
            ARGV *i = &ins->argv[2];
            d->size = data32wordsize;
            s->size = data32wordsize;
            i->size = 1;
            INS_MRM mrm=decode_rm_(s,ins,d->size,PLUSR_TRANSFORM);
            set_register(d,mrm.f.reg);
            set_immediate(i,(native_int_t)((signed char)fetch_u8()));
		} break;
#endif
#if core_level >= 1
		COVER_4(0x6C): {
			ins->opcode = MXOP_INS+((first_byte>>1)&1); ins->argc = 1;
			ARGV *r = &ins->argv[0]; r->size = (first_byte&1)?data32wordsize:1;
			r->regtype = MX86_RT_NONE; r->scalar = 0; r->memregs = 1;
			r->memref_base = 0; r->memregsz = addrwordsize;
			r->memreg[0] = (first_byte & 2) ? MX86_REG_ESI : MX86_REG_EDI;
			r->segment = first_byte & 2 ? (ins->segment >= 0 ? ins->segment : MX86_SEG_DS) : MX86_SEG_ES;
		} break;
#endif
		COVER_ROW(0x70): {
			ins->opcode = MXOP_JO+(first_byte&0xF); ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = addrwordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(mref,((int64_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFFFFFFFFFULL);
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			set_immediate(mref,((int32_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFUL);
#endif
		} break;
		COVER_4(0x80): {
			ARGV *rm=&ins->argv[0],*imm=&ins->argv[1]; rm->size=imm->size=(first_byte&1)?datawordsize:1;
			ins->argc=2; imm->regtype=MX86_RT_IMM; INS_MRM mrm=decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM);
			if (first_byte == 0x83) imm->value = (native_int_t)((signed char)fetch_u8());
			else if (first_byte == 0x81) imm->value = imm32sbysize(ins); else imm->value = fetch_u8();
			ins->opcode = MXOP_ADD+mrm.f.reg;
		} break;
		COVER_2(0x84): {
			ARGV *rm=&ins->argv[0],*reg=&ins->argv[1]; rm->size=reg->size=(first_byte&1)?datawordsize:1;
			INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_register(reg,mrm.f.reg);
			ins->opcode = MXOP_TEST; ins->argc = 2;
		} break;
		COVER_2(0x86): {
			ARGV *rm=&ins->argv[0],*reg=&ins->argv[1]; rm->size=reg->size=(first_byte&1)?datawordsize:1;
			INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_register(reg,mrm.f.reg);
			ins->opcode = MXOP_XCHG; ins->argc = 2; rm->memregsz=addrwordsize;
		} break;
		COVER_4(0x88): {
			const int which=(first_byte>>1)&1; ARGV *rm=&ins->argv[which],*reg=&ins->argv[which^1];
			rm->size=reg->size=(first_byte&1)?datawordsize:1;ins->opcode=MXOP_MOV;ins->argc=2;rm->memregsz=addrwordsize;
			INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_register(reg,mrm.f.reg);
		} break;
		case 0x8C: case 0x8E: { /* mov r/m, seg reg */
			ins->opcode = MXOP_MOV; ins->argc = 2; const int which = (first_byte>>1)&1;
			ARGV *rm = &ins->argv[which],*reg = &ins->argv[which^1]; rm->size = reg->size = 2;
			INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_segment_register(reg,mrm.f.reg);
		} break;
		case 0x8D: { /* LEA reg,mem */
			ins->opcode = MXOP_LEA; ins->argc = 2;
			ARGV *reg = &ins->argv[0],*rm = &ins->argv[1]; rm->size = reg->size = data64wordsize;
			INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_register(reg,mrm.f.reg);
		} break;
		case 0x8F:
			if (*cip >= 0xC0) {
#  if defined(vex_level) || defined(everything)
				union minx86dec_vex v; v.raw = fetch_u16() ^ 0x78E0; ins->vex = v;
				unsigned int vector_size = 16 << (v.f.l?1:0); const uint8_t opcode = fetch_u8();
				ins->oes = opcode & 3;

				switch (v.f.pp) {
					case 0x1: ins->data32 ^= 1; dataprefix32++; break;	/* as if 0x66 prefix */
					case 0x2: ins->rep = MX86_REPNE; break;			/* as if 0xF3 prefix */
					case 0x3: ins->rep = MX86_REPE; break;			/* as if 0xF2 prefix */
				};

				switch (v.f.m) {
/*------------------------------------*/case 0x8: switch (opcode) {
	case 0x85: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMACSSWW,vector_size);
break;	case 0x86: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMACSSWD,vector_size);
break;	case 0x8E: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMACSSDD,vector_size);
break;	case 0x95: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMACSWW,vector_size);
break;	case 0x96: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMACSWD,vector_size);
break;	COVER_2(0x9E): if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMACSDD+(opcode&1),vector_size);
break;	case 0xA2: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPCMOV,vector_size);
break;	case 0xA3: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPPERM,vector_size);
break;	case 0xA6: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMADCSSWD,vector_size);
break;	case 0xB6: if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_it4(MXOP_VPMADCSWD,vector_size);
break;	COVER_4(0xC0): if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l) typical_x86_amd_vex_m_rm_i8(MXOP_VPROTB+(opcode&3),vector_size);
		}
	} break; COVER_4(0xCC): if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_i8(MXOP_VPCOMB+ins->oes,vector_size);
	break;	COVER_4(0xEC):	if (v.f.pp == 0)
		typical_x86_amd_vex_m_v_rm_i8(MXOP_VPCOMUB+ins->oes,vector_size);
	break; } break;
/*------------------------------------*/case 0x9: switch (opcode) {
	COVER_2(0x80): if (v.f.pp == 0) {
		if (v.f.v == 0)
			typical_x86_amd_vex_m_rm(MXOP_VFRCZPS+(opcode&1),vector_size);
} break; COVER_2(0x82): if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l)
				typical_x86_amd_vex_m_rm(MXOP_VFRCZSS+(opcode&1),vector_size);
		}
} break; COVER_4(0x90): if (v.f.pp == 0) {
		if (!v.f.l)
			typical_x86_amd_vex_m_rm_v_wswap(MXOP_VPROTB+(opcode&3),vector_size);
} break; COVER_4(0x94): if (v.f.pp == 0) {
		if (!v.f.l)
			typical_x86_amd_vex_m_rm_v_wswap(MXOP_VPSHLB+(opcode&3),vector_size);
} break; COVER_4(0x98): if (v.f.pp == 0) {
		if (!v.f.l)
			typical_x86_amd_vex_m_rm_v_wswap(MXOP_VPSHAB+(opcode&3),vector_size);
} break; COVER_4(0xC0): if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l) {
				if (opcode != 0xC0)
					typical_x86_amd_vex_m_rm(MXOP_VPHADDBW+(opcode&3)-1,vector_size);
			}
		}
} break; COVER_2(0xC6): if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l)
				typical_x86_amd_vex_m_rm(MXOP_VPHADDWD+(opcode&1),vector_size);
		}
} break; case 0xCB: if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l)
				typical_x86_amd_vex_m_rm(MXOP_VPHADDDQ,vector_size);
		}
} break; COVER_4(0xD0): if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l) {
				if (opcode != 0xD0)
					typical_x86_amd_vex_m_rm(MXOP_VPHADDUBW+(opcode&3)-1,vector_size);
			}
		}
} break; COVER_2(0xD6): if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l)
				typical_x86_amd_vex_m_rm(MXOP_VPHADDUWD+(opcode&1),vector_size);
		}
} break; case 0xDB: if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l)
				typical_x86_amd_vex_m_rm(MXOP_VPHADDUDQ,vector_size);
		}
} break; COVER_4(0xE0): if (v.f.pp == 0) {
		if (v.f.v == 0) {
			if (!v.f.l) {
				if (opcode != 0xE0)
					typical_x86_amd_vex_m_rm(MXOP_VPHSUBBW+(opcode&3)-1,vector_size);
			}
		}
} break; } break;
/*------------------------------------*/}
#  endif
			}
			else {
				ARGV *d = &ins->argv[0]; ins->argc = 1; d->size = stackwordsize;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0: ins->opcode = MXOP_POP; break;
				}
			} break;
		case 0x90: {
			if (core_level >= 6 && ins->rep == MX86_REPNE) ins->opcode = MXOP_PAUSE;
			else ins->opcode = MXOP_NOP;
		} break;
		COVER_4(0x91): COVER_2(0x95): case 0x97: {
			ins->opcode = MXOP_XCHG; ins->argc = 2;
			ARGV *a = &ins->argv[0],*r = &ins->argv[1]; a->size = r->size = datawordsize;
			set_register(r,first_byte & 7); set_register(a,MX86_REG_AX);
		} break;
		case 0x98: ins->opcode = isdata64?MXOP_CDQE:(isdata32?MXOP_CWDE:MXOP_CBW); break;
		case 0x99: ins->opcode = isdata64?MXOP_CQO:(MXOP_CWD+(isdata32&1)); break;
#ifndef x64_mode /* not valid in 64-bit mode */
		case 0x9A: {
			ARGV *mref=&ins->argv[0]; mref->size=mref->memregsz=data32wordsize+2;
			ins->opcode=MXOP_CALL_FAR; ins->argc=1; mref->regtype = MX86_RT_IMM,mref->segment = MX86_SEG_IMM;
			if (isdata32) mref->value = fetch_u32(); else mref->value = fetch_u16(); mref->segval = fetch_u16();
		} break;
#endif
		case 0x9B: {
			if ((cip[0]&0xF8) == 0xD8 || (cip[0]&0xFE) == 0x66) {
				ins->fwait = 1; next_round();
			}
			else { ins->opcode = MXOP_FWAIT; }
		} break;
		case 0x9C: ins->opcode = isdata32?MXOP_PUSHFD:MXOP_PUSHF; break;
		case 0x9D: ins->opcode = isdata32?MXOP_POPFD:MXOP_POPF; break;
		case 0x9E: ins->opcode = MXOP_SAHF; break;
		case 0x9F: ins->opcode = MXOP_LAHF; break;
		COVER_4(0xA0): {
			ins->opcode = MXOP_MOV; ins->argc = 2; const int which = (first_byte>>1)&1;
			ARGV *areg=&ins->argv[which],*mref=&ins->argv[which^1]; mref->memregsz=addrwordsize;
			areg->size=mref->size=(first_byte&1)?datawordsize:1; set_register(areg,MX86_REG_AX);
			mref->segment = (ins->segment >= 0 ? ins->segment : MX86_SEG_DS);
#ifdef x64_mode
			set_mem_ref_imm(mref,isaddr32 ? ((uint64_t)((int32_t)fetch_u32())) : fetch_u64());
#else
			set_mem_ref_imm(mref,isaddr32 ? fetch_u32() : fetch_u16());
#endif
		} break;
		COVER_2(0xA4): string_instruction_typical(MXOP_MOVS); break;
		COVER_2(0xA6): string_instruction_typical(MXOP_CMPS); break;
		COVER_2(0xA8): {
			ARGV *rm=&ins->argv[0],*imm=&ins->argv[1]; rm->size=imm->size=first_byte&1?datawordsize:1;
			ins->opcode = MXOP_TEST; ins->argc = 2; set_register(rm,MX86_REG_EAX);
			if (first_byte & 1) set_immediate(imm,imm32sbysize(ins)); else set_immediate(imm,fetch_u8());
		} break;
		COVER_4(0xAA): COVER_2(0xAE): {
			ins->opcode=MXOP_STOS+((first_byte-0xAA)>>1); ins->argc=1;
			ARGV *r=&ins->argv[0]; r->size=(first_byte&1)?datawordsize:1; r->regtype=MX86_RT_NONE;
			r->segment = first_byte & 2 ? MX86_SEG_ES : (ins->segment >= 0 ? ins->segment : MX86_SEG_DS);
			r->scalar = 0,r->memregs = 1,r->memref_base = 0,r->memregsz = addrwordsize;
			r->memreg[0] = (first_byte & 2) ? MX86_REG_EDI : MX86_REG_ESI;
		} break;
		COVER_ROW(0xB0): {
			ins->opcode = MXOP_MOV; ins->argc = 2;
			ARGV *r = &ins->argv[0],*imm = &ins->argv[1]; r->size = imm->size = (first_byte&8)?datawordsize:1;
			set_immediate(imm,(first_byte & 8) ? imm64bysize(ins) : fetch_u8()); set_register(r,x64_translate_rm3b(r->size,first_byte&7,ins));
		} break;
		COVER_2(0xC0): {
			ARGV *d=&ins->argv[0],*imm=&ins->argv[1]; d->size = (first_byte&1)?datawordsize:1;imm->size=1;
			INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); ins->argc = 2;
			switch (mrm.f.reg) {
				case 0:	ins->opcode = MXOP_ROL; break; case 1: ins->opcode = MXOP_ROR; break;
				case 2:	ins->opcode = MXOP_RCL; break; case 3: ins->opcode = MXOP_RCR; break;
				case 4:	ins->opcode = MXOP_SHL; break; case 5: ins->opcode = MXOP_SHR; break;
				case 7:	ins->opcode = MXOP_SAR; break;
			}; set_immediate(imm,fetch_u8());
		} break;
		COVER_2(0xC2): {
			ins->opcode = MXOP_RET; 
			if ((first_byte&1)==0)
				{ ARGV *imm = &ins->argv[ins->argc++]; imm->size = 2; set_immediate(imm,fetch_u16()); }
		} break;
		COVER_2(0xC4): { /* LDS/LES */
			if ((*cip & 0xC0) == 0xC0) { /* NOPE! AVX/VEX extensions (illegal encoding of LDS/LES) */
#  if (defined(vex_level) || defined(everything))
				union minx86dec_vex v;
				if (first_byte & 1) { v.raw=(fetch_u8()^0x78)<<8; v.f.r=v.f.w; v.f.w=0; v.f.m=1; }
				else v.raw = fetch_u16() ^ 0x78E0;
				unsigned int vector_size = 16 << (v.f.l?1:0); ins->vex = v;
				switch (v.f.pp) {
					case 0x1: ins->data32 ^= 1; dataprefix32++; break;	/* as if 0x66 prefix */
					case 0x2: ins->rep = MX86_REPNE; break;			/* as if 0xF3 prefix */
					case 0x3: ins->rep = MX86_REPE; break;			/* as if 0xF2 prefix */
				};
				switch (v.f.m) {
					case 1: {
						const uint8_t second_byte = fetch_u8(); switch (second_byte) {
						COVER_2(0x12): {
							const uint8_t which = second_byte&1; ins->argc=2;
							ARGV *d=&ins->argv[which],*s=&ins->argv[which^1];
							d->size = vector_size; s->size = vector_size>>1;
							INS_MRM mrm=decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg);
							if (ins->rep == MX86_REPNE) ins->opcode = MXOP_MOVSLDUP;
							else if (ins->rep == MX86_REPE) ins->opcode = MXOP_MOVDDUP;
							else ins->opcode=dataprefix32?MXOP_MOVLPD:MXOP_MOVLPS;
							/* Hm..... so the reg-reg version of MXOP_MOVLPS is MXOP_MOVHLPS? */
							if (ins->opcode==MXOP_MOVLPS && s->regtype!=MX86_RT_NONE && !which) {
								ARGV *sl=&ins->argv[2]; *sl=*s;s->size=sl->size=vector_size;
								ins->opcode=MXOP_MOVHLPS;ins->argc=3;
								set_sse_register(s,v.f.v);
							}
						} break;
						COVER_2(0x28): {
							const uint8_t which = second_byte&1;
							ins->opcode = MXOP_MOVAPS-(dataprefix32&1); ins->argc=2;
							ARGV *re = &ins->argv[which],*rm = &ins->argv[which^1];
							re->size = rm->size = vector_size;
							INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,
								PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(re,mrm.f.reg);
						} break;
						case 0x2A: {
							INS_MRM mrm;
							ARGV *d=&ins->argv[0],*s=&ins->argv[1]; ins->argc=2; d->size=16;
							if (ins->rep >= MX86_REPE) {
								mrm = decode_rm_(s,ins,s->size=4,PLUSR_TRANSFORM);
								ins->opcode=ins->rep==MX86_REPNE?MXOP_CVTSI2SS:MXOP_CVTSI2SD;
							}
							else {
								mrm=decode_rm_ex_(s,ins,s->size=8,
									PLUSR_TRANSFORM,s->regtype=MX86_RT_MMX);
								ins->opcode=MXOP_CVTPI2PS+(dataprefix32&1);
							}
							set_sse_register(d,mrm.f.reg);
						} break;

						case 0x2C: {
							ARGV *d=&ins->argv[0],*s=&ins->argv[1];
							ins->argc=2; s->size=16; s->regtype=MX86_RT_SSE;
							if (ins->rep >= MX86_REPE)
								ins->opcode = MXOP_CVTTSD2SI+ins->rep-MX86_REPE;
							else
								ins->opcode = MXOP_CVTTPS2PI+(dataprefix32&1);
							if (ins->rep >= MX86_REPE)
								{ d->size = 4; d->regtype = MX86_RT_REG; }
							else				
								{ d->size = 8; d->regtype = MX86_RT_MMX; }
							INS_MRM mrm = decode_rm_ex_(s,ins,s->size,
								PLUSR_TRANSFORM,MX86_RT_SSE);
							d->reg = mrm.f.reg;
						} break;
						case 0x2D: {
							ARGV *d=&ins->argv[0],*s=&ins->argv[1];
							s->size=16; ins->argc=2; s->regtype=MX86_RT_SSE; d->size = 4;
							if (ins->rep >= MX86_REPE) {
								ins->opcode =
									(ins->rep==MX86_REPNE?MXOP_CVTSS2SI:MXOP_CVTSD2SI);
								INS_MRM mrm = decode_rm_ex_(s,ins,s->size,
									PLUSR_TRANSFORM,MX86_RT_SSE);
								set_register(d,mrm.f.reg);
							}
							else {
								ins->opcode=MXOP_CVTPS2PI+(dataprefix32&1);
								INS_MRM mrm=decode_rm_ex_(s,ins,s->size,
									PLUSR_TRANSFORM,MX86_RT_SSE); d->size = 8;
								set_mmx_register(d,mrm.f.reg);
							}
						} break;

						case 0x2F: {
							if (v.f.v == 0) {
								if (!v.f.l) {
									ARGV *d=&ins->argv[0],*s=&ins->argv[1];
									ins->opcode=MXOP_COMISS+(dataprefix32?1:0);
									ins->argc = 2; d->size = s->size = (dataprefix32?8:4);
									INS_MRM mrm = decode_rm_ex_(s,ins,s->size,
										PLUSR_TRANSFORM,MX86_RT_SSE);
									set_sse_register(d,mrm.f.reg);
								}
							}
						} break;

						COVER_2(0x54): {
							ARGV *d = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
							ins->opcode = MXOP_ANDPS+dataprefix32+((second_byte&1)<<1);
							d->size = s1->size = s2->size = vector_size; ins->argc = 3;
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
								PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(s1,v.f.v); set_sse_register(d,mrm.f.reg);
						} break;

						case 0x58: {
							if (v.f.l && ins->rep != 0) {
								/* forms involving ADDSS/ADDSD not supported with L=1 */
							}
							else {
								ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2];
								ins->opcode=(ins->rep>=MX86_REPE?(2+ins->rep-MX86_REPE):
									dataprefix32)+MXOP_ADDPS;
								d->size=s1->size=s2->size=vector_size; ins->argc=3;
								INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
									PLUSR_TRANSFORM,MX86_RT_SSE);
								set_sse_register(s1,v.f.v); set_sse_register(d,mrm.f.reg);
							}
						} break;

						case 0x5A: {
							ARGV *d=&ins->argv[0],*s=&ins->argv[1]; ins->argc = 2;
							if (ins->rep >= MX86_REPE) {
								ins->opcode=ins->rep==MX86_REPNE?MXOP_CVTSS2SD:MXOP_CVTSD2SS;
								d->size = 16; s->size = 16;
							}
							else {
								ins->opcode = MXOP_CVTPS2PD+(dataprefix32&1);
								s->size=(dataprefix32&1)==1?vector_size:16;
								d->size=(dataprefix32&1)==0?vector_size:16;
							}
							INS_MRM mrm=decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg);
						} break;
						case 0x5B: {
							ARGV *d = &ins->argv[0],*s = &ins->argv[1];
							ins->argc = 2; d->size = s->size = vector_size;
							if (ins->rep == MX86_REPNE) ins->opcode = MXOP_CVTTPS2DQ;
							else if (ins->rep == MX86_REPE)	break; /* not defined */
							else ins->opcode = MXOP_CVTDQ2PS+(dataprefix32&1);
							INS_MRM mrm=decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg);
						} break;

						case 0x5D: {
							ins->opcode=(ins->rep>=MX86_REPE)?
								(ins->rep+MXOP_MINSD-MX86_REPE):(MXOP_MINPS-dataprefix32);
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2];
							ins->argc=3; s1->size=s2->size=d->size=vector_size; 
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
								PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(s1,v.f.v); set_sse_register(d,mrm.f.reg);
						} break;
						case 0x5E: {
							ARGV *d = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
							ins->argc=3; d->size=s1->size=s2->size=vector_size;
							if (ins->rep >= MX86_REPE) ins->opcode=MXOP_DIVSD+ins->rep-MX86_REPE;
							else if (dataprefix32) ins->opcode=MXOP_DIVPD;
							else ins->opcode = MXOP_DIVPS;
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
								PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
						} break;
						case 0x5F: {
							if (v.f.l && ins->rep != 0) {
								/* forms involving ADDSS/ADDSD not supported with L=1 */
							}
							else {
								ins->opcode=(ins->rep>=MX86_REPE)?
									(ins->rep+MXOP_MAXSD-MX86_REPE):
									(MXOP_MAXPS-dataprefix32);
								ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2];
								ins->argc=3; s1->size=s2->size=d->size=vector_size;
								INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
									PLUSR_TRANSFORM,MX86_RT_SSE);
								set_sse_register(s1,v.f.v); set_sse_register(d,mrm.f.reg);
							}
						} break;

						case 0x6E: case 0x7E: {
							if (ins->rep >= MX86_REPE) {
								if (second_byte == 0x7E && ins->rep == MX86_REPNE) {
									ins->opcode = MXOP_MOVQ; ins->argc = 2;
									ARGV *re = &ins->argv[0],*rm = &ins->argv[1];
									rm->size = re->size = vector_size;
									INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,
										PLUSR_TRANSFORM,MX86_RT_SSE);
									set_sse_register(re,mrm.f.reg);
								}
							}
							else {
								const unsigned int which = (second_byte >> 4) & 1;
								ARGV *re = &ins->argv[which],*rm = &ins->argv[which^1];
								ins->opcode = MXOP_MOVD; ins->argc = 2; rm->size = 4;
								re->size = vector_size;
								INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM);
								set_sse_register(re,mrm.f.reg);
							}
						} break;
						case 0x6F: case 0x7F: {
							if (ins->rep == MX86_REPE) {
							}
							else {
								const unsigned int which = (second_byte >> 4) & 1;
								ARGV *d = &ins->argv[which],*s = &ins->argv[which^1];
								INS_MRM mrm; ins->argc = 2;
								ins->opcode = (ins->rep >= MX86_REPE) ? MXOP_MOVDQU :
									(dataprefix32 ? MXOP_MOVDQA : MXOP_MOVQ);
								if (dataprefix32 || ins->rep >= MX86_REPE) {
									s->size = d->size = vector_size;
									mrm = decode_rm_ex_(s,ins,s->size,
										PLUSR_TRANSFORM,MX86_RT_SSE);
									set_sse_register(d,mrm.f.reg);
								}
								else {
									s->size = d->size = 8;
									mrm = decode_rm_ex_(s,ins,s->size,
										PLUSR_TRANSFORM,MX86_RT_MMX);
									set_mmx_register(d,mrm.f.reg);
								}
							}
						} break;

						COVER_2(0x7C): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2];
							ins->argc=3; d->size=s1->size=s2->size=vector_size;
							if (dataprefix32)
								ins->opcode=(second_byte&1)?MXOP_HSUBPD:MXOP_HADDPD;
							else if (ins->rep == MX86_REPE)
								ins->opcode=(second_byte&1)?MXOP_HSUBPS:MXOP_HADDPS;
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
								PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
						} break;

						case 0xC2: {
							if (v.f.l && ins->rep != 0) {
								/* forms involving CMPSS/CMPSD not supported with L=1 */
							}
							else {
								ins->opcode=(ins->rep>=MX86_REPE)?
									(MXOP_CMPSD+ins->rep-MX86_REPE):
									(MXOP_CMPPS+(dataprefix32?1:0));
								ARGV *d=&ins->argv[0],*s1=&ins->argv[1],
								     *s2=&ins->argv[2],*i=&ins->argv[3];
								ins->argc=4; i->size=8; d->size=s1->size=s2->size=vector_size;
								INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
									PLUSR_TRANSFORM,MX86_RT_SSE);
								set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
								set_immediate(i,fetch_u8());
							}
						} break;

						case 0xD0: {
							ARGV *d = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
							unsigned char t = dataprefix32 + (ins->rep << 2),m = 0;
							ins->argc = 3; d->size = s1->size = s2->size = vector_size;
							switch (t) {
#define PAIR(d,r)  ((d) + ((r) << 2))
	case PAIR(1,MX86_REP_NONE): ins->opcode=MXOP_ADDSUBPD;m=1;break;
	case PAIR(0,MX86_REPE):	ins->opcode=MXOP_ADDSUBPS;m=1;break;
#undef PAIR
							};
							switch (m) {
								case 1: { /* ADDSUBPD/S */
						INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
								} break;
							}
						} break;

						case 0xD6: {
							if (ins->rep >= MX86_REPE) {
								const unsigned int which = (ins->rep - MX86_REPE)&1;
								ARGV *d = &ins->argv[0],*s = &ins->argv[1];
								ins->opcode = which ? MXOP_MOVQ2DQ : MXOP_MOVDQ2Q;
								ins->argc = 2; d->size = 8; s->size = 16;
								INS_MRM mrm = decode_rm_ex_(s,ins,s->size,
									PLUSR_TRANSFORM,which ? MX86_RT_MMX : MX86_RT_SSE);
								if (which) set_sse_register(d,mrm.f.reg);
								else set_mmx_register(d,mrm.f.reg);
							}
							else if (dataprefix32) {
								ARGV *d = &ins->argv[1],*s = &ins->argv[0];
								ins->opcode=MXOP_MOVQ; ins->argc=2; d->size=8; s->size=16;
								INS_MRM mrm = decode_rm_ex_(s,ins,s->size,
									PLUSR_TRANSFORM,MX86_RT_SSE);
								set_sse_register(d,mrm.f.reg);
							}
						} break;

						case 0xE6: {
							ARGV *d = &ins->argv[0],*s = &ins->argv[1];
							ins->argc = 2; d->size = s->size = vector_size;
							if (ins->rep >= MX86_REPE) {
								ins->opcode = MXOP_CVTPD2DQ + ins->rep - MX86_REPE;
								if (ins->rep == MX86_REPE) d->size >>= 1;
								else s->size >>= 1;
							}
							else if (dataprefix32)
								ins->opcode = MXOP_CVTTPD2DQ;
							else
								break;
							INS_MRM mrm=decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg);
						} break;

						case 0xF0: {
							ARGV *d = &ins->argv[0],*s = &ins->argv[1];
							ins->opcode=MXOP_LDDQU; ins->argc=2; d->size=s->size=vector_size;
							INS_MRM mrm=decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
							set_sse_register(d,mrm.f.reg);
						} break;
					} break; }

					case 2: { /* 0x0F 0x38 */
						const uint8_t third_byte = fetch_u8(); switch (third_byte) {
						case 0x18: { 
							if (dataprefix32) {
								if (v.f.v == 0) {/* VBROADCASTSS */
									ARGV *d=&ins->argv[0],*s=&ins->argv[1];
									ins->opcode=MXOP_BROADCASTSS; ins->argc=2;
									d->size=vector_size; s->size=4;
									INS_MRM mrm = decode_rm_(s,ins,s->size,
										PLUSR_TRANSFORM);
									set_sse_register(d,mrm.f.reg);
								}
							}
						} break;
						case 0x19: { 
							if (dataprefix32) {
								if (v.f.l) {
									if (v.f.v == 0) {/* VBROADCASTSD */
										ARGV *d=&ins->argv[0],
										     *s=&ins->argv[1];
										ins->opcode = MXOP_BROADCASTSD;
										ins->argc=2; d->size=32; s->size=8;
										INS_MRM mrm = decode_rm_(s,ins,
											s->size,PLUSR_TRANSFORM);
										set_sse_register(d,mrm.f.reg);
									}
								}
							}
						} break;
						case 0x1A: { 
							if (dataprefix32) {
								if (v.f.l) {
									if (v.f.v == 0) {/* VBROADCASTF128 */
										ARGV *d=&ins->argv[0],
										     *s=&ins->argv[1];
										ins->opcode = MXOP_BROADCASTF128;
										ins->argc=2; d->size=32; s->size=16;
										INS_MRM mrm = decode_rm_(s,ins,
											s->size,PLUSR_TRANSFORM);
										set_sse_register(d,mrm.f.reg);
									}
								}
							}
						} break;

						case 0xDB: { /* AES IMC */
							if (dataprefix32) {
								ins->argc=2; ARGV *d=&ins->argv[0],*s2=&ins->argv[1];
								d->size=s2->size = 16; ins->opcode = MXOP_AESIMC;
								INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
									PLUSR_TRANSFORM,MX86_RT_SSE);
								set_sse_register(d,mrm.f.reg);
							}
						} break;
						COVER_4(0xDC): { /* AES ENC/DEC */
							if (dataprefix32) {
								ARGV *d=&ins->argv[0],*s1=&ins->argv[1],
								     *s2=&ins->argv[2]; ins->argc = 3;
								d->size = s1->size = s2->size = vector_size;
								ins->opcode = MXOP_AESENC + (third_byte & 3);
								INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,
									PLUSR_TRANSFORM,MX86_RT_SSE);
								set_sse_register(d,mrm.f.reg);
								set_sse_register(s1,v.f.v);
							}
						} break;
					} break; }

					case 3: { /* 0x0F 0x3A */
						const uint8_t third_byte = fetch_u8(); switch (third_byte) {
						COVER_2(0x0C): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],
							     *s2=&ins->argv[2],*i=&ins->argv[3]; ins->argc=4; i->size=1;
							d->size=s1->size=s2->size=vector_size;
							INS_MRM mrm=decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); ins->opcode=MXOP_BLENDPS+(third_byte&1);
							set_sse_register(d,mrm.f.reg); set_immediate(i,fetch_u8());
							set_sse_register(s1,v.f.v);
						} break;

						COVER_2(0x40): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],
							     *s2=&ins->argv[2],*i=&ins->argv[3]; ins->argc=4; i->size=1;
							d->size=s1->size=s2->size=16; ins->opcode=MXOP_DPPS+(third_byte&1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); set_immediate(i,fetch_u8());
							set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
						} break;

						COVER_2(0x48): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],
							     *s2=&ins->argv[2],*s3=&ins->argv[3],*s4 = &ins->argv[4];
							ins->argc=5; d->size=s1->size=s2->size=s3->size=vector_size;
							ins->opcode = MXOP_VPERMIL2PS+(third_byte&1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t imm8 = fetch_u8(); 
							set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
							set_sse_register(s3,imm8>>4); set_immediate(s4,imm8&0xF);
						} break;
						COVER_2(0x4A): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_BLENDVPS+(third_byte&1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); set_sse_register(d,mrm.f.reg);
							uint8_t imm8 = fetch_u8(); set_sse_register(s3,imm8>>4);
							set_sse_register(s1,v.f.v);
						} break;

						COVER_2(0x5C): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_VFMADDSUBPS + (third_byte & 1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t c = fetch_u8();
							set_sse_register(s3,c>>4); set_sse_register(s1,v.f.v);
							set_sse_register(d,mrm.f.reg);
						} break;
						COVER_2(0x5E): { /* FIXME: untested */
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_VFMSUBADDPS + (third_byte & 1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t c = fetch_u8();
							set_sse_register(s3,c>>4); set_sse_register(s1,v.f.v);
							set_sse_register(d,mrm.f.reg);
						} break;

						COVER_2(0x68): { /* UNTESTED */
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_VFMADDPS + (third_byte & 1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t c = fetch_u8();
							set_sse_register(s3,c>>4); set_sse_register(s1,v.f.v);
							set_sse_register(d,mrm.f.reg);
						} break;
						COVER_2(0x6A): {
							if (v.f.l) {
								/* illegal */
							}
							else { /* UNTESTED */
								ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
								  *s3=&ins->argv[3]; ins->opcode=MXOP_VFMADDSS+(third_byte&1);
								d->size=s1->size=s2->size=s3->size=vector_size; ins->argc=4;
								INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
									MX86_RT_SSE); uint8_t c = fetch_u8();
								set_sse_register(s3,c>>4); set_sse_register(s1,v.f.v);
								set_sse_register(d,mrm.f.reg);
							}
						} break;
						COVER_2(0x6C): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_VFMSUBPS + (third_byte & 1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t c = fetch_u8();
							set_sse_register(s3,c>>4); set_sse_register(s1,v.f.v);
							set_sse_register(d,mrm.f.reg);
						} break;
						COVER_2(0x6E): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_VFMSUBSS + (third_byte & 1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t c = fetch_u8();
							set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
							set_sse_register(s3,c>>4);
						} break;

						COVER_2(0x78): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_VFNMADDPS + (third_byte & 1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t c = fetch_u8();
							set_sse_register(d,mrm.f.reg); set_sse_register(s1,v.f.v);
							set_sse_register(s3,c>>4);
						} break;
						COVER_2(0x7C): {
							ARGV *d=&ins->argv[0],*s1=&ins->argv[1],*s2=&ins->argv[2],
							     *s3=&ins->argv[3];d->size=s1->size=s2->size=s3->size=vector_size;
							ins->argc=4; ins->opcode = MXOP_VFNMSUBPS + (third_byte & 1);
							INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,
								MX86_RT_SSE); uint8_t c = fetch_u8(); 
							set_sse_register(s3,c>>4); set_sse_register(d,mrm.f.reg);
							set_sse_register(s1,v.f.v);
						} break;

						case 0xDF: {
							ARGV *d = &ins->argv[0],*s = &ins->argv[1],*i = &ins->argv[2];
							ins->opcode=MXOP_AESKEYGENASSIST; ins->argc=3; d->size=s->size=16;
							INS_MRM mrm=decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
							i->size=1; set_immediate(i,fetch_u8()); set_sse_register(d,mrm.f.reg);
						} break;
					} break; }
				}
#  endif
			}
#  ifndef x64_mode
			else { /* LDS/LES not valid in x86-64 mode */
				ARGV *d=&ins->argv[0],*s=&ins->argv[1]; d->size=data32wordsize; s->size=data32wordsize+2;
				INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
				ins->opcode = (first_byte & 1) ? MXOP_LDS : MXOP_LES; ins->argc = 2;
			}
#  endif
		} break;
		COVER_2(0xC6): {
			ARGV *d=&ins->argv[0],*s=&ins->argv[1]; s->size=d->size=(first_byte&1)?datawordsize:1;
			ins->argc = 2; INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
			switch (mrm.f.reg) {
				case 0: ins->opcode = MXOP_MOV; switch (d->size) {
					case 1:	set_immediate(s,fetch_u8()); break;
					case 2:	set_immediate(s,fetch_u16()); break;
					case 4:	set_immediate(s,fetch_u32()); break;
					case 8: set_immediate(s,(uint64_t)((int32_t)fetch_u32())); break;
				} break;
			}
		} break;
#if core_level >= 1
		COVER_2(0xC8): {
			ins->opcode = MXOP_ENTER+(first_byte&1); ins->argc = (first_byte == 0xC8)?2:0;
			if (ins->argc) {
				ARGV *a = &ins->argv[0],*b = &ins->argv[1]; a->size = 16; b->size = 8;
				set_immediate(a,fetch_u16()); set_immediate(b,fetch_u8());
			}
		} break;
#endif
		COVER_2(0xCA): {
			ins->opcode = MXOP_RETF; ins->argv[1].size = 2;
			if ((first_byte&1)==0) set_immediate(&ins->argv[ins->argc++],fetch_u16());
		} break;
		case 0xCC: ins->opcode = MXOP_INT; ins->argc = 1; set_immediate(&ins->argv[0],3); break;
		case 0xCD: ins->opcode = MXOP_INT; ins->argc = 1; set_immediate(&ins->argv[0],fetch_u8()); break;
#ifndef x64_mode /* not valid in 64-bit mode */
		case 0xCE: ins->opcode = MXOP_INTO; break;
#endif
		case 0xCF: ins->opcode = isdata32 ? MXOP_IRETD : MXOP_IRET; break;
		COVER_4(0xD0): {
			ins->argc=2; ARGV *d=&ins->argv[0],*imm=&ins->argv[1];
			d->size = (first_byte&1)?datawordsize:1; imm->size=1;
			INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
			if (first_byte&2) set_register(imm,MX86_REG_CL); else set_immediate(imm,1);
			switch (mrm.f.reg) {
				case 0:	ins->opcode = MXOP_ROL; break; case 1: ins->opcode = MXOP_ROR; break;
				case 2:	ins->opcode = MXOP_RCL; break; case 3: ins->opcode = MXOP_RCR; break;
				case 4:	ins->opcode = MXOP_SHL; break; case 5: ins->opcode = MXOP_SHR; break;
				case 7:	ins->opcode = MXOP_SAR; break;
			};
		} break;
#ifndef x64_mode /* not valid in 64-bit mode */
		COVER_2(0xD4): {
			ins->opcode = MXOP_AAM+(first_byte&1); ins->argc = 1;
			ARGV *im = &ins->argv[0]; im->size = 1; set_immediate(im,fetch_u8());
		} break;
#endif
#if !defined(no_salc)
		case 0xD6:
# if !defined(x64_mode) && (defined(umc) || defined(everything))
			if (ins->segment == MX86_SEG_FS) { ins->opcode = MXOP_UMC_IDENT; } else
# endif
			{ ins->opcode = MXOP_SALC; } break;
#endif
		case 0xD7: ins->opcode = MXOP_XLAT; break;

		case 0xE0: {
			ins->opcode = MXOP_LOOPNE; ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = addrwordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(mref,((int64_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFFFFFFFFFULL);
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			set_immediate(mref,((int32_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFUL);
#endif
		} break;
		case 0xE1: {
			ins->opcode = MXOP_LOOPE; ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = addrwordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(mref,((int64_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFFFFFFFFFULL);
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			set_immediate(mref,((int32_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFUL);
#endif
		} break;
		case 0xE2: {
			ins->opcode = MXOP_LOOP; ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = addrwordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(mref,((int64_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFFFFFFFFFULL);
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			set_immediate(mref,((int32_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFUL);
#endif
		} break;
		case 0xE3: {
			ins->opcode = MXOP_JCXZ; ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = addrwordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(mref,((int64_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFFFFFFFFFULL);
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			set_immediate(mref,((int32_t)((int8_t)fetch_u8()) + curp + 1) & 0xFFFFFFFFUL);
#endif
		} break;
		COVER_2(0xE4): { ins->argc=2;
			ARGV *d=&ins->argv[0],*s=&ins->argv[1]; d->size=(first_byte&1)?datawordsize:1;
			set_register(d,MX86_REG_AX); set_immediate(s,fetch_u8()); ins->opcode=MXOP_IN;
		} break;
		COVER_2(0xE6): { ins->opcode=MXOP_OUT; ins->argc=2;
			ARGV *ioport=&ins->argv[0],*reg=&ins->argv[1];set_immediate(ioport,fetch_u8());
			reg->size=(first_byte&1)?data32wordsize:1; set_register(reg,MX86_REG_AX);
		} break;
		case 0xE8: {
			ins->opcode = MXOP_CALL; ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = mref->memregsz = datawordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(mref,((int32_t)fetch_u32() + curp + 4) & 0xFFFFFFFFFFFFFFFFULL);
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			if (isdata32)	set_immediate(mref,(fetch_u32() + curp + 4) & 0xFFFFFFFFUL);
			else		set_immediate(mref,(fetch_u16() + curp + 2) & 0x0000FFFFUL);
#endif
		} break;
		case 0xE9: {
			ins->opcode = MXOP_JMP; ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = mref->memregsz = datawordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(mref,((int32_t)fetch_u32() + curp + 4) & 0xFFFFFFFFFFFFFFFFULL);
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			if (isdata32)	set_immediate(mref,(fetch_u32() + curp + 4) & 0xFFFFFFFFUL);
			else		set_immediate(mref,(fetch_u16() + curp + 2) & 0x0000FFFFUL);
#endif
		} break;
#ifndef x64_mode /* not valid in 64-bit mode */
		case 0xEA: {
			ins->opcode = MXOP_JMP_FAR; ins->argc = 1;
			ARGV *mref = &ins->argv[0]; mref->size = mref->memregsz = data32wordsize + 2;
			mref->regtype = MX86_RT_IMM; mref->segment = MX86_SEG_IMM;
			if (isdata32) mref->value = fetch_u32(); else mref->value = fetch_u16();
			mref->segval = fetch_u16();
		} break;
#endif
		case 0xEB: {
			ins->opcode = MXOP_JMP; ins->argc = 1;
			ARGV *r = &ins->argv[0]; r->size = addrwordsize;
#ifdef x64_mode
			uint64_t curp = state->ip_value + (uint64_t)(cip - state->read_ip);
			set_immediate(r,curp + 1 + ((uint64_t)((char)fetch_u8())));
#else
			uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
			set_immediate(r,curp + 1 + ((uint32_t)((char)fetch_u8())));
#endif
		} break;
		COVER_2(0xEC): {
			ins->opcode = MXOP_IN; ins->argc = 2;
			ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = (first_byte & 1) ? data32wordsize : 1,s->size = 2;
			set_register(d,MX86_REG_AX); set_register(s,MX86_REG_DX);
		} break;
		COVER_2(0xEE): {
			ins->opcode = MXOP_OUT; ins->argc = 2;
			ARGV *rdx = &ins->argv[0],*rax = &ins->argv[1]; rax->size = (first_byte & 1) ? data32wordsize : 1;
			rdx->size = 2; set_register(rax,MX86_REG_AX); set_register(rdx,MX86_REG_DX);
		} break;
		case 0xF0:
			ins->lock = 1; next_round();
#if (core_level == 3 || core_level == 4) && (defined(am386) || defined(am486))
		case 0xF1: ins->opcode = MXOP_SMI; break;
#elif !defined(no_icebp) && (core_level >= 3) && !defined(x64_mode)
		case 0xF1: ins->opcode = MXOP_ICEBP; break;
#endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
		case 0xF1: { /* BRKS <int> */
			ins->opcode = MXOP_BRKS; ins->argc = 1;
			ARGV *r = &ins->argv[0]; set_immediate(r,fetch_u8());
		} break;
# endif
		COVER_2(0xF2):
			ins->rep = (first_byte & 1) + MX86_REPE; next_round();
		case 0xF4: ins->opcode = MXOP_HLT; break;
		case 0xF5: ins->opcode = MXOP_CMC; break;
		COVER_2(0xF6): {
			ARGV *where=&ins->argv[0],*imm=&ins->argv[1]; where->regtype = MX86_RT_NONE;
			where->size=where->memregsz=imm->size=(first_byte&1)?datawordsize:1; ins->argc = 1;
			INS_MRM mrm = decode_rm_(where,ins,where->size,PLUSR_TRANSFORM);
			static int map_f6[8] = {MXOP_TEST,MXOP_UD,MXOP_NOT,MXOP_NEG, MXOP_MUL,MXOP_IMUL,MXOP_DIV,MXOP_IDIV};
			ins->opcode = map_f6[mrm.f.reg&7]; ins->argc = 1;
			if (mrm.f.reg == 0) { ins->argc++;set_immediate(imm,(first_byte&1)?imm32sbysize(ins):fetch_u8()); }
		} break;
		case 0xF8: ins->opcode = MXOP_CLC; break;
		case 0xF9: ins->opcode = MXOP_STC; break;
		case 0xFA: ins->opcode = MXOP_CLI; break;
		case 0xFB: ins->opcode = MXOP_STI; break;
		case 0xFC: ins->opcode = MXOP_CLD; break;
		case 0xFD: ins->opcode = MXOP_STD; break;
		COVER_2(0xFE): {
			ARGV *where=&ins->argv[0];where->size=(first_byte&1)?datawordsize:1;where->memregsz=addrwordsize;ins->argc=1;
			INS_MRM mrm = decode_rm_(where,ins,where->size,PLUSR_TRANSFORM);
			switch (mrm.f.reg) {
				case 0:	ins->opcode = MXOP_INC;	break; case 1: ins->opcode = MXOP_DEC;	break;
				case 2: case 3: { if (((mrm.f.reg&1)&&(mrm.f.mod==3))||(first_byte&1)==0) break;
					ins->opcode = MXOP_CALL + (mrm.f.reg & 1);
					where->size += (mrm.f.reg & 1) ? 2 : 0;
				} break;
				case 4: case 5: { if (((mrm.f.reg&1)&&(mrm.f.mod==3))||(first_byte&1)==0) break;
					ins->opcode = MXOP_JMP + (mrm.f.reg & 1);
					where->size += (mrm.f.reg & 1) ? 2 : 0;
				} break;
				case 6: case 7: {
					ins->opcode = MXOP_PUSH + (mrm.f.reg & 1);
				} break;
			}
		} break;

/*---------------------------------------------------------------------------------------------------------*/
#if core_level > 0
		/* extended opcode escape */
		case 0x0F: {
			const uint8_t second_byte = fetch_u8(); switch (second_byte) {
# if core_level >= 2
			case 0x00: { /* LLDT */
				ARGV *m = &ins->argv[0]; ins->argc = 1; m->size = 2;
				INS_MRM mrm = decode_rm_(m,ins,m->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_SLDT; break;
					case 1: ins->opcode = MXOP_STR; break;
					case 2: ins->opcode = MXOP_LLDT; break;
					case 3: ins->opcode = MXOP_LTR; break;
					case 4: case 5: ins->opcode = MXOP_VERR + mrm.f.reg - 4; break;
# if defined(everything) && !defined(x64_mode)
					case 6: ins->opcode = MXOP_JMPX; break; /* Merced Itanium "jump to IA64" */
# endif
				}
			} break;
# endif
# if core_level >= 2
			case 0x01: { /* LGDT. illegal encodings are used for newer instructions */
				if (0) { } /* this makes the else statement below valid */
#  if core_level >= 6
				else if (*cip == 0xC8) { ins->opcode = MXOP_MONITOR; cip++; }
				else if (*cip == 0xC9) { ins->opcode = MXOP_MWAIT; cip++; }
#   if sse_level >= 1 /* SSE instructions */
				else if (*cip == 0xD0) { ins->opcode = MXOP_XGETBV; cip++; }
				else if (*cip == 0xD1) { ins->opcode = MXOP_XSETBV; cip++; }
#   endif
				else if (*cip == 0xD8) { ins->opcode = MXOP_VMRUN; ins->argc = 1; cip++;
					ins->argv[0].size = 4; set_register(&ins->argv[0],MX86_REG_EAX); }
				else if (*cip == 0xD9) { ins->opcode = MXOP_VMMCALL; cip++; }
				else if (*cip == 0xDA) { ins->opcode = MXOP_VMLOAD; ins->argc = 1; cip++;
					ins->argv[0].size = 4; set_register(&ins->argv[0],MX86_REG_EAX); }
				else if (*cip == 0xDB) { ins->opcode = MXOP_VMSAVE; cip++; }
				else if (*cip == 0xDC) { ins->opcode = MXOP_STGI; cip++; }
				else if (*cip == 0xDD) { ins->opcode = MXOP_CLGI; cip++; }
				else if (*cip == 0xDE) { ins->opcode = MXOP_SKINIT; ins->argc = 1; cip++;
					ins->argv[0].size = 4; set_register(&ins->argv[0],MX86_REG_EAX); }
				else if (*cip == 0xDF) { ins->opcode = MXOP_INVLPGA; ins->argc = 2; cip++;
					ins->argv[0].size = ins->argv[1].size = 4;
					set_register(&ins->argv[0],MX86_REG_EAX); set_register(&ins->argv[1],MX86_REG_ECX); }
				else if (*cip == 0xF9) { ins->opcode = MXOP_RDTSCP; cip++; }
				else if (*cip >= 0xC1 && *cip <= 0xC3) { ins->opcode = MXOP_VMCALL + *cip - 0xC1; cip++; }
				else if (*cip == 0xC4) { ins->opcode = MXOP_VMXOFF; cip++; }
#   if defined(x64_mode) || defined(everything)
				else if (*cip == 0xF8) { ins->opcode = MXOP_SWAPGS; cip++; }
#   endif
#  endif
				else {
					ARGV *m = &ins->argv[0]; ins->argc = 1;
					INS_MRM mrm = decode_rm_(m,ins,m->size,PLUSR_TRANSFORM);
					switch (mrm.f.reg) {
						case 0: case 1: case 2: case 3: m->size=6; if (mrm.f.mod==3) break;
							ins->opcode=((mrm.f.reg&2)?MXOP_LGDT:MXOP_SGDT)+(mrm.f.reg&1); break;
						case 4:	ins->opcode = MXOP_SMSW; ins->argc = 1; m->size = 2; break;
						case 6:	ins->opcode = MXOP_LMSW; m->size = 2; break;
						case 7: ins->opcode = MXOP_INVLPG; m->size = 4; break;
					}
				}
			} break;
# endif
# if core_level >= 2
			case 0x02: { /* LAR */
				ARGV *d=&ins->argv[0],*s=&ins->argv[1]; d->size=s->size=data32wordsize;
				ins->opcode=MXOP_LAR; ins->argc=2; INS_MRM mrm=decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 2
			case 0x03: { /* LSL */
				ARGV *d=&ins->argv[0],*s=&ins->argv[1]; d->size=s->size=data32wordsize;
				ins->opcode=MXOP_LSL; ins->argc=2; INS_MRM mrm=decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
			} break;
# endif
# if core_level == 2 && !defined(everything) /* secret unknown opcode said to cause 286s to hang, LOADALL alias? */
			case 0x04: ins->opcode = MXOP_UNKNOWN_286_0F04; break;
# endif
# if core_level == 2 || (defined(everything) && core_level > 2 && core_level <= 4)
			case 0x05: ins->opcode = MXOP_LOADALL_286; break;
# endif
# if core_level >= 5 && (defined(pentiumpro) || pentium >= 2)
			case 0x05: ins->opcode = MXOP_SYSCALL; break;
# endif
# if core_level >= 2
			case 0x06: ins->opcode = MXOP_CLTS; break;
# endif
# if core_level == 3 && defined(am386)
			case 0x07: ins->opcode = MXOP_RES3; break;
# elif core_level == 4 && defined(am486)
			case 0x07: ins->opcode = MXOP_RES4; break;
# elif core_level == 4 && ibmslc_level == 1 /* TODO: I'm guessing the 486SLC doesn't support it */
			/* nothing */
# elif core_level == 4 && ibmslc_level == 2 /* IBM 486SLC2 */
			case 0x07: ins->opcode = MXOP_ICERET; break;
# elif core_level == 3 || (defined(everything) && core_level == 4)
			case 0x07: ins->opcode = MXOP_LOADALL_386; break;
# endif
# if core_level >= 5 && (defined(pentiumpro) || pentium >= 2)
			case 0x07: ins->opcode = MXOP_SYSRET; break;
# endif
# if core_level >= 4
			case 0x08: ins->opcode = MXOP_INVD; break;
			case 0x09: ins->opcode = MXOP_WBINVD; break;
# endif

# if core_level >= 5 && sse_level >= 1
			case 0x0B: ins->opcode = MXOP_UD2; break;
# endif

# if (core_level >= 5 && amd_3dnow >= 1) || defined(everything)
			case 0x0D: {
				ARGV *rm = &ins->argv[0]; rm->size = 4; ins->argc = 1;
				INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM);
				if (mrm.f.mod == 3) break; /* register-only version is illegal */
				switch (mrm.f.reg) {
					case 0: ins->opcode = MXOP_PREFETCH; break;
					case 1: ins->opcode = MXOP_PREFETCHW; break;
				}
			} break;
# endif
# if (core_level >= 5 && amd_3dnow >= 1) || defined(everything)
			case 0x0E: ins->opcode = MXOP_FEMMS; break;
# endif
# if (core_level >= 5 && amd_3dnow >= 1) || defined(everything)
			case 0x0F: { /* AMD 3DNow instructions */
				ARGV *reg = &ins->argv[0],*rm = &ins->argv[1];
				ins->argc = 2; rm->size = reg->size = 8;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(reg,mrm.f.reg);
				const uint8_t suffix = fetch_u8();
				switch (suffix) {
#  if (core_level >= 5 && amd_3dnow >= 2) || defined(everything) /* 3dnow+ aka enhanced, etc. whatever you call it */
					case 0x0C: ins->opcode = MXOP_PI2FW; break;
#  endif
					case 0x0D: ins->opcode = MXOP_PI2FD; break;
#  if (core_level >= 5 && amd_3dnow >= 2) || defined(everything) /* 3dnow+ aka enhanced, etc. whatever you call it */
					case 0x1C: ins->opcode = MXOP_PF2IW; break;
#  endif
					case 0x1D: ins->opcode = MXOP_PF2ID; break;

#  if (core_level >= 5 && amd_3dnow >= 3) || defined(everything) /* Geode */
					case 0x86: ins->opcode = MXOP_PFRCPV; break;
					case 0x87: ins->opcode = MXOP_PFRSQRTV; break;
#  endif
#  if (core_level >= 5 && amd_3dnow >= 2) || defined(everything) /* 3dnow+ aka enhanced, etc. whatever you call it */
					case 0x8A: ins->opcode = MXOP_PFNACC; break;
					case 0x8E: ins->opcode = MXOP_PFPNACC; break;
#  endif
					case 0x90: ins->opcode = MXOP_PFCMPGE; break;
					case 0x94: ins->opcode = MXOP_PFMIN; break;
					case 0x96: ins->opcode = MXOP_PFRCP; break;
					case 0x97: ins->opcode = MXOP_PFRSQRT; break;
					case 0x9A: ins->opcode = MXOP_PFSUB; break;
					case 0x9E: ins->opcode = MXOP_PFADD; break;

					case 0xA0: ins->opcode = MXOP_PFCMPGT; break;
					case 0xA4: ins->opcode = MXOP_PFMAX; break;
					case 0xA6: ins->opcode = MXOP_PFRCPIT1; break;
					case 0xA7: ins->opcode = MXOP_PFRSQIT1; break;
					case 0xAA: ins->opcode = MXOP_PFSUBR; break;
					case 0xAE: ins->opcode = MXOP_PFACC; break;

					case 0xB0: ins->opcode = MXOP_PFCMPEQ; break;
					case 0xB4: ins->opcode = MXOP_PFMUL; break;
					case 0xB6: ins->opcode = MXOP_PFRCPIT2; break;
					case 0xB7: ins->opcode = MXOP_PMULHRWA; break;
#  if (core_level >= 5 && amd_3dnow >= 2) || defined(everything) /* 3dnow+ aka enhanced, etc. whatever you call it */
					case 0xBB: ins->opcode = MXOP_PSWAPD; break;
#  endif
					case 0xBF: ins->opcode = MXOP_PAVGUSB; break;
				}
			} break;
# endif
# if (core_level >= 3 && core_level <= 4) && !defined(no_umov)
			COVER_4(0x10): { /* UMOV */
				ins->opcode = MXOP_UMOV; ins->argc = 2; const int which = (second_byte >> 1) & 1;
				ARGV *s = &ins->argv[which],*d = &ins->argv[which^1];
				d->size = s->size = second_byte & 1 ? data32wordsize : 1;
				INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0x10): {
				const uint8_t which = second_byte&1; ARGV *re = &ins->argv[which],*rm = &ins->argv[which^1];
				re->size = rm->size = 16; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				if (ins->rep >= MX86_REPE) ins->opcode = ins->rep == MX86_REPNE ? MXOP_MOVSS : MXOP_MOVSD;
				else ins->opcode = dataprefix32 ? MXOP_MOVUPD : MXOP_MOVUPS;
				set_sse_register(re,mrm.f.reg);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			COVER_2(0x10): COVER_2(0x18): { /* TEST1 r/m,imm */
				ARGV *d = &ins->argv[0],*imm = &ins->argv[1];
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_TEST1; break;
				};
				d->size = (second_byte & 1) ? 2 : 1; ins->argc = 2; imm->size = 1;
				if (second_byte & 8)	set_immediate(imm,fetch_u8());
				else			set_register(imm,MX86_REG_CL);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0x12): {
				const uint8_t which = second_byte & 1; ARGV *d = &ins->argv[which],*s = &ins->argv[which^1];
				ins->argc = 2; d->size = 16; s->size = 8;
				if (ins->rep == MX86_REPNE) ins->opcode = MXOP_MOVSLDUP;
				else if (ins->rep == MX86_REPE) ins->opcode = MXOP_MOVDDUP;
				else ins->opcode = dataprefix32 ? MXOP_MOVLPD : MXOP_MOVLPS;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
				/* Hm..... so the reg-reg version of MXOP_MOVLPS is MXOP_MOVHLPS? */
				if (ins->opcode == MXOP_MOVLPS && s->regtype != MX86_RT_NONE)
					ins->opcode = MXOP_MOVHLPS;
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			COVER_2(0x12): COVER_2(0x1A): { /* CLEAR1 r/m,imm */
				ARGV *d = &ins->argv[0],*imm = &ins->argv[1];
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_CLEAR1; break;
				};
				d->size = (second_byte & 1) ? 2 : 1; imm->size = 1; ins->argc = 2;
				if (second_byte & 8)	set_immediate(imm,fetch_u8());
				else			set_register(imm,MX86_REG_CL);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			COVER_2(0x14): COVER_2(0x1C): { /* SET1 r/m,imm */
				ARGV *d = &ins->argv[0],*imm = &ins->argv[1];
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_SET1; break;
				};
				d->size = (second_byte & 1) ? 2 : 1; imm->size = 1; ins->argc = 2;
				if (second_byte & 8)	set_immediate(imm,fetch_u8());
				else			set_register(imm,MX86_REG_CL);
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			COVER_2(0x14): { /* MXOP_UNPCKLPS, MXOP_UNPCKLPD, MXOP_UNPCKHPS, MXOP_UNPCKHPD */
				ins->opcode = MXOP_UNPCKLPS + ((second_byte & 1) << 1) + (dataprefix32 & 1);
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0x16): {
				const uint8_t which = second_byte & 1; ARGV *d = &ins->argv[which],*s = &ins->argv[which^1];
				ins->opcode=(ins->rep==MX86_REPNE)?MXOP_MOVSHDUP:(dataprefix32?MXOP_MOVHPD:MXOP_MOVHPS);
				ins->argc = 2; d->size = 16; s->size = 8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
				if (ins->opcode == MXOP_MOVHPS && s->regtype != MX86_RT_NONE) ins->opcode = MXOP_MOVLHPS;
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			COVER_2(0x16): COVER_2(0x1E): { /* NOT1 r/m,imm */
				ARGV *d = &ins->argv[0],*imm = &ins->argv[1];
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_NOT1; break;
				};
				d->size = (second_byte & 1) ? 2 : 1; imm->size = 1; ins->argc = 2;
				if (second_byte & 8)	set_immediate(imm,fetch_u8());
				else			set_register(imm,MX86_REG_CL);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x18: {
				ARGV *d = &ins->argv[0]; d->size = 4; INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_PREFETCHNTA; ins->argc = 1; break;
					case 1: case 2: case 3: ins->opcode = MXOP_PREFETCHT0 + mrm.f.reg - 1; ins->argc = 1; break;
				}
			} break;
# endif

# if core_level >= 5 && (defined(pentiumpro) || pentium >= 2)
			case 0x1F: {
				ARGV *d = &ins->argv[0]/*,*s = &ins->argv[1]*/; d->size = data32wordsize;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_NOP; ins->argc = 1; break;
				}
			} break;
# endif
# if core_level >= 3
			COVER_4(0x20): { /* mov control/debug reg (mod == 3, apparently mod != 3 is illegal) */
				const int which = (second_byte >> 1) & 1; ins->opcode = MXOP_MOV; ins->argc = 2;
				ARGV *ctrl = &ins->argv[which^1],*reg = &ins->argv[which]; unsigned char imr = fetch_u8();
				unsigned int ridx = (imr>>3)&7;
#  if defined(x64_mode)
				ridx |= (ins->rex.f.r << 3);
#  endif
				ctrl->size = reg->size = ctrlwordsize; set_register(reg,imr&7);
				if (second_byte & 1)	set_debug_register(ctrl,ridx);
				else			set_control_register(ctrl,ridx);
			} break;
# elif defined(do_necv20) /* NEC V20/V30 */
			case 0x20: { /* ADD4S. conflicts with 386 instruction mov reg,CRx */
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->segment = MX86_SEG_ES;
				d->size = s->size = 2; d->memregsz = s->memregsz = 2; ins->opcode = MXOP_ADD4S; ins->argc = 2;
				s->segment = ins->segment >= 0 ? ins->segment : MX86_SEG_DS;
				set_mem_ref_reg(s,MX86_REG_SI); set_mem_ref_reg(d,MX86_REG_DI);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x22: { /* SUB4S */
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->segment = MX86_SEG_ES;
				d->size = s->size = 2; d->memregsz = s->memregsz = 2; ins->opcode = MXOP_SUB4S; ins->argc = 2;
				s->segment = ins->segment >= 0 ? ins->segment : MX86_SEG_DS;
				set_mem_ref_reg(s,MX86_REG_SI); set_mem_ref_reg(d,MX86_REG_DI);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x25: { /* MOVSPA */
				ins->opcode = MXOP_MOVSPA;
			} break;
# endif
# if core_level >= 3 && !defined(x64_mode)
			case 0x24: case 0x26: { /* (mod == 3, apparently mod != 3 is illegal) */
				const int which = (second_byte >> 1) & 1;
				ins->opcode = MXOP_MOV; ins->argc = 2;
				ARGV *ctrl = &ins->argv[which^1],*reg = &ins->argv[which];
				ctrl->size = reg->size = ctrlwordsize; unsigned char mrm = fetch_u8();
				set_register(reg,mrm&7); set_test_register(ctrl,(mrm>>3)&7);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x26: { /* CMP4S */
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->segment = MX86_SEG_ES;
				d->size = s->size = 2; d->memregsz = s->memregsz = 2; ins->opcode = MXOP_CMP4S; ins->argc = 2;
				s->segment = first_byte & 2 ? (ins->segment >= 0 ? ins->segment : MX86_SEG_DS) : MX86_SEG_ES;
				set_mem_ref_reg(s,MX86_REG_SI); set_mem_ref_reg(d,MX86_REG_DI);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2 
			COVER_2(0x28): {
				const uint8_t which = second_byte&1; ins->opcode=MXOP_MOVAPS-(dataprefix32&1); ins->argc = 2;
				ARGV *re = &ins->argv[which],*rm = &ins->argv[which^1]; re->size = rm->size = 16;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(re,mrm.f.reg);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x28: { /* ROL4 */
				ARGV *d = &ins->argv[0]; INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				d->size = 1; ins->argc = 1;
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_ROL4; break;
				};
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x2A: { /* ROR4 */
				ARGV *d = &ins->argv[0]; INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				d->size = 1; ins->argc = 1;
				switch (mrm.f.reg) {
					case 0:	ins->opcode = MXOP_ROR4; break;
				};
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x2A: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; ins->argc = 2; d->size = 16; INS_MRM mrm;
				if (ins->rep >= MX86_REPE) {
					ins->opcode=(ins->rep==MX86_REPNE)?MXOP_CVTSI2SS:MXOP_CVTSI2SD; s->size = 4;
					mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM);
				}
				else {
					ins->opcode = MXOP_CVTPI2PS + (dataprefix32 & 1); s->size = 8;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				}
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x2B: {
				ARGV *rm = &ins->argv[0],*reg = &ins->argv[1]; ins->argc = 2; rm->size = reg->size = 16;
				if (ins->rep >= MX86_REPE) ins->opcode = MXOP_MOVNTSD + ins->rep - MX86_REPE;
				else ins->opcode = MXOP_MOVNTPS - (dataprefix32 & 1);
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(reg,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x2C: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; ins->argc = 2; s->size = 16;
				if (ins->rep >= MX86_REPE) ins->opcode = MXOP_CVTTSD2SI + ins->rep - MX86_REPE;
				else ins->opcode = MXOP_CVTTPS2PI + (dataprefix32 & 1);
				if (ins->rep >= MX86_REPE) { d->size = 4; d->regtype = MX86_RT_REG; }
				else { d->size = 8; d->regtype = MX86_RT_MMX; }
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE); d->reg = mrm.f.reg;
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x2D: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; ins->argc = 2; s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				if (ins->rep >= MX86_REPE) {
					ins->opcode = ins->rep == MX86_REPNE ? MXOP_CVTSS2SI : MXOP_CVTSD2SI; d->size = 4;
					set_register(d,mrm.f.reg);
				}
				else {
					ins->opcode = MXOP_CVTPS2PI + (dataprefix32 & 1); d->size = 8;
					set_mmx_register(d,mrm.f.reg);
				}
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x2D: { /* BRKCS <reg16> */
				INS_MRM mrm = fetch_modregrm();
				if (mrm.f.mod == 3 && mrm.f.reg == 7) {
					ins->opcode = MXOP_BRKCS; ins->argc = 1;
					ARGV *r = &ins->argv[0]; set_register(r,mrm.f.rm); r->size = 2;
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x2E: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; ins->argc = 2; d->size = s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,d->regtype = MX86_RT_SSE);
				ins->opcode = MXOP_UCOMISS + (dataprefix32 ? 1 : 0); d->reg = mrm.f.reg;
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x2F: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,d->regtype = MX86_RT_SSE);
				ins->opcode = MXOP_COMISS + (dataprefix32 ? 1 : 0); d->reg = mrm.f.reg;
			} break;
# endif
# if (core_level == 3 || core_level == 4) && ibmslc_level == 1 /* IBM 386SLC/486SLC */
			case 0x32: ins->opcode = MXOP_RDMSR; break;
# elif core_level == 4 && ibmslc_level >= 2 /* IBM 486SLC2 */
			case 0x30: ins->opcode = MXOP_WRMSR; break;
			case 0x32: ins->opcode = MXOP_RDMSR; break;
# elif core_level >= 5
			case 0x30: ins->opcode = MXOP_WRMSR; break;
			case 0x31: case 0x32: /* RDTSC, RDMSR */
#  if defined(pentiumpro) || pentium >= 2 
			case 0x33: /* Pentium Pro or higher: RDPMC */
#  endif
				ins->opcode = MXOP_RDTSC + second_byte - 0x31; break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x31: case 0x39: case 0x33: case 0x3B: { /* EXT */
				if ((*cip & 0xC0) == 0xC0) { /* mod==3 */
					unsigned char reg = (*cip >> 3) & 7,rm = (*cip & 7); cip++;
					ARGV *s = &ins->argv[2],*bit_start_pos = &ins->argv[0],*bit_length = &ins->argv[1];
					s->size = s->memregsz = 2; set_mem_ref_reg(s,MX86_REG_SI); ins->argc = 3;
					s->segment = ins->segment >= 0 ? ins->segment : MX86_SEG_DS;
					ins->opcode=(second_byte&2)?MXOP_EXT:MXOP_INS;bit_start_pos->size=bit_length->size=1;
					bit_start_pos->memregsz = bit_length->memregsz = 1; set_register(bit_start_pos,rm);
					if (second_byte & 8) set_immediate(bit_length,fetch_u8());
					else set_register(bit_length,reg);
				}
			} break;
# endif
# if core_level >= 6 /* Pentium II or higher */
			case 0x34: ins->opcode = MXOP_SYSENTER; 	break;
# endif
# if core_level >= 6 /* Pentium II or higher */
			case 0x35: ins->opcode = MXOP_SYSEXIT; break;
# endif

# if core_level >= 5 && sse_level >= 2
			case 0x37: ins->opcode = MXOP_GETSEC; break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x38: { /* eugh yuck */
				uint8_t third_byte = fetch_u8(); switch (third_byte) {
				COVER_8(0x00): COVER_4(0x08): {
					ins->opcode = MXOP_PSHUFB + (third_byte & 15); ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				} break;
				case 0x10: if (dataprefix32) {
					ins->opcode = MXOP_PBLENDVB; ins->argc = 3;
					ARGV *d=&ins->argv[0],*s=&ins->argv[1],*i=&ins->argv[2]; d->size=s->size=i->size=16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg); set_sse_register(i,MX86_XMM0);
				} break;
				COVER_2(0x14): {
					ins->opcode = MXOP_BLENDVPS + (third_byte & 1); ins->argc = 3;
					ARGV *d=&ins->argv[0],*s=&ins->argv[1],*i=&ins->argv[2]; d->size=s->size=i->size=16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg); set_sse_register(i,MX86_XMM0);
				} break;
				case 0x17: if (dataprefix32) {
					ins->opcode = MXOP_PTEST; ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				COVER_2(0x1C): case 0x1E: {
					ins->opcode = MXOP_PABSB + (third_byte & 3); ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				} break;
				COVER_4(0x20): COVER_2(0x24): {
					ins->opcode = MXOP_PMOVSXBW + (third_byte & 7); ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				} break;
				COVER_2(0x28): {
					ins->opcode = MXOP_PMULDQ + (third_byte & 1); ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				} break;
				case 0x2A: {
					if (ins->rep >= MX86_REPE) { }
					else if (dataprefix32) {
						ins->opcode = MXOP_MOVNTDQA; ins->argc = 2;
						ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
						INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						set_sse_register(d,mrm.f.reg);
					}
				} break;
				case 0x2B: {
					ins->opcode = MXOP_PACKUSDW; ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				COVER_4(0x30): COVER_2(0x34): {
					ins->opcode = MXOP_PMOVZXBW + (third_byte & 7); ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				} break;
				case 0x37: COVER_8(0x38): COVER_2(0x40): if (dataprefix32) {
					ins->opcode = MXOP_PCMPGTQ + third_byte - 0x37; ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				} break;
				COVER_2(0x80): {
					if (dataprefix32) {
						ins->opcode = MXOP_INVEPT + (third_byte & 1); ins->argc = 2;
						ARGV *s = &ins->argv[0],*d = &ins->argv[1]; d->size = s->size = 4;
						INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
						set_register(s,mrm.f.reg);
					}
				} break;
				case 0xDB: { /* AES IMC */
					if (dataprefix32) {
						ins->opcode = MXOP_AESIMC; ins->argc = 2;
						ARGV *s = &ins->argv[0],*d = &ins->argv[1]; d->size = s->size = 16;
						INS_MRM mrm = decode_rm_ex_(d,ins,d->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						set_sse_register(s,mrm.f.reg);
					}
				} break;
				COVER_4(0xDC): { /* AES ENC/DEC */
					if (dataprefix32) {
						ins->opcode = MXOP_AESENC + (third_byte & 3); ins->argc = 2;
						ARGV *s = &ins->argv[0],*d = &ins->argv[1]; d->size = s->size = 16;
						INS_MRM mrm = decode_rm_ex_(d,ins,d->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						set_sse_register(s,mrm.f.reg);
					}
				} break;
				COVER_2(0xF0):
					if (ins->rep == MX86_REPE) {
						ins->opcode = MXOP_CRC32; ins->argc = 2;
						ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = 4;
						s->size = (third_byte&1)?data32wordsize:1;
						INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM);
						set_register(d,mrm.f.reg);
					}
					else {
						ins->opcode = MXOP_MOVBE; ins->argc = 2;
						const unsigned int which = (third_byte & 1);
						ARGV *re = &ins->argv[which],*rm = &ins->argv[which^1];
						re->size = rm->size = data32wordsize;
						INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM);
						set_register(re,mrm.f.reg);
					}
					break;
			} } break;
# endif

# if core_level >= 5 && sse_level >= 2
			case 0x3A: { /* eugh it gets worse */
				ARGV *d=&ins->argv[0],*s=&ins->argv[1],*i=&ins->argv[2]; ins->argc=3; i->size=1; INS_MRM mrm;
				const uint8_t third_byte = fetch_u8(); switch (third_byte) {
				COVER_2(0x0C): {
					ins->opcode = MXOP_BLENDPS + (third_byte & 1); d->size = s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				case 0x0E: {
					ins->opcode = MXOP_PBLENDW; d->size = s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				case 0x0F: {
					ins->opcode = MXOP_PALIGNR; d->size = s->size = dataprefix32?16:8;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				} break;
				case 0x14: {
					ins->opcode = MXOP_PEXTRB; d->size = 4; s->size = 16;
					mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_sse_register(s,mrm.f.reg);
				} break;
				case 0x16: {
					ins->opcode = MXOP_PEXTRD; d->size = 4; s->size = 16;
					mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_sse_register(s,mrm.f.reg);
				} break;
				case 0x17: {
					ins->opcode = MXOP_EXTRACTPS; s->size = 16; d->size = 4;
					mrm = decode_rm_ex_(d,ins,d->size,PLUSR_TRANSFORM,MX86_RT_REG);
					set_sse_register(s,mrm.f.reg);
				} break;
				case 0x20: {
					ins->opcode = MXOP_PINSRB; d->size = 16; s->size = 4;
					mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_sse_register(d,mrm.f.reg);
				} break;
				case 0x22: {
					ins->opcode = MXOP_PINSRD; d->size = 16; s->size = 4;
					mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_sse_register(d,mrm.f.reg);
				} break;
				case 0x42: {
					ins->opcode = MXOP_MPSADBW; d->size = 16; s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				case 0x21: {
					ins->opcode = MXOP_INSERTPS; d->size = 16; s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				COVER_4(0x08): {
					ins->opcode = MXOP_ROUNDPS + (third_byte & 3); d->size = s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				COVER_2(0x40): {
					ins->opcode = MXOP_DPPS + (third_byte & 1); d->size = s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				COVER_4(0x60): {
					ins->opcode = MXOP_PCMPESTRM + (third_byte & 3); d->size = s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
				case 0xDF: {
					ins->opcode = MXOP_AESKEYGENASSIST; d->size = s->size = 16;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				} break;
			}; set_immediate(i,fetch_u8()); } break;
# endif

# if core_level >= 5 && (defined(pentiumpro) || pentium >= 2)
			COVER_ROW(0x40): { /* CMOVcc */
				ins->opcode = MXOP_CMOVO + second_byte - 0x40; ins->argc = 2;
				ARGV *d = &ins->argv[1],*s = &ins->argv[0]; d->size = s->size = data32wordsize;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x50: {
				ARGV *reg = &ins->argv[0],*rm = &ins->argv[1]; reg->size = 4; rm->size = 16;
				ins->opcode = MXOP_MOVMSKPS - (dataprefix32 & 1); ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_register(reg,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x50: { /* PAVEB conflicts with MOVMSKPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				dst->size = s1->size = s2->size = 8; ins->opcode = MXOP_PAVEB; ins->argc = 3;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x51: {
				ARGV *reg = &ins->argv[0],*rm = &ins->argv[1]; ins->argc = 2; reg->size = rm->size = 16;
				if (dataprefix32) ins->opcode = MXOP_SQRTPD; else ins->opcode = MXOP_SQRTPS + ins->rep;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(reg,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x51: { /* PADDSIW conflicts with SSE SQRTPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				dst->size = s1->size = s2->size = 8; ins->opcode = MXOP_PADDSIW; ins->argc = 3;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			COVER_2(0x52): {
				if (ins->rep == MX86_REPNE) ins->opcode = (second_byte & 1) ? MXOP_RCPSS : MXOP_RSQRTSS;
				else if (second_byte & 1) ins->opcode = MXOP_RCPPS;
				else ins->opcode = MXOP_RSQRTPS;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x52: { /* PMAGW conflicts with RSQRTPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				dst->size = s1->size = s2->size = 8; ins->opcode = MXOP_PMAGW; ins->argc = 3;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif

# if core_level >= 5 && sse_level >= 1
			COVER_2(0x54): {
				ins->opcode = dataprefix32 + MXOP_ANDPS + ((second_byte & 1) << 1); ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x54: { /* PDISTIB conflicts with ANDPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PDISTIB; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
# endif
			case 0x55: { /* PDISTIB conflicts with ANDNPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PSUBSIW; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x56: {
				ins->opcode = MXOP_ORPS + (dataprefix32 & 1); ins->argc = 2;
				ARGV *reg = &ins->argv[0],*rm = &ins->argv[1]; reg->size = rm->size = 16;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(reg,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x57: {
				ins->opcode = MXOP_XORPS + (dataprefix32 & 1); ins->argc = 2;
				ARGV *reg = &ins->argv[0],*rm = &ins->argv[1]; reg->size = rm->size = 16;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(reg,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x58: {
				ins->opcode = (ins->rep >= MX86_REPE ? (2+ins->rep-MX86_REPE) : dataprefix32) + MXOP_ADDPS;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x58: { /* PMVZB conflicts with ADDPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PMVZB; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x59: {
				ins->opcode = (ins->rep>=MX86_REPE)?(MXOP_MULSD+ins->rep-MX86_REPE):
					(MXOP_MULPS+(dataprefix32&1)); ins->argc = 2;
				ARGV *reg = &ins->argv[0],*rm = &ins->argv[1]; reg->size = rm->size = 16;
				INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(reg,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x59: { /* PMULHRW conflicts with MULPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PMULHRWC; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x5A: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; ins->argc = 2; d->size = s->size = 16;
				if (ins->rep >= MX86_REPE) ins->opcode=(ins->rep==MX86_REPNE?MXOP_CVTSS2SD:MXOP_CVTSD2SS);
				else ins->opcode = MXOP_CVTPS2PD + (dataprefix32 & 1);
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,s->regtype=MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x5A: { /* PMVNZB conflicts with CVTPS2PD */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PMVNZB; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x5B: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,s->regtype=MX86_RT_SSE);
				if (ins->rep == MX86_REPNE) ins->opcode = MXOP_CVTTPS2DQ;
				else if (ins->rep == MX86_REPE) break;
				else ins->opcode = MXOP_CVTDQ2PS + (dataprefix32 & 1);
				ins->argc = 2; set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x5B: { /* PMVLZB conflicts with CVTDQ2PS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PMVLZB; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x5C: {
				if (dataprefix32) ins->opcode = MXOP_SUBPD;
				else ins->opcode = MXOP_SUBPS + ins->rep;
				ins->argc = 2; ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x5C: { /* PMVNZB conflicts with SUBPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PMVGEZB; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x5D: {
				ins->opcode = (ins->rep>=MX86_REPE)?(ins->rep+MXOP_MINSD-MX86_REPE):
					(MXOP_MINPS-dataprefix32); ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x5D: { /* PMULHRIW conflicts with MINPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PMULHRIW; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x5E: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; ins->argc = 2; d->size = 16; s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
				if (ins->rep >= MX86_REPE) ins->opcode = MXOP_DIVSD + ins->rep - MX86_REPE;
				else if (dataprefix32) ins->opcode = MXOP_DIVPD;
				else ins->opcode = MXOP_DIVPS;
			} break;
# endif
# if core_level >= 5 && cyrix_level == 6 && mmx == 1
			case 0x5E: { /* PMACHRIW conflicts with DIVPS */
				ARGV *dst = &ins->argv[0],*s1 = &ins->argv[1],*s2 = &ins->argv[2];
				ins->opcode = MXOP_PMACHRIW; ins->argc = 3; dst->size = s1->size = s2->size = 8;
				INS_MRM mrm = decode_rm_ex_(s2,ins,s2->size,PLUSR_TRANSFORM,MX86_RT_MMX);
				set_mmx_register(s1,mrm.f.reg); set_mmx_register(dst,cyrix6x86_mmx_implied(mrm.f.reg));
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x5F: {
				ins->opcode = (ins->rep >= MX86_REPE) ? (ins->rep + MXOP_MAXSD - MX86_REPE) :
					(MXOP_MAXPS - dataprefix32);
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = 16; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x60: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				ins->opcode = MXOP_PUNPCKLBW; ins->argc = 2;
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x61: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				ins->opcode = MXOP_PUNPCKLWD; ins->argc = 2;
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x62: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32 ? 16 : 8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				ins->opcode = MXOP_PUNPCKLDQ; ins->argc = 2;
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x63: case 0x6B: {
				ins->opcode = MXOP_PACKSSWB + ((second_byte >> 3)&1); ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0x64): case 0x66: {
				ins->opcode = MXOP_PCMPGTB + (second_byte & 3); ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x67: {
				ins->opcode = MXOP_PACKUSWB; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x68: {
				ins->opcode = MXOP_PUNPCKHBW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x69: {
				ins->opcode = MXOP_PUNPCKHWD; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x6A: {
				ins->opcode = MXOP_PUNPCKHDQ; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x6C: {
				if (dataprefix32) {
					ins->opcode = MXOP_PUNPCKLQDQ; ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = 16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x6D: {
				if (dataprefix32) {
					ins->opcode = MXOP_PUNPCKHQDQ; ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = 16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE); 
					set_sse_register(d,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && (defined(pentiumpro) || pentium >= 2)
			case 0x6E: case 0x7E: {
				if (ins->rep >= MX86_REPE) {
					if (second_byte == 0x7E && ins->rep == MX86_REPNE) {
						ins->opcode = MXOP_MOVQ; ins->argc = 2;
						ARGV *re = &ins->argv[0],*rm = &ins->argv[1]; rm->size = re->size = 16;
						INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						set_sse_register(re,mrm.f.reg);
					}
				}
				else {
					const unsigned int which = (second_byte >> 4) & 1;
					ins->opcode = MXOP_MOVD; ins->argc = 2;
					ARGV *re = &ins->argv[which],*rm = &ins->argv[which^1]; rm->size = 4;
					INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM);
					if (dataprefix32)	{ set_sse_register(re,mrm.f.reg); re->size = 16; }
					else			{ set_mmx_register(re,mrm.f.reg); re->size = 8; }
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x6F: case 0x7F: {
				if (ins->rep == MX86_REPE) { }
				else {
					const unsigned int which = (second_byte >> 4) & 1; INS_MRM mrm;
					ARGV *d = &ins->argv[which],*s = &ins->argv[which^1]; ins->argc = 2;
					ins->opcode = (ins->rep >= MX86_REPE) ? MXOP_MOVDQU :
						(dataprefix32 ? MXOP_MOVDQA : MXOP_MOVQ);
					if (dataprefix32 || ins->rep >= MX86_REPE) { s->size = d->size = 16;
						mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						set_sse_register(d,mrm.f.reg); }
					else { s->size = d->size = 8;
						mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_MMX);
						set_mmx_register(d,mrm.f.reg); }
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x70: {
				ARGV *d=&ins->argv[0],*s=&ins->argv[1],*i=&ins->argv[2]; i->size=1; ins->argc=3; INS_MRM mrm;
				if (ins->rep >= MX86_REPE) {
					d->size = s->size = 16; ins->opcode = MXOP_PSHUFLW + ins->rep - MX86_REPE; 
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				}
				else {
					d->size = s->size = dataprefix32?16:8; ins->opcode = MXOP_PSHUFW;
					mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				}
				set_immediate(i,fetch_u8());
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x71: {
				ARGV *d=&ins->argv[0],*s=&ins->argv[1]; d->size=dataprefix32?16:8; s->size=1; ins->argc=2;
				INS_MRM mrm = decode_rm_ex_(d,ins,d->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX); set_immediate(s,fetch_u8());
				switch (mrm.f.reg) {
					case 2:	ins->opcode = MXOP_PSRLW; break;
					case 4:	ins->opcode = MXOP_PSRAW; break;
					case 6:	ins->opcode = MXOP_PSLLW; break;
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x72: {
				ARGV *d=&ins->argv[0],*s=&ins->argv[1]; d->size=dataprefix32?16:8; s->size=1; ins->argc=2;
				INS_MRM mrm = decode_rm_ex_(d,ins,d->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX); set_immediate(s,fetch_u8());
				switch (mrm.f.reg) {
					case 2:	ins->opcode = MXOP_PSRLD; break;
					case 4:	ins->opcode = MXOP_PSRAD; break;
					case 6:	ins->opcode = MXOP_PSLLD; break;
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0x73: {
				ARGV *d=&ins->argv[0],*s=&ins->argv[1]; ins->argc=2; s->size=1; d->size=dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(d,ins,d->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				switch (mrm.f.reg) {
					case 2:	                  ins->opcode = MXOP_PSRLQ;  break;
					case 3:	if (dataprefix32) ins->opcode = MXOP_PSRLDQ; break;
					case 6:	                  ins->opcode = MXOP_PSLLQ;  break;
					case 7:	if (dataprefix32) ins->opcode = MXOP_PSLLDQ; break;
				}
				set_immediate(s,fetch_u8());
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0x74): case 0x76: {
				ins->opcode = MXOP_PCMPEQB + (second_byte & 3); ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0x77: ins->opcode = MXOP_EMMS; break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0x78):
				if (ins->rep == MX86_REPE) {
					ARGV *d=&ins->argv[0],*s=&ins->argv[1]; d->size=s->size=16;
					ins->argc=(second_byte&1)?2:4; ins->opcode=MXOP_INSERTQ;
					ins->argv[2].size=ins->argv[3].size=1;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
					if ((second_byte & 1) == 0) { set_immediate(&ins->argv[2],fetch_u8());
						set_immediate(&ins->argv[3],fetch_u8()); }
				}
				else if (dataprefix32) {
					ARGV *d = &ins->argv[0];
					if (second_byte & 1) {
						ins->opcode = MXOP_EXTRQ; ins->argc = 2; ins->argv[1].size = d->size = 16;
						INS_MRM mrm = decode_rm_ex_(&ins->argv[1],ins,ins->argv[1].size,
							PLUSR_TRANSFORM,MX86_RT_SSE);
						set_sse_register(d,mrm.f.reg);
					}
					else {
						INS_MRM mrm = decode_rm_ex_(d,ins,d->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						switch (mrm.f.reg) {
							case 0:	ins->opcode = MXOP_EXTRQ; ins->argc = 3;
								ins->argv[1].size = ins->argv[2].size = 1; d->size = 16;
								set_immediate(&ins->argv[1],fetch_u8());
								set_immediate(&ins->argv[2],fetch_u8()); break;
						}
					}
				}
				else {
					const int which=second_byte&1; ins->opcode=MXOP_VMREAD+(second_byte&1); ins->argc=2; 
					ARGV *s = &ins->argv[which],*d = &ins->argv[which^1]; d->size = s->size = 4;
					INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
				} break;
# endif

# if core_level >= 5 && sse_level >= 1
			COVER_2(0x7C): {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
				INS_MRM mrm; ins->argc = 2;
				if (dataprefix32)               ins->opcode = (second_byte & 1) ? MXOP_HSUBPD : MXOP_HADDPD;
				else if (ins->rep == MX86_REPE) ins->opcode = (second_byte & 1) ? MXOP_HSUBPS : MXOP_HADDPS;
				mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif

# if core_level >= 3
			COVER_ROW(0x80): {
				ARGV *r = &ins->argv[0]; r->size = addrwordsize;
				ins->opcode = MXOP_JO+(second_byte&0xF); ins->argc = 1;
				uint32_t curp = state->ip_value + (uint32_t)(cip - state->read_ip);
				if (isdata32)	set_immediate(r,curp + 4 + ((uint32_t)((int32_t)fetch_u32())));
				else		set_immediate(r,curp + 2 + ((uint32_t)((int16_t)fetch_u16())));
			} break;
# endif
# if core_level >= 3
			COVER_ROW(0x90): {
				ins->opcode = MXOP_SETO+(second_byte&0xF); ins->argc = 1;
				ARGV *r = &ins->argv[0]; r->size = 1;
				/*INS_MRM mrm = */decode_rm_(r,ins,r->size,PLUSR_TRANSFORM);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x91: { /* RETRBI */
				ins->opcode = MXOP_RETRBI; 
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x92: { /* FINT */
				ins->opcode = MXOP_FINT;
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x94: { /* TSKSW */
				ins->opcode = MXOP_TSKSW;
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0x95: { /* MOVSPB <reg16> */
				INS_MRM mrm = fetch_modregrm();
				if (mrm.f.mod == 3 && mrm.f.reg == 7) {
					ins->opcode = MXOP_MOVSPB; ins->argc = 1;
					ARGV *r = &ins->argv[0]; set_register(r,mrm.f.rm); r->size = 2;
				}
			} break;
# endif
# if core_level >= 3
			COVER_2(0xA0): {
				ins->opcode = MXOP_PUSH + (second_byte & 1); ins->argc = 1;
				ARGV *r = &ins->argv[0]; set_segment_register(r,MX86_SEG_FS);
			} break;
# endif
# if core_level >= 4
			case 0xA2: ins->opcode = MXOP_CPUID; break;
# endif
# if core_level >= 3
			case 0xA3: {
				ins->opcode = MXOP_BT; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = datawordsize;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
			} break;
# endif
# if core_level >= 3
			COVER_2(0xA4): COVER_2(0xAC): {
				ins->opcode=(second_byte&8)?MXOP_SHRD:MXOP_SHLD; ins->argc=3;
				ARGV *d=&ins->argv[0],*s=&ins->argv[1],*imm=&ins->argv[2]; imm->size=1;
				d->size=s->size=datawordsize;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
				if (second_byte & 1) set_register(imm,MX86_REG_CL); else set_immediate(imm,fetch_u8());
			} break;
# endif
# if core_level == 4 && !defined(everything) && defined(step_A)
			COVER_2(0xA6): { /* the original CMPXCHG */
				ins->opcode = MXOP_CMPXCHG; ins->argc = 2; ins->altopcode = 1;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1];
				d->size = s->size = (second_byte & 1) ? datawordsize : 1;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				set_register(s,mrm.f.reg);
			} break;
# endif
# if core_level == 3 && !defined(everything) && !defined(am386)
			case 0xA6: {
				ins->opcode = MXOP_XBTS; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = datawordsize;
				INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
			} break;
# endif
# if (core_level >= 5 && cyrix_level >= 134) || defined(everything) /* VIA Nehemiah Padlock extensions */
			case 0xA6: {
				if (ins->rep == MX86_REPNE && *cip == 0xC0)
					{ ins->opcode = MXOP_MONTMUL;   cip++; }
				else if (ins->rep == MX86_REPNE && *cip == 0xC8)
					{ ins->opcode = MXOP_XSHA1;     cip++; }
				else if (ins->rep == MX86_REPNE && *cip == 0xD0) 
					{ ins->opcode = MXOP_XSHA256;   cip++; }
			} break;
# endif
# if core_level == 3 && !defined(everything) && !defined(am386)
			case 0xA7: {
				ins->opcode = MXOP_IBTS; ins->argc = 2;
				ARGV *d = &ins->argv[1],*s = &ins->argv[0]; d->size = s->size = datawordsize;
				INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
			} break;
# endif
# if (core_level >= 5 && cyrix_level >= 134) || defined(everything) /* VIA Nehemiah Padlock extensions */
			case 0xA7: {
				if (ins->rep != MX86_REPE && *cip == 0xC0)
					{ ins->opcode = MXOP_XSTORE;    cip++; }
				else if (ins->rep == MX86_REPNE && *cip == 0xC8)
					{ ins->opcode = MXOP_XCRYPTECB; cip++; }
				else if (ins->rep == MX86_REPNE && *cip == 0xD0)
					{ ins->opcode = MXOP_XCRYPTCBC; cip++; }
				else if (ins->rep == MX86_REPNE && *cip == 0xD8)
					{ ins->opcode = MXOP_XCRYPTCTR; cip++; }
				else if (ins->rep == MX86_REPNE && *cip == 0xE0)
					{ ins->opcode = MXOP_XCRYPTCFB; cip++; }
				else if (ins->rep == MX86_REPNE && *cip == 0xE8)
					{ ins->opcode = MXOP_XCRYPTOFB; cip++; }
			} break;
# endif
# if core_level >= 3
			COVER_2(0xA8): {
				ins->opcode = MXOP_PUSH + (second_byte & 1); ins->argc = 1;
				ARGV *r = &ins->argv[0]; set_segment_register(r,MX86_SEG_GS);
			} break;
# endif
# if core_level >= 3
			case 0xAA: ins->opcode = MXOP_RSM; break;
# endif
# if core_level >= 3
			case 0xAB: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = datawordsize;
				ins->opcode = MXOP_BTS; ins->argc = 2;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
			} break;
# endif

# if core_level >= 5 && sse_level >= 2
			case 0xAE: {
				int m = -1; ARGV *d = &ins->argv[0]; const uint8_t is_mod3 = ((*cip>>6)==3); 
				switch ((*cip >> 3) & 0x07) {
					case 0:	ins->opcode = MXOP_FXSAVE;  m = 1; break;
					case 1:	ins->opcode = MXOP_FXRSTOR; m = 1; break;
					case 2:	ins->opcode = MXOP_LDMXCSR; m = 0; break;
					case 3:	ins->opcode = MXOP_STMXCSR; m = 0; break;
					case 4: if (is_mod3) { } else { ins->opcode = MXOP_XSAVE; m = 0; } break;
					case 5:	if (is_mod3) { ins->opcode = MXOP_LFENCE; } else
						{ ins->opcode = MXOP_XRSTOR; m = 0; } break;
					case 6:	ins->opcode = MXOP_MFENCE; break;
					case 7: if (is_mod3) { ins->opcode = MXOP_SFENCE; } else
						{ ins->opcode = MXOP_CLFLUSH; m = 0; } break;
				}
				switch (m) {
					case 0:	ins->argc = 1; d->size = data32wordsize;
						decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); break;
					case 1: if (!is_mod3) { ins->argc = 1; d->size = 512;
						decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); } break;
					default: cip++; break;
				};
			} break;
# endif

# if core_level >= 3
            case 0xAF: {
                ins->opcode = MXOP_IMUL; ins->argc = 2;
                ARGV *d = &ins->argv[0];
                ARGV *s = &ins->argv[1];
                d->size = data32wordsize;
                s->size = data32wordsize;
                INS_MRM mrm=decode_rm_(s,ins,d->size,PLUSR_TRANSFORM);
                set_register(d,mrm.f.reg);
            } break;
# endif

# if core_level >= 4 && !defined(step_A)
			COVER_2(0xB0): {
				ins->opcode = MXOP_CMPXCHG; ins->argc = 2; ins->altopcode = 0;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1];
				d->size = s->size = (second_byte & 1) ? datawordsize : 1;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				set_register(s,mrm.f.reg);
			} break;
# endif
# if core_level >= 3
			case 0xB2: {
				if ((*cip & 0xC0) == 0xC0) { } else {
					ARGV *d = &ins->argv[0],*s = &ins->argv[1];
					ins->opcode=MXOP_LSS; ins->argc=2; d->size=datawordsize; s->size=data32wordsize+2;
					INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 3
			case 0xB3: {
				ins->opcode = MXOP_BTR; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = datawordsize;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
			} break;
# endif
# if core_level >= 3
			case 0xB4: {
				if ((*cip & 0xC0) == 0xC0) { } else {
					ARGV *d = &ins->argv[0],*s = &ins->argv[1];
					ins->opcode=MXOP_LFS; ins->argc=2; d->size=datawordsize; s->size=data32wordsize+2;
					INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 3 
			case 0xB5: {
				if ((*cip & 0xC0) == 0xC0) { } else {
					ARGV *d = &ins->argv[0],*s = &ins->argv[1];
					ins->opcode=MXOP_LGS; ins->argc=2; d->size=datawordsize; s->size=data32wordsize+2;
					INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 3
			COVER_2(0xB6): {
				ins->opcode = MXOP_MOVZX; ins->argc = 2;
				ARGV *re=&ins->argv[0],*rm=&ins->argv[1]; re->size=datawordsize; rm->size=(second_byte&1)+1;
				INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_register(re,mrm.f.reg);
			} break;
# endif

# if defined(merced_itanium_x86) && !defined(x64_mode) && !defined(everything)
			case 0xB8: { /* Intel merced Itanium */
				ins->opcode = MXOP_JMPX; ins->argc = 1;
				ARGV *re = &ins->argv[0]; re->size = addr32wordsize;
				set_immediate(imm,re->size >= 4 ? fetch_u32() : (uint32_t)((int16_t)fetch_u16()));
			} break;
# elif core_level >= 5 && sse_level >= 2
			case 0xB8: {
				ins->opcode = MXOP_POPCNT; ins->argc = 2;
				ARGV *re = &ins->argv[0],*rm = &ins->argv[1]; re->size = rm->size = data32wordsize;
				INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_register(re,mrm.f.reg);
			} break;
# endif

# if core_level >= 3
			case 0xBA: {
				int m = -1; ARGV *d = &ins->argv[0],*s = &ins->argv[1];
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 4: ins->opcode = MXOP_BT; m = 0; break;
					case 5: ins->opcode = MXOP_BTS; m = 0; break;
					case 6: ins->opcode = MXOP_BTR; m = 0; break;
					case 7: ins->opcode = MXOP_BTC; m = 0; break;
				}
				switch (m) {
					case 0:	ins->argc = 2; d->size = datawordsize;
						set_immediate(s,fetch_u8()); s->size = 1; break;
				};
			} break;
# endif
# if core_level >= 3
			case 0xBB: {
				ins->opcode = MXOP_BTC; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = datawordsize;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
			} break;
# endif
# if core_level >= 3
			COVER_2(0xBC): {
#  if defined(everything)
				if (second_byte == 0xBD && ins->rep == MX86_REPNE) {
					ins->opcode = MXOP_LZCNT; ins->argc = 2;
					ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = datawordsize;
					INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
					break;
				}
#  endif
				ins->opcode = MXOP_BSF + (second_byte & 1); ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = datawordsize;
				INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_register(d,mrm.f.reg);
			} break;
# endif

# if core_level >= 3
			COVER_2(0xBE): {
				ARGV *re = &ins->argv[0],*rm = &ins->argv[1]; ins->opcode = MXOP_MOVSX; ins->argc = 2;
				re->size = datawordsize; rm->size = (second_byte & 1) + 1;
				INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM); set_register(re,mrm.f.reg);
			} break;
# endif

# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0xBE: { /* STOP */
				ins->opcode = MXOP_STOP;
			} break;
# endif

# if core_level >= 4
			COVER_2(0xC0): {
				ins->opcode = MXOP_XADD; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1];
				d->size = s->size = (second_byte & 1) ? datawordsize : 1;
				INS_MRM mrm = decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); set_register(s,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0xC2: {
				ins->opcode = (ins->rep >= MX86_REPE) ? (MXOP_CMPSD + ins->rep - MX86_REPE) :
					(MXOP_CMPPS + (dataprefix32 ? 1 : 0));
				ins->argc = 3; ARGV *d = &ins->argv[0],*s = &ins->argv[1],*i = &ins->argv[2];
					d->size = s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg); set_immediate(i,fetch_u8());
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xC3:
				if (dataprefix32) {
				}
				else {
					ins->opcode = MXOP_MOVNTI; ins->argc = 2;
					ARGV *rm = &ins->argv[0],*reg = &ins->argv[1]; rm->size = reg->size = 4;
					INS_MRM mrm = decode_rm_(rm,ins,rm->size,PLUSR_TRANSFORM);
					set_register(reg,mrm.f.reg);
				}
			break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xC4: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1],*i = &ins->argv[2];
				ins->opcode = MXOP_PINSRW; ins->argc = 3; i->size = 1; s->size = 4;
				d->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_(s,ins,s->size,PLUSR_TRANSFORM); set_immediate(i,fetch_u8());
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xC5: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1],*i = &ins->argv[2];
				ins->opcode = MXOP_PEXTRW; ins->argc = 3; s->size = dataprefix32?16:8;
				d->size = 4; i->size = 1;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				set_register(d,mrm.f.reg); set_immediate(i,fetch_u8());
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xC6:
				if (ins->rep >= MX86_REPE) {
				}
				else {
					ARGV *reg = &ins->argv[0],*imm = &ins->argv[2],*rm = &ins->argv[1];
					ins->opcode = MXOP_SHUFPS + dataprefix32; ins->argc = 3;
					imm->size = 1; rm->size = reg->size = 16;
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(reg,mrm.f.reg); set_immediate(imm,fetch_u8());
				}
				break;
# endif
# if core_level >= 5
			case 0xC7: {
				ARGV *a = &ins->argv[0]; a->size = 8;
				INS_MRM mrm = decode_rm_(a,ins,a->size,PLUSR_TRANSFORM);
				switch (mrm.f.reg) {
					case 1:	ins->opcode = MXOP_CMPXCHG8B; ins->argc = 1; break;
					case 6:	if (ins->rep == MX86_REPNE) ins->opcode = MXOP_VMXON;
						else ins->opcode = dataprefix32 ? MXOP_VMCLEAR : MXOP_VMPTRLD;
						ins->argc = 1; break;
					case 7:	ins->opcode = MXOP_VMPTRST; ins->argc = 1; break;
				};
			} break;
# endif
# if core_level >= 4
			COVER_8(0xC8): {
				ins->opcode = MXOP_BSWAP; ins->argc = 1;
				ARGV *r = &ins->argv[0]; r->size = datawordsize; set_register(r,second_byte & 7);
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0xD0: {
#   define PAIR(d,r)  ((d) + ((r) << 2))
				unsigned char t = dataprefix32 + (ins->rep << 2); unsigned char m = 0;
				switch (t) {
					case PAIR(1,MX86_REP_NONE): ins->opcode = MXOP_ADDSUBPD; m = 1; break;
					case PAIR(0,MX86_REPE): ins->opcode = MXOP_ADDSUBPS; m = 1; break;
				};
				switch (m) {
					case 1: { /* ADDSUBPD/S */
						ARGV *d = &ins->argv[0],*s = &ins->argv[1];
						INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
						ins->argc = 2; d->size = s->size = 16; set_sse_register(d,mrm.f.reg);
					} break;
				}
#   undef PAIR
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD1: {
				ins->opcode = MXOP_PSRLW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD2: {
				ins->opcode = MXOP_PSRLD; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD3: {
				ins->opcode = MXOP_PSRLQ; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD4: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				ins->opcode = MXOP_PADDQ; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD5: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				ins->opcode = MXOP_PMULLW; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD6: {
				if (ins->rep >= MX86_REPE) {
					const uint8_t which = (ins->rep-MX86_REPE)&1; ARGV *d=&ins->argv[0],*s=&ins->argv[1];
					ins->opcode = which?MXOP_MOVQ2DQ:MXOP_MOVDQ2Q; ins->argc=2; d->size=8; s->size=16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
						which ? MX86_RT_MMX : MX86_RT_SSE);
					if (which) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
				}
				else if (dataprefix32) {
					ARGV *d = &ins->argv[1],*s = &ins->argv[0]; ins->opcode = MXOP_MOVQ;
					ins->argc = 2; d->size = 8; s->size = 16;
					INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
					set_sse_register(d,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD7: {
				ins->opcode = MXOP_PMOVMSKB; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = 4; s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_register(d,mrm.f.reg); else set_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD8: {
				ins->opcode = MXOP_PSUBUSB; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xD9: {
				ins->opcode = MXOP_PSUBUSW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xDA: {
				if (ins->rep >= MX86_REPE) {
				}
				else {
					ARGV *rm = &ins->argv[1],*reg = &ins->argv[0];
					ins->argc = 2; ins->opcode = MXOP_PMINUB; rm->size = reg->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(reg,mrm.f.reg);
					else set_mmx_register(reg,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xDB: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				ins->opcode = MXOP_PAND; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0xDC): {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; ins->opcode = MXOP_PADDUSB + (second_byte & 1);
				ins->argc = 2; d->size = s->size = dataprefix32 ? 16 : 8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32 ? MX86_RT_SSE : MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xDE: {
				if (ins->rep >= MX86_REPE) {
				}
				else {
					ARGV *rm = &ins->argv[1],*reg = &ins->argv[0];
					ins->argc = 2; ins->opcode = MXOP_PMAXUB; rm->size = reg->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(reg,mrm.f.reg);
					else set_mmx_register(reg,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xDF: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				ins->opcode = MXOP_PANDN; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE0: {
				ins->opcode = MXOP_PAVGB; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0xE0: { /* BRKXA <int> */
				ins->opcode = MXOP_BRKXA; ins->argc = 1;
				ARGV *r = &ins->argv[0]; set_immediate(r,fetch_u8());
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE1: {
				ins->opcode = MXOP_PSRAW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE2: {
				ins->opcode = MXOP_PSRAD; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE3: {
				ins->opcode = MXOP_PAVGW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE4: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				ins->opcode = MXOP_PMULHUW; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE5: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				ins->opcode = MXOP_PMULHW; ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE6: {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = 16; s->size = 16;
				if (ins->rep >= MX86_REPE) ins->opcode = MXOP_CVTPD2DQ + ins->rep - MX86_REPE;
				else if (dataprefix32) ins->opcode = MXOP_CVTTPD2DQ; else break;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg); ins->argc = 2;
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE7: {
				if (ins->rep >= MX86_REPE) {
				}
				else {
					ARGV *rm = &ins->argv[0],*reg = &ins->argv[1]; ins->argc = 2;
					if (dataprefix32) { ins->opcode = MXOP_MOVNTDQ; rm->size = reg->size = 16; }
					else { ins->opcode = MXOP_MOVNTQ; rm->size = reg->size = 8; }
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(reg,mrm.f.reg);
					else set_mmx_register(reg,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE8: {
				ins->opcode = MXOP_PSUBSB; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xE9: {
				ins->opcode = MXOP_PSUBSW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xEA: {
				if (ins->rep >= MX86_REPE) {
				}
				else {
					ARGV *rm = &ins->argv[1],*reg = &ins->argv[0];
					ins->argc = 2; ins->opcode = MXOP_PMINSW; rm->size = reg->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(reg,mrm.f.reg);
					else set_mmx_register(reg,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xEB: {
				if (ins->rep >= MX86_REPE) {
					/* who wants to bet Intel will add instructions by overloading this one? >:( */
				}
				else {
					ARGV *reg = &ins->argv[0],*rm = &ins->argv[1];
					rm->size = reg->size = dataprefix32?16:8; ins->opcode = MXOP_POR; ins->argc = 2;
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(reg,mrm.f.reg);
					else set_mmx_register(reg,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0xEC): {
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				ins->opcode = MXOP_PADDSB + (second_byte & 1); ins->argc = 2;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xEE: {
				if (ins->rep >= MX86_REPE) {
				}
				else {
					ins->argc = 2; ins->opcode = MXOP_PMAXSW;
					ARGV *rm=&ins->argv[1],*reg=&ins->argv[0]; rm->size = reg->size = dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(reg,mrm.f.reg);
					else set_mmx_register(reg,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0xEF: {
				ins->opcode = MXOP_PXOR; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1];
				d->size = s->size = dataprefix32 ? 16 : 8; /* 128 bit = 16 bytes */
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 1
			case 0xF0: {
				ins->opcode = MXOP_LDDQU; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = 16;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,MX86_RT_SSE);
				set_sse_register(d,mrm.f.reg);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0xF0: { /* RETXA */
	 			ins->opcode = MXOP_RETXA; ins->argc = 1;
				ARGV *d = &ins->argv[0]; set_immediate(d,fetch_u8());
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF1: {
				ins->opcode = MXOP_PSLLW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF2: {
				ins->opcode = MXOP_PSLLD; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF3: {
				ins->opcode = MXOP_PSLLQ; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF4: {
				ins->opcode = MXOP_PMULUDQ; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = 4; s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_register(d,mrm.f.reg); else set_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF5: {
				if (ins->rep >= MX86_REPE) {
				}
				else {
					ins->opcode = MXOP_PMADDWD; ins->argc = 2;
					ARGV *rm=&ins->argv[1],*reg=&ins->argv[0]; rm->size=reg->size=dataprefix32?16:8;
					INS_MRM mrm = decode_rm_ex_(rm,ins,rm->size,PLUSR_TRANSFORM,
						dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
					if (dataprefix32) set_sse_register(reg,mrm.f.reg);
					else set_mmx_register(reg,mrm.f.reg);
				}
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF6: {
				ins->opcode = MXOP_PSADBW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF7: {
				ins->opcode = dataprefix32 ? MXOP_MASKMOVDQU : MXOP_MASKMOVQ; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; s->size = d->size = dataprefix32 ? 16 : 8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2 
			case 0xF8: {
				ins->opcode = MXOP_PSUBB; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xF9: {
				ins->opcode = MXOP_PSUBW; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xFA: {
				ins->opcode = MXOP_PSUBD; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			case 0xFB: {
				ins->opcode = MXOP_PSUBQ; ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if core_level >= 5 && sse_level >= 2
			COVER_2(0xFC): case 0xFE: {
				ins->opcode = MXOP_PADDB + (second_byte & 3); ins->argc = 2;
				ARGV *d = &ins->argv[0],*s = &ins->argv[1]; d->size = s->size = dataprefix32?16:8;
				INS_MRM mrm = decode_rm_ex_(s,ins,s->size,PLUSR_TRANSFORM,
					dataprefix32?MX86_RT_SSE:MX86_RT_MMX);
				if (dataprefix32) set_sse_register(d,mrm.f.reg); else set_mmx_register(d,mrm.f.reg);
			} break;
# endif
# if defined(do_necv20) && !defined(x64_mode) /* NEC V20/V30 */
			case 0xFF: { /* BRKEM */
				ins->opcode = MXOP_BRKEM; ins->argc = 1; 
				ARGV *r = &ins->argv[0]; set_immediate(r,fetch_u8());
			} break;
# endif
# if defined(enable_oio) /* Cyrix 6x86 "Official Invalid Opcode", which everyone else also leaves undefined */
/* TODO: Amd 586 also defines this */
			case 0xFF: ins->opcode = MXOP_OIO; break;
# endif
		} break; }
#endif /* core > 0 */

/* FIXME: you need to go through this opcode list and #ifdef off the ones that
 *        appeared on the 287, 387, 486, Pentium, etc */
/*---------------------------------- FPU decoding ------------------------------ */
		COVER_8(0xD8): {
#define FPU_CODE(fb,sb)	((((fb)&7)<<8)|(sb))
			const unsigned char fpu_second_byte = fetch_u8();
			const uint16_t fpu_code = ins->fpu_code = ((first_byte & 7) << 8) | fpu_second_byte;
			switch (fpu_code) {
				COVER_16ROW(FPU_CODE(0xD8,0x00)): COVER_16ROW(FPU_CODE(0xDC,0x00)): { /* 0xD800...0xD8FF, 0xDC00...0xDCFF */
					const unsigned int which = (fpu_code >> 10) && (fpu_code & 0xC0) == 0xC0;
					ARGV *d = &ins->argv[which],*s = &ins->argv[which^1]; ins->argc = 2;
					set_fpu_register(d,MX86_ST(0));

					if ((fpu_code&0xC0) == 0xC0)
						set_fpu_register(s,MX86_ST(fpu_code&7));
					else {
						s->size = (fpu_code >> 10) ? 8 : 4;
						cip--; decode_rm_(s,ins,s->size,PLUSR_TRANSFORM);
					}

					if (which) {
						switch ((fpu_code>>3)&7) {
							case 0x00:ins->opcode = MXOP_FADD;  break; /* 0xDC 0xC0 */
							case 0x01:ins->opcode = MXOP_FMUL;  break; /* 0xDC 0xC8 */
							case 0x04:ins->opcode = MXOP_FSUBR; break; /* 0xDC 0xE0 */
							case 0x05:ins->opcode = MXOP_FSUB;  break; /* 0xDC 0xE8 */
							case 0x06:ins->opcode = MXOP_FDIVR; break; /* 0xDC 0xF0 */
							case 0x07:ins->opcode = MXOP_FDIV;  break; /* 0xDC 0xF8 */
						}
					}
					else {
						switch ((fpu_code>>3)&7) {
							case 0x00:ins->opcode = MXOP_FADD;  break; /* 0xD8 0xC0 */
							case 0x01:ins->opcode = MXOP_FMUL;  break; /* 0xD8 0xC8 */
							case 0x02:ins->opcode = MXOP_FCOM;  break; /* 0xD8 0xD0 */
							case 0x03:ins->opcode = MXOP_FCOMP; break; /* 0xD8 0xD8 */
							case 0x04:ins->opcode = MXOP_FSUB;  break; /* 0xD8 0xE0 */
							case 0x05:ins->opcode = MXOP_FSUBR; break; /* 0xD8 0xE8 */
							case 0x06:ins->opcode = MXOP_FDIV;  break; /* 0xD8 0xF0 */
							case 0x07:ins->opcode = MXOP_FDIVR; break; /* 0xD8 0xF8 */
						}
					};
				} break;
				COVER_4ROW(FPU_CODE(0xD9,0x00)): COVER_4ROW(FPU_CODE(0xD9,0x40)):
				COVER_4ROW(FPU_CODE(0xD9,0x80)): { /* 0xD900...0xD9BF */
					const unsigned char in = (fpu_code >> 3) & 7; ins->argc = 1; ARGV *d = &ins->argv[0]; cip--;
					switch (in) {
						case 0: d->size = 4; ins->opcode = MXOP_FLD;    break;
						case 2: d->size = 4; ins->opcode = MXOP_FST;    break;
						case 3: d->size = 4; ins->opcode = MXOP_FSTP;   break;
						case 4: d->size = isdata32?28:14;ins->opcode = MXOP_FLDENV; break;
						case 5: d->size = 2; ins->opcode = MXOP_FLDCW;  break;
						case 6: d->size = isdata32?28:14;ins->opcode = MXOP_FSTENV; break;
						case 7: d->size = 2; ins->opcode = MXOP_FSTCW;  break;
					}; decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				} break;
				COVER_8(FPU_CODE(0xD9,0xC0)): ins->argc = 1; ins->opcode = MXOP_FLD;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xD9,0xC8)): ins->argc = 1; ins->opcode = MXOP_FXCH;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				case FPU_CODE(0xD9,0xD0): ins->opcode = MXOP_FNOP; break;

				case FPU_CODE(0xD9,0xE0): ins->opcode = MXOP_FCHS; break;
				case FPU_CODE(0xD9,0xE1): ins->opcode = MXOP_FABS; break;

				case FPU_CODE(0xD9,0xE4): ins->opcode = MXOP_FTST; break;
				case FPU_CODE(0xD9,0xE5): ins->opcode = MXOP_FXAM; break;

				case FPU_CODE(0xD9,0xE8): ins->opcode = MXOP_FLD1; break;
				case FPU_CODE(0xD9,0xE9): ins->opcode = MXOP_FLDL2T; break;
				case FPU_CODE(0xD9,0xEA): ins->opcode = MXOP_FLDL2E; break;
				case FPU_CODE(0xD9,0xEB): ins->opcode = MXOP_FLDPI; break;
				case FPU_CODE(0xD9,0xEC): ins->opcode = MXOP_FLDLG2; break;
				case FPU_CODE(0xD9,0xED): ins->opcode = MXOP_FLDLN2; break;
				case FPU_CODE(0xD9,0xEE): ins->opcode = MXOP_FLDZ; break;

				case FPU_CODE(0xD9,0xF0): ins->opcode = MXOP_F2XM1; break;
				case FPU_CODE(0xD9,0xF1): ins->opcode = MXOP_FYL2X; break;
				case FPU_CODE(0xD9,0xF2): ins->opcode = MXOP_FPTAN; break;
				case FPU_CODE(0xD9,0xF3): ins->opcode = MXOP_FPATAN; break;
				case FPU_CODE(0xD9,0xF4): ins->opcode = MXOP_FXTRACT; break;
				case FPU_CODE(0xD9,0xF5): ins->opcode = MXOP_FPREM1; break;
				case FPU_CODE(0xD9,0xF6): ins->opcode = MXOP_FDECSTP; break;
				case FPU_CODE(0xD9,0xF7): ins->opcode = MXOP_FINCSTP; break;
				case FPU_CODE(0xD9,0xF8): ins->opcode = MXOP_FPREM; break;
				case FPU_CODE(0xD9,0xF9): ins->opcode = MXOP_FYL2XP1; break;
				case FPU_CODE(0xD9,0xFA): ins->opcode = MXOP_FSQRT; break;
				case FPU_CODE(0xD9,0xFB): ins->opcode = MXOP_FSINCOS; break;
				case FPU_CODE(0xD9,0xFC): ins->opcode = MXOP_FRNDINT; break;
				case FPU_CODE(0xD9,0xFD): ins->opcode = MXOP_FSCALE; break;
				case FPU_CODE(0xD9,0xFE): ins->opcode = MXOP_FSIN; break;
				case FPU_CODE(0xD9,0xFF): ins->opcode = MXOP_FCOS; break;

				COVER_4ROW(FPU_CODE(0xDA,0x00)): COVER_4ROW(FPU_CODE(0xDA,0x40)):
				COVER_4ROW(FPU_CODE(0xDA,0x80)): { ins->argc = 1; ARGV *d = &ins->argv[0]; cip--;
					switch ((fpu_code>>3)&7) {
						case 0x00:ins->opcode = MXOP_FIADD;  break; /* 0xD8 0xC0 */
						case 0x01:ins->opcode = MXOP_FIMUL;  break; /* 0xD8 0xC8 */
						case 0x02:ins->opcode = MXOP_FICOM;  break; /* 0xD8 0xD0 */
						case 0x03:ins->opcode = MXOP_FICOMP; break; /* 0xD8 0xD8 */
						case 0x04:ins->opcode = MXOP_FISUB;  break; /* 0xD8 0xE0 */
						case 0x05:ins->opcode = MXOP_FISUBR; break; /* 0xD8 0xE8 */
						case 0x06:ins->opcode = MXOP_FIDIV;  break; /* 0xD8 0xF0 */
						case 0x07:ins->opcode = MXOP_FIDIVR; break; /* 0xD8 0xF8 */
					} d->size = 4; decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				} break;

# if defined(everything) || core_level >= 6 || (core_level == 5 && defined(pentiumpro))
				COVER_8(FPU_CODE(0xDA,0xC0)): ins->argc = 1; ins->opcode = MXOP_FCMOVB;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDA,0xC8)): ins->argc = 1; ins->opcode = MXOP_FCMOVE;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDA,0xD0)): ins->argc = 1; ins->opcode = MXOP_FCMOVBE;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDA,0xD8)): ins->argc = 1; ins->opcode = MXOP_FCMOVU;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
# endif

				case FPU_CODE(0xDA,0xE9): ins->opcode = MXOP_FUCOMPP; break;

				COVER_4ROW(FPU_CODE(0xDB,0x00)): { ins->argc = 2; ARGV *d=&ins->argv[1];
					const unsigned int which = (fpu_code>>3)&7; static unsigned int table[8*2] = {
						MXOP_FILD,	4,	MXOP_FISTTP,	4,
						MXOP_FIST,	4,	MXOP_FISTP,	4,
						MXOP_FBLD,	0,	MXOP_FLD,	10,
						MXOP_FBSTP,	0,	MXOP_FSTP,	10
					}; d->size = table[(which*2)+1]; if (d->size == 0) break;
					ARGV *s=&ins->argv[0];set_fpu_register(s,MX86_ST(0));cip--;
					decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); ins->opcode = table[which*2];
				} break;

# if defined(everything) || core_level >= 6 || (core_level == 5 && defined(pentiumpro))
				COVER_8(FPU_CODE(0xDB,0xC0)): ins->argc = 1; ins->opcode = MXOP_FCMOVNB;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDB,0xC8)): ins->argc = 1; ins->opcode = MXOP_FCMOVNE;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDB,0xD0)): ins->argc = 1; ins->opcode = MXOP_FCMOVNBE;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDB,0xD8)): ins->argc = 1; ins->opcode = MXOP_FCMOVNU;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
#endif

				/* TODO: The IIT FPU chip (387-level) defines custom instructions that conflict with
					 the P6 Pentium level instructions defined here */

# if defined(everything) || core_level >= 6 || (core_level == 5 && defined(pentiumpro))
				COVER_8(FPU_CODE(0xDB,0xE8)): ins->argc = 1; ins->opcode = MXOP_FUCOMI;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDB,0xF0)): ins->argc = 1; ins->opcode = MXOP_FCOMI;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
# endif

				case FPU_CODE(0xDB,0xE0): ins->opcode = MXOP_FENI; break;
				case FPU_CODE(0xDB,0xE1): ins->opcode = MXOP_FDISI; break;
				case FPU_CODE(0xDB,0xE2): ins->opcode = MXOP_FCLEX; break;
				case FPU_CODE(0xDB,0xE3): ins->opcode = MXOP_FINIT; break;
#if fpu_level >= 2 || defined(everything)
				case FPU_CODE(0xDB,0xE4): ins->opcode = MXOP_FSETPM; break; /* <- i287 only, later CPUs/FPUs ignore it */
#endif
#if fpu_level == 2 || defined(everything) /* <- FIXME: remove "everything" match if some future instruction takes this opcode */
				case FPU_CODE(0xDB,0xE5): ins->opcode = MXOP_FRSTPM; break; /* <- Undocumented i287 FPU opcode, dropped for i387,
												  modern CPUs unlikely to even know what it is! */
#endif
# if (core_level >= 4 && cyrix_level == 4) || defined(everything) /* FIXME: Did Cyrix keep this for their 586 and 686 clone CPUs? Do any new opcodes conflict? */
				case FPU_CODE(0xDB,0xFC): ins->argc = 1; ins->opcode = MXOP_FRINT2;
					set_fpu_register(&ins->argv[0],0); break;
# endif
				COVER_4ROW(FPU_CODE(0xDD,0x00)): COVER_4ROW(FPU_CODE(0xDD,0x40)):
				COVER_4ROW(FPU_CODE(0xDD,0x80)): { /* 0xDD00...0xDDBF */
					const unsigned char in = (fpu_code >> 3) & 7; ins->argc = 1; ARGV *d = &ins->argv[0]; cip--;
					switch (in) {
						case 0: d->size = 8; ins->opcode = MXOP_FLD;    break;
						case 1: d->size = 8; ins->opcode = MXOP_FISTTP; break;
						case 2: d->size = 8; ins->opcode = MXOP_FST;    break;
						case 3:	d->size = 8; ins->opcode = MXOP_FSTP;   break;
						case 4:	d->size=108; ins->opcode = MXOP_FRSTOR;	break;
						case 6:	d->size=108; ins->opcode = MXOP_FSAVE;	break;
						case 7:	d->size = 2; ins->opcode = MXOP_FSTSW;	break;
					}; decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				} break;
				COVER_8(FPU_CODE(0xDD,0xC0)): ins->opcode=MXOP_FFREE; ins->argc=1;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDD,0xD0)): ins->opcode=MXOP_FST; ins->argc=1;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDD,0xD8)): ins->opcode=MXOP_FSTP; ins->argc=1;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDD,0xE0)): ins->opcode=MXOP_FUCOM; ins->argc=1;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDD,0xE8)): ins->opcode=MXOP_FUCOMP; ins->argc=1;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
# if (core_level >= 4 && cyrix_level == 4) || defined(everything) /* FIXME: Did Cyrix keep this for their 586 and 686 clone CPUs? Do any new opcodes conflict? */
				case FPU_CODE(0xDD,0xFC): ins->argc = 1; ins->opcode = MXOP_FRICHOP;
					set_fpu_register(&ins->argv[0],0); break;
# endif
				COVER_4ROW(FPU_CODE(0xDE,0x00)): COVER_4ROW(FPU_CODE(0xDE,0x40)): 
				COVER_4ROW(FPU_CODE(0xDE,0x80)): { ins->argc = 1; ARGV *d = &ins->argv[0]; cip--;
					switch ((fpu_code>>3)&7) {
						case 0x00:ins->opcode = MXOP_FIADD;  break; /* 0xD8 0xC0 */
						case 0x01:ins->opcode = MXOP_FIMUL;  break; /* 0xD8 0xC8 */
						case 0x02:ins->opcode = MXOP_FICOM;  break; /* 0xD8 0xD0 */
						case 0x03:ins->opcode = MXOP_FICOMP; break; /* 0xD8 0xD8 */
						case 0x04:ins->opcode = MXOP_FISUB;  break; /* 0xD8 0xE0 */
						case 0x05:ins->opcode = MXOP_FISUBR; break; /* 0xD8 0xE8 */
						case 0x06:ins->opcode = MXOP_FIDIV;  break; /* 0xD8 0xF0 */
						case 0x07:ins->opcode = MXOP_FIDIVR; break; /* 0xD8 0xF8 */
					} d->size = 2; decode_rm_(d,ins,d->size,PLUSR_TRANSFORM);
				} break;
				COVER_4ROW(FPU_CODE(0xDE,0xC0)): { ins->argc = 2; ARGV *d=&ins->argv[1],*s=&ins->argv[0];
					set_fpu_register(d,MX86_ST(0)); set_fpu_register(s,fpu_code&7);
					const unsigned char in = (fpu_code >> 3) & 7; switch (in) {
						case 0: ins->opcode = MXOP_FADDP; break;
						case 1: ins->opcode = MXOP_FMULP; break;
						case 4: ins->opcode = MXOP_FSUBP; break;
						case 5: ins->opcode = MXOP_FSUBRP;break;
						case 6: ins->opcode = MXOP_FDIVP; break;
						case 7: ins->opcode = MXOP_FDIVRP;break;
						default: d->reg=s->reg;s->reg=0; switch (fpu_second_byte) {
							case 0xD9: ins->opcode = MXOP_FCOMPP; break;
						} break;
					}
				} break;
				COVER_4ROW(FPU_CODE(0xDF,0x00)): { ins->argc = 2; ARGV *d=&ins->argv[1];
					const unsigned int which = (fpu_code>>3)&7; static unsigned int table[8*2] = {
						MXOP_FILD,	2,	MXOP_FISTTP,	2,
						MXOP_FIST,	2,	MXOP_FISTP,	2,
						MXOP_FBLD,	10,	MXOP_FILD,	8,
						MXOP_FBSTP,	10,	MXOP_FISTP,	8
					}; d->size = table[(which*2)+1];
					ARGV *s=&ins->argv[0];set_fpu_register(s,MX86_ST(0));cip--;
					decode_rm_(d,ins,d->size,PLUSR_TRANSFORM); ins->opcode = table[which*2];
				} break;
# if (core_level >= 4 && cyrix_level == 4) || defined(everything) /* FIXME: Did Cyrix keep this for their 586 and 686 clone CPUs? Do any new opcodes conflict? */
				case FPU_CODE(0xDF,0xFC): ins->argc = 1; ins->opcode = MXOP_FRINEAR;
					set_fpu_register(&ins->argv[0],0); break;
# endif
				COVER_8(FPU_CODE(0xDF,0xE0)): ins->argc = 1; ins->opcode = MXOP_FSTSW; ins->argv[0].size = 2;
					set_register(&ins->argv[0],MX86_REG_AX); break;

# if defined(everything) || core_level >= 6 || (core_level == 5 && defined(pentiumpro))
				COVER_8(FPU_CODE(0xDF,0xE8)): ins->argc = 1; ins->opcode = MXOP_FUCOMIP;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
				COVER_8(FPU_CODE(0xDF,0xF0)): ins->argc = 1; ins->opcode = MXOP_FCOMIP;
					set_fpu_register(&ins->argv[0],fpu_code&7); break;
#endif
			};
#undef FPU_CODE
		} break;
	};
}

#undef data32wordsize
#undef ARGV

/* } */

