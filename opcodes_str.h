#ifndef __MINX86_OPCODES_STR_H
#define __MINX86_OPCODES_STR_H

#include <stdio.h>

#include "minx86dec/opcodes.h"
#include "minx86dec/state.h"

#if defined(DEFINE_OPCODES_STR)
const char *opcode_string[MXOP_LAST+1] = {
/* 0x00-0x03 */
	"#UD",			
	"#silent-ud",
	"NOP",
	"PUSH",
/* 0x04-0x07 */
	"POP",
	"MOV",
	"ADD",
	"OR",
/* 0x08-0x0B */
	"ADC",
	"SBB",
	"AND",
	"SUB",
/* 0x0C-0x0F */
	"XOR",
	"CMP",
	"DAA",
	"DAS",
/* 0x10-0x13 */
	"AAA",
	"AAS",
	"INC",
	"DEC",
/* 0x14-0x17 */
	"JO",
	"JNO",
	"JB",
	"JNB",
/* 0x18-0x1B */
	"JZ",
	"JNZ",
	"JBE",
	"JA",
/* 0x1C-0x1F */
	"JS",
	"JNS",
	"JP",
	"JNP",
/* 0x20-0x23 */
	"JL",
	"JGE",
	"JLE",
	"JG",
/* 0x24-0x27 */
	"XCHG",
	"TEST",
	"LEA",
	"ARPL",
/* 0x28-0x2B */
	"BOUND",
	"CLTS",
	"LAR",
	"LGDT",
/* 0x2C-0x2F */
	"LIDT",
	"LLDT",
	"LMSW",
	"LSL",
/* 0x30-0x33 */
	"LTR",
	"SGDT",
	"SIDT",
	"SLDT",
/* 0x34-0x37 */
	"STR",
	"SMSW",
	"VERR",
	"VERW",
/* 0x38-0x3B */
	"INS",
	"OUTS",
	"ENTER",
	"LEAVE",
/* 0x3C-0x3F */
	"PUSHA",
	"POPA",
	"PUSHAD",
	"POPAD",
/* 0x40-0x43 */
	"MOVS",
	"STOS",
	"LODS",
	"SCAS",
/* 0x44-0x47 */
	"BSF",
	"BSR",
	"AAM",
	"AAD",
/* 0x48-0x4B */
	"CALL",
	"CALL FAR",	/* we made this up */
	"ADDPS",
	"ADDPD",
/* 0x4C-0x4F */
	"ADDSD",
	"ADDSS",
	"ADDSUBPD",
	"ADDSUBPS",
/* 0x50-0x53 */
	"ANDPS",
	"ANDPD",
	"ANDNPS",
	"ANDNPD",
/* 0x54-0x57 */
	"BSWAP",
	"BT",
	"BTC",
	"BTR",
/* 0x58-0x5B */
	"BTS",
	"CBW",
	"CWDE",
	"CLC",
/* 0x5C-0x5F */
	"CLD",
	"CLFLUSH",
	"CLI",
	"CMC",
/* 0x60-0x63 */
	"CMPPS",
	"CMPPD",
	"CMPS",
	"CMOVO",		/* 40 CMOVO -------------- CMOVcc set */
/* 0x64-0x67 */
	"CMOVNO",		/* 41 CMOVNO */
	"CMOVC",		/* 42 CMOVC */
	"CMOVNC",		/* 43 CMOVNC */
	"CMOVZ",		/* 44 CMOVZ */
/* 0x68-0x6B */
	"CMOVNZ",		/* 45 CMOVNZ */
	"CMOVBE",		/* 46 CMOVBE */
	"CMOVA",		/* 47 CMOVA */
	"CMOVS",		/* 48 CMOVS */
/* 0x6C-0x6F */
	"CMOVNS",		/* 49 CMOVNS */
	"CMOVP",		/* 4A CMOVP */
	"CMOVNP",		/* 4B CMOVNP */
	"CMOVL",		/* 4C CMOVL */
/* 0x70-0x73 */
	"CMOVNL",		/* 4D CMOVNL */
	"CMOVNG",		/* 4E CMOVNG */
	"CMOVG",		/* 4F CMOVG */
	"CMPSD",                /* CMPSD */
/* 0x74-0x77 */
	"CMPSS",                /* CMPSS */
	"CMPXCHG",		/* CMPXCHG */
	"CMPXCHG8B",		/* CMPXCHG8B */
	"BLENDPS",		/* BLENSPS */
/* 0x78-0x7B */
	"BLENDPD",		/* BLENDPD */
	"BLENDVPS",		/* BLENDVPS */
	"BLENDVPD",		/* BLENDVPD */
	"CRC32",		/* CRC32 */
/* 0x7C-0x7F */
	"COMISS",		/* COMISS */
	"COMISD",		/* COMISD */
	"CPUID",		/* CPUID */
	"CVTPI2PS",		/* CVTPI2PS */
/* 0x80-0x83 */
	"CVTPI2PD",		/* CVTPI2PD */
	"CVTSI2SD",		/* CVTSI2SD */
	"CVTPS2PI",		/* CVTPS2PI */
	"CVTPD2PI",		/* CVTPD2PI */
/* 0x84-0x87 */
	"CVTSD2SI",		/* CVTSD2SI */
	"CVTPS2PD",		/* CVTPS2PD */
	"CVTPD2PS",		/* CVTPD2PS */
	"CVTSD2SS",		/* CVTSD2SS */
/* 0x88-0x8B */
	"CVTDQ2PS",		/* CVTDQ2PS */
	"CVTPS2DQ",		/* CVTPS2DQ */
	"CVTPD2DQ",		/* CVTPD2DQ */
	"CVTDQ2PD",		/* CVTDQ2PD */

	NULL
};
#else
extern const char *opcode_string[MXOP_LAST+1];
#endif

/* register argv printing (output must be at least 100 chars) */
void minx86dec_regprint(struct minx86dec_argv *a,char *output);

#endif /* */

