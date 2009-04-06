#define DEFINE_OPCODES_STR
#include "minx86dec/opcodes_str.h"
#include <string.h>
#include <stdio.h>

const char *regnames[9][17] = {
/* 0 */	{	NULL								},
/* 1 */	{	"AL",	"CL",	"DL",	"BL",	"AH",	"CH",	"DH",	"BH"	},
/* 2 */	{	"AX",	"CX",	"DX",	"BX",	"SP",	"BP",	"SI",	"DI"	},
/* 3 */	{	NULL								},
/* 4 */	{	"EAX",	"ECX",	"EDX",	"EBX",	"ESP",	"EBP",	"ESI",	"EDI"	},
/* 5 */	{	NULL								},
/* 6 */	{	NULL								},
/* 7 */	{	NULL								},
/* 8 */	{	"RAX",	"RCX",	"RDX",	"RBX",	"RSP",	"RBP",	"RSI",	"RDI",
		"R8",	"R9",	"R10",	"R11",	"R12",	"R13",	"R14",	"R15",
		"RIP"								}
};

const char *sregnames[8] = {
	"ES",	"CS",	"SS",	"DS",
	"",	"",	"",	""
};

const char *memptrsizes[17] = {
	"",			/* 0 */
	"(BYTE*)",		/* 1 */
	"(WORD*)",		/* 2 */
	"",
	"(DWORD*)",		/* 4 */
	"",
	"(TWORD*)",		/* 6 */
	"",
	"(QWORD*)",		/* 8 */
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"(DQWORD*)"		/* 16 */
};

static void minx86dec_memref_print(struct minx86dec_argv *a,char *output) {
	char doop=0;

	if (a->segment >= 0)
		output += sprintf(output,"%s:",sregnames[a->segment]);

	output += sprintf(output,"%s[",memptrsizes[a->size]);

	if (a->memregs) {
		int c;

		/* assume [0] is valid */
		if (a->scalar)
			output += sprintf(output,"%s*%d",regnames[a->memregsz][a->memreg[0]],1<<a->scalar);
		else
			output += sprintf(output,"%s",regnames[a->memregsz][a->memreg[0]]);

		doop++;

		for (c=1;c < a->memregs;c++) {
			*output++ = '+';
			output += sprintf(output,"%s",regnames[a->memregsz][a->memreg[c]]);
			doop++;
		}
	}

	if (a->memref_base || a->memregs == 0) {
		uint32_t pofs = a->memref_base;
		char sgn = '+';

		if (pofs & 0x80000000) {
			pofs = (uint32_t)(-pofs);
			sgn = '-';
		}

		if (doop++) *output++ = sgn;
		output += sprintf(output,"0x%X",pofs);
	}

	*output++ = ']';
	*output = 0;
}

void minx86dec_regprint(struct minx86dec_argv *a,char *output) {
	output[0] = 0;
	switch (a->regtype) {
		case MX86_RT_NONE:
			minx86dec_memref_print(a,output);
			break;
		case MX86_RT_IMM:
			sprintf(output,"0x%X",a->value);
			break;
		case MX86_RT_REG:
			strcpy(output,regnames[a->size][a->reg]);
			break;
		case MX86_RT_SREG:
			strcpy(output,sregnames[a->reg]);
			break;
		case MX86_RT_ST:
			sprintf(output,"st%d",a->reg);
			break;
		case MX86_RT_MMX:
			sprintf(output,"mm%d",a->reg);
			break;
		case MX86_RT_SSE:
			sprintf(output,"xmm%d",a->reg);
			break;
	}
}

