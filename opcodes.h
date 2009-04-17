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

	MXOP_LAST
};

#endif /* */

