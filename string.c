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
	"FS",	"GS",	"",	""
};

const char *memptrsizes[17] = {
	"",			/* 0 */
	"(BYTE*)",		/* 1 */
	"(WORD*)",		/* 2 */
	"",
	"(DWORD*)",		/* 4 */
	"",
	"(3WORD*)",		/* 6 */
	"",
	"(QWORD*)",		/* 8 */
	"",
	"(TWORD*)",		/* 10 */
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
	else if (a->segment == MX86_SEG_IMM)
		output += sprintf(output,"0x%04X:",a->segval);

	if (a->size >= 0 && a->size <= 16)
		output += sprintf(output,"%s[",memptrsizes[a->size]);
	else
		output += sprintf(output,"(BYTE[%u])[",a->size);

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
	const char *x;
	output[0] = 0;
	switch (a->regtype) {
		case MX86_RT_NONE:
			minx86dec_memref_print(a,output);
			break;
		case MX86_RT_IMM:
			if (a->segment == MX86_SEG_IMM)
				output += sprintf(output,"0x%04X:",a->segval);

			sprintf(output,"0x%X",a->value);
			break;
		case MX86_RT_REG:
			if (a->size >= 0 && a->size <= 8 && a->reg >= 0 && a->reg <= 16)
				x = regnames[a->size][a->reg];
			else
				x = NULL;

			if (x == NULL)
				fprintf(stderr,"BUG: MX86_RT_REG with size=%d reg=%d which does not exist\n",
					a->size,a->reg);

			strcpy(output,x != NULL ? x : "(null)");
			break;
		case MX86_RT_SREG:
			strcpy(output,sregnames[a->reg]);
			break;
		case MX86_RT_ST:
			sprintf(output,"st(%d)",a->reg);
			break;
		case MX86_RT_MMX:
			sprintf(output,"MM%d",a->reg);
			break;
		case MX86_RT_SSE:
			sprintf(output,"XMM%d",a->reg);
			break;
		case MX86_RT_CR:
			sprintf(output,"CR%d",a->reg);
			break;
		case MX86_RT_DR:
			sprintf(output,"DR%d",a->reg);
			break;
	}
}



static void minx86dec_memref_print_x64(struct minx86dec_argv_x64 *a,char *output) {
	char doop=0;

	if (a->segment >= 0)
		output += sprintf(output,"%s:",sregnames[a->segment]);
	else if (a->segment == MX86_SEG_IMM)
		output += sprintf(output,"0x%04X:",a->segval);

	if (a->size >= 0 && a->size <= 16)
		output += sprintf(output,"%s[",memptrsizes[a->size]);
	else
		output += sprintf(output,"(BYTE[%u])[",a->size);

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

void minx86dec_regprint_x64(struct minx86dec_argv_x64 *a,char *output) {
	const char *x;
	output[0] = 0;
	switch (a->regtype) {
		case MX86_RT_NONE:
			minx86dec_memref_print_x64(a,output);
			break;
		case MX86_RT_IMM:
			if (a->segment == MX86_SEG_IMM)
				output += sprintf(output,"0x%04X:",a->segval);

			sprintf(output,"0x%X",a->value);
			break;
		case MX86_RT_REG:
			if (a->size >= 0 && a->size <= 8 && a->reg >= 0 && a->reg <= 16)
				x = regnames[a->size][a->reg];
			else
				x = NULL;

			if (x == NULL)
				fprintf(stderr,"BUG: MX86_RT_REG with size=%d reg=%d which does not exist\n",
					a->size,a->reg);

			strcpy(output,x != NULL ? x : "(null)");
			break;
		case MX86_RT_SREG:
			strcpy(output,sregnames[a->reg]);
			break;
		case MX86_RT_ST:
			sprintf(output,"st(%d)",a->reg);
			break;
		case MX86_RT_MMX:
			sprintf(output,"MM%d",a->reg);
			break;
		case MX86_RT_SSE:
			sprintf(output,"XMM%d",a->reg);
			break;
		case MX86_RT_CR:
			sprintf(output,"CR%d",a->reg);
			break;
		case MX86_RT_DR:
			sprintf(output,"DR%d",a->reg);
			break;
	}
}

