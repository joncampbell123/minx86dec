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

	NULL
};
#else
extern const char *opcode_string[MXOP_LAST+1];
#endif

/* register argv printing (output must be at least 100 chars) */
void minx86dec_regprint(struct minx86dec_argv *a,char *output);

#endif /* */

