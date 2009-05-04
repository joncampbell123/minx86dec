#ifndef __MINX86_OPCODES_H
#define __MINX86_OPCODES_H

/* decoded instruction codes */
enum {
/* 0x00-0x03 */
	MXOP_UD=0,		/* undefined opcode. signal INT 6 */
	MXOP_UD_NOP,		/* undefined opcode, but nothing that would cause INT 6 */
	MXOP_NOP,		/* NOP aka XCHG AX,AX */
	MXOP_PUSH,		/* PUSH */
/* 0x04-0x07 */
	MXOP_POP,		/* POP */
	MXOP_MOV,		/* MOV */
	MXOP_ADD,		/* ADD */
	MXOP_OR,		/* OR */
/* 0x08-0x0B */
	MXOP_ADC,		/* ADC */
	MXOP_SBB,		/* SBB */
	MXOP_AND,		/* AND */
	MXOP_SUB,		/* SUB */
/* 0x0C-0x0F */
	MXOP_XOR,		/* XOR */
	MXOP_CMP,		/* CMP */
	MXOP_DAA,		/* DAA */
	MXOP_DAS,		/* DAS */
/* 0x10-0x13 */
	MXOP_AAA,		/* AAA */
	MXOP_AAS,		/* AAS */
	MXOP_INC,		/* INC */
	MXOP_DEC,		/* DEC */
/* 0x14-0x17 */
	MXOP_JO,		/* JO */
	MXOP_JNO,		/* JNO */
	MXOP_JB,		/* JB */
	MXOP_JNB,		/* JNB */
/* 0x18-0x1B */
	MXOP_JZ,		/* JZ */
	MXOP_JNZ,		/* JNZ */
	MXOP_JBE,		/* JBE */
	MXOP_JA,		/* JA */
/* 0x1C-0x1F */
	MXOP_JS,		/* JS */
	MXOP_JNS,		/* JNS */
	MXOP_JP,		/* JP */
	MXOP_JNP,		/* JNP */
/* 0x20-0x23 */
	MXOP_JL,		/* JL */
	MXOP_JGE,		/* JGE */
	MXOP_JLE,		/* JLE */
	MXOP_JG,		/* JG */
/* 0x24-0x27 */
	MXOP_XCHG,		/* XCHG */
	MXOP_TEST,		/* TEST */
	MXOP_LEA,		/* LEA */
	MXOP_ARPL,		/* ARPL */
/* 0x28-0x2B */
	MXOP_BOUND,		/* BOUND */
	MXOP_CLTS,		/* CLTS */
	MXOP_LAR,		/* LAR */
	MXOP_LGDT,		/* LGDT */
/* 0x2C-0x2F */
	MXOP_LIDT,		/* LIDT */
	MXOP_LLDT,		/* LLDT */
	MXOP_LMSW,		/* LMSW */
	MXOP_LSL,		/* LSL */
/* 0x30-0x33 */
	MXOP_LTR,		/* LTR */
	MXOP_SGDT,		/* SGDT */
	MXOP_SIDT,		/* SIDT */
	MXOP_SLDT,		/* SLDT */
/* 0x34-0x37 */
	MXOP_STR,		/* STR */
	MXOP_SMSW,		/* SMSW */
	MXOP_VERR,		/* VERR */
	MXOP_VERW,		/* VERW */
/* 0x38-0x3B */
	MXOP_INS,		/* INS */
	MXOP_OUTS,		/* OUTS */
	MXOP_ENTER,		/* ENTER */
	MXOP_LEAVE,		/* LEAVE */
/* 0x3C-0x3F */
	MXOP_PUSHA,		/* PUSHA */
	MXOP_POPA,		/* POPA */
	MXOP_PUSHAD,		/* PUSHAD */
	MXOP_POPAD,		/* POPAD */
/* 0x40-0x43 */
	MXOP_MOVS,		/* MOVS */
	MXOP_STOS,		/* STOS */
	MXOP_LODS,		/* LODS */
	MXOP_SCAS,		/* SCAS */
/* 0x44-0x47 */
	MXOP_BSF,		/* BSF */
	MXOP_BSR,		/* BSR */
	MXOP_AAM,		/* AAM */
	MXOP_AAD,		/* AAD */
/* 0x48-0x4B */
	MXOP_CALL,		/* CALL */
	MXOP_CALL_FAR,		/* CALL [far] */
	MXOP_ADDPS,		/* ADDPS */
	MXOP_ADDPD,		/* ADDPD */
/* 0x4C-0x4F */
	MXOP_ADDSD,		/* ADDSD */
	MXOP_ADDSS,		/* ADDSS */
	MXOP_ADDSUBPD,		/* ADDSUBPD */
	MXOP_ADDSUBPS,		/* ADDSUBPS */
/* 0x50-0x53 */
	MXOP_ANDPS,		/* ANDPS */
	MXOP_ANDPD,		/* ANDPD */
	MXOP_ANDNPS,		/* ANDPS */
	MXOP_ANDNPD,		/* ANDPD */
/* 0x54-0x57 */
	MXOP_BSWAP,		/* BSWAP */
	MXOP_BT,		/* BT */
	MXOP_BTC,		/* BTC */
	MXOP_BTR,		/* BTR */
/* 0x58-0x5B */
	MXOP_BTS,		/* BTS */
	MXOP_CBW,		/* CBW */
	MXOP_CWDE,		/* CWDE */
	MXOP_CLC,		/* CLC */
/* 0x5C-0x5F */
	MXOP_CLD,		/* CLD */
	MXOP_CLFLUSH,		/* CLFLUSH */
	MXOP_CLI,		/* CLI */
	MXOP_CMC,		/* CMC */
/* 0x60-0x63 */
	MXOP_CMPPS,		/* CMPPS */
	MXOP_CMPPD,		/* CMPPD */
	MXOP_CMPS,		/* CMPS */
	MXOP_CMOVO,		/* 40 CMOVO -------------- CMOVcc set */
/* 0x64-0x67 */
	MXOP_CMOVNO,		/* 41 CMOVNO */
	MXOP_CMOVC,		/* 42 CMOVC */
	MXOP_CMOVNC,		/* 43 CMOVNC */
	MXOP_CMOVZ,		/* 44 CMOVZ */
/* 0x68-0x6B */
	MXOP_CMOVNZ,		/* 45 CMOVNZ */
	MXOP_CMOVBE,		/* 46 CMOVBE */
	MXOP_CMOVA,		/* 47 CMOVA */
	MXOP_CMOVS,		/* 48 CMOVS */
/* 0x6C-0x6F */
	MXOP_CMOVNS,		/* 49 CMOVNS */
	MXOP_CMOVP,		/* 4A CMOVP */
	MXOP_CMOVNP,		/* 4B CMOVNP */
	MXOP_CMOVL,		/* 4C CMOVL */
/* 0x70-0x73 */
	MXOP_CMOVNL,		/* 4D CMOVNL */
	MXOP_CMOVNG,		/* 4E CMOVNG */
	MXOP_CMOVG,		/* 4F CMOVG */
	MXOP_CMPSD,		/* CMPSD */
/* 0x74-0x77 */
	MXOP_CMPSS,		/* CMPSS */
	MXOP_CMPXCHG,		/* CMPXCHG */
	MXOP_CMPXCHG8B,		/* CMPXCHG8B */
	MXOP_BLENDPS,		/* BLENSPS */
/* 0x78-0x7B */
	MXOP_BLENDPD,		/* BLENDPD */
	MXOP_BLENDVPS,		/* BLENDVPS */
	MXOP_BLENDVPD,		/* BLENDVPD */
	MXOP_CRC32,		/* CRC32 */
/* 0x7C-0x7F */
	MXOP_COMISS,		/* COMISS */
	MXOP_COMISD,		/* COMISD */
	MXOP_CPUID,		/* CPUID */
	MXOP_CVTPI2PS,		/* CVTPI2PS */
/* 0x80-0x83 */
	MXOP_CVTPI2PD,		/* CVTPI2PD */
	MXOP_CVTSI2SD,		/* CVTSI2SD */
	MXOP_CVTPS2PI,		/* CVTPS2PI */
	MXOP_CVTPD2PI,		/* CVTPD2PI */
/* 0x84-0x87 */
	MXOP_CVTSD2SI,		/* CVTSD2SI */
	MXOP_CVTPS2PD,		/* CVTPS2PD */
	MXOP_CVTPD2PS,		/* CVTPD2PS */
	MXOP_CVTSD2SS,		/* CVTSD2SS */
/* 0x88-0x8B */
	MXOP_CVTDQ2PS,		/* CVTDQ2PS */
	MXOP_CVTPS2DQ,		/* CVTPS2DQ */
	MXOP_CVTPD2DQ,		/* CVTPD2DQ */
	MXOP_CVTDQ2PD,		/* CVTDQ2PD */
/* 0x8C-0x8F */
	MXOP_CVTTPS2DQ,		/* CVTTPS2DQ */
	MXOP_CVTTPD2DQ,		/* CVTTPD2DQ */
	MXOP_CVTTPS2PI,		/* CVTTPS2PI */
	MXOP_CVTTPD2PI,		/* CVTTPD2PI */
/* 0x90-0x93 */
	MXOP_CVTSI2SS,		/* CVTSI2SS */
	MXOP_CVTSS2SD,		/* CVTSS2SD */
	MXOP_CVTSS2SI,		/* CVTSS2SI */
	MXOP_CVTTSD2SI,		/* CVTTSD2SI */
/* 0x94-0x97 */
	MXOP_CVTTSS2SI,		/* CVTTSS2SI */

	MXOP_LAST
};

#endif /* */

