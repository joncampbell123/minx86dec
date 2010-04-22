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
/* 0x8C-0x8F */
	"CVTTPS2DQ",		/* CVTTPS2DQ */
	"CVTTPD2DQ",		/* CVTTPD2DQ */
	"CVTTPS2PI",		/* CVTTPS2PI */
	"CVTTPD2PI",		/* CVTTPD2PI */
/* 0x90-0x93 */
	"CVTSI2SS",		/* CVTSI2SS */
	"CVTSS2SD",		/* CVTSS2SD */
	"CVTSS2SI",		/* CVTSS2SI */
	"CVTTSD2SI",		/* CVTTSD2SI */
/* 0x94-0x97 */
	"CVTTSS2SI",		/* CVTTSS2SI */
	"CWD",			/* CWD */
	"CDQ",			/* CDQ */
	"DIV",			/* DIV */
/* 0x98-0x9B */
	"DIVPS",		/* DIVPS */
	"DIVPD",
	"DIVSD",
	"DIVSS",
/* 0x9C-0x9F */
	"DPPS",
	"DPPD",
	"EMMS",
	"EXTRACTPS",
/* 0xA0-0xA3 */
	"F2XM1",
	"FABS",
	"FADD",
	"FADDP",
/* 0xA4-0xA7 */
	"FIADD",
	"FIADDP",
	"FBLD",
	"FBSTP",
/* 0xA8-0xAB */
	"FCHS",
	"FCLEX",
	"FNCLEX",
	"FWAIT",
/* 0xAC-0xAF */
	"FCMOVB",
	"FCMOVE",
	"FCMOVBE",
	"FCMOVU",
/* 0xB0-0xB3 */
	"FCMOVNB",
	"FCMOVNE",
	"FCMOVNBE",
	"FCMOVNU",
/* 0xB4-0xB7 */
	"FCOMI",
	"FCOMIP",
	"FUCOMI",
	"FUCOMIP",
/* 0xB8-0xBC */
	"FCOS",
	"FDECSTP",
	"FDIV",
	"FDIVP",
/* 0xBC-0xBF */
	"FIDIV",
	"FDIVR",
	"FDIVRP",
	"FIDIVR",
/* 0xC0-0xC3 */
	"FFREE",
	"FICOM",
	"FICOMP",
	"FILD",
/* 0xC4-0xC7 */
	"FINCSTP",
	"FINIT",
	"FNINIT",
	"FIST",
/* 0xC8-0xCB */
	"FISTP",
	"FISTTP",
	"FLD",
	"FLD1",
/* 0xCC-0xCF */
	"FLDL2T",
	"FLDL2E",
	"FLDPI",
	"FLDLG2",
/* 0xD0-0xD3 */
	"FLDLN2",
	"FLDZ",
	"FLDCW",
	"FLDENV",
/* 0xD4-0xD7 */
	"FMUL",
	"FMULP",
	"FIMUL",
	"FNOP",
/* 0xD8-0xDB */
	"FPATAN",
	"FPREM",
	"FPREM1",
	"FPTAN",
/* 0xDC-0xDF */
	"FRNDINT",
	"FRSTOR",
	"FSAVE",
	"FNSAVE",
/* 0xE0-0xE3 */
	"FSCALE",
	"FSIN",
	"FSINCOS",
	"FSQRT",
/* 0xE4-0xE7 */
	"FST",
	"FSTP",
	"FSTCW",
	"FNSTCW",
/* 0xE8-0xEB */
	"FSTENV",
	"FNSTENV",
	"FSTSW",
	"FNSTSW",
/* 0xEC-0xEF */
	"FSUB",
	"FSUBP",
	"FISUB",
	"FSUBR",
/* 0xF0-0xF3 */
	"FSUBRP",
	"FISUBR",
	"FTST",
	"FUCOM",
/* 0xF4-0xF7 */
	"FUCOMP",
	"FUCOMPP",
	"FXAM",
	"FXCH",
/* 0xF8-0xFB */
	"FXRSTOR",
	"FXSAVE",
	"FXTRACT",
	"FYL2X",
/* 0xFC-0xFF */
	"FYL2XP1",
	"HADDPD",
	"HADDPS",
	"HLT",
/* 0x100-0x103 */
	"HSUBPD",
	"HSUBPS",
	"IDIV",
	"IMUL",
/* 0x104-0x107 */
	"IN",
	"INSERTPS",
	"INT",
	"INTO",
/* 0x108-0x10B */
	"INVD",
	"INVLPG",
	"IRET",
	"IRETD",
/* 0x10C-0x10F */
	"JCXZ",
	"JMP",
	"JMP FAR",
	"LAHF",
/* 0x110-0x113 */
	"LFENCE",
	"LDDQU",
	"LDMXCSR",
	"LDS",
/* 0x114-0x117 */
	"LSS",
	"LES",
	"LFS",
	"LGS",
/* 0x118-0x11B */
	"LOOP",
	"LOOPE",
	"LOOPNE",
	"MASKMOVDQU",
/* 0x11C-0x11F */
	"MASKMOVQ",
	"MFENCE",
	"MONITOR",
	"MAXPD",
/* 0x120-0x123 */
	"MAXPS",
	"MAXSD",
	"MAXSS",
	"MINPD",
/* 0x124-0x127 */
	"MINPS",
	"MINSD",
	"MINSS",
	"MWAIT",
/* 0x128-0x12B */
	"MOVAPD",
	"MOVAPS",
	"MOVBE",
	"MOVD",
/* 0x12C-0x12F */
	"MOVDDUP",
	"MOVDQA",
	"MOVDQ2Q",
	"MOVHLPS",
/* 0x130-0x133 */
	"MOVHPD",
	"MOVLHPS",
	"MOVLPD",
	"MOVLPS",
/* 0x134-0x137 */
	"MOVQ",
	"MOVDQU",
	"MOVQ2DQ",
	"MOVHPS",
/* 0x138-0x13B */
	"MOVSHDUP",
	"MOVMSKPD",
	"MOVMSKPS",
	"MOVNTDQA",
/* 0x13C-0x13F */
	"MOVNTDQ",
	"MOVNTI",
	"MOVNTPD",
	"MOVNTPS",
/* 0x140-0x143 */
	"MOVNTQ",
	"MOVSLDUP",
	"MOVSS",
	"MOVSX",
/* 0x144-0x147 */
	"MOVUPD",
	"MOVUPS",
	"MOVZX",
	"MOVSD",
/* 0x148-0x14B */
	"MPSADBW",
	"MUL",
	"MULPS",
	"MULPD",
/* 0x14C-0x14F */
	"MULSD",
	"MULSS",
	"NEG",
	"NOT",
/* 0x150-0x153 */
	"ORPS",
	"ORPD",
	"OUT",
	"POPCNT",
/* 0x154-0x157 */
	"POPF",
	"POPFD",
	"PUSHF",
	"PUSHFD",
/* 0x158-0x15B */
	"RCL",
	"RET",
	"RETF",
	"SHL",
/* 0x15C-0x15F */
	"SHR",
	"SAR",
	"SFENCE",
	"--DUMMY_2--",
/* 0x160-0x163 */
	"SETO",
	"SETNO",
	"SETB",
	"SETNB",
/* 0x164-0x167 */
	"SETZ",
	"SETNZ",
	"SETBE",
	"SETA",
/* 0x168-0x16B */
	"SETS",
	"SETNS",
	"SETP",
	"SETNP",
/* 0x16C-0x16F */
	"SETL",
	"SETGE",
	"SETLE",
	"SETG",
/* 0x170-0x173 */
	"SHLD",
	"SHRD",
	"SHUFPS",
	"SHUFPD",
/* 0x174-0x177 */
	"SQRTPS",
	"SQRTSD",
	"SQRTSS",
	"STC",
/* 0x178-0x17B */
	"STD",
	"STI",
	"STMXCSR",
	"RCR",
/* 0x17C-0x17F */
	"SUBPD",
	"SUBPS",
	"SUBSD",
	"SUBSS",
/* 0x180-0x183 */
	"SYSCALL",
	"SYSENTER",
	"SYSEXIT",
	"SYSRET",
/* 0x184-0x187 */
	"UCOMISS",
	"UCOMISD",
	"#UD2",
	"WBINVD",
/* 0x188-0x18B */
	"WRMSR",
	"XADD",
	"XGETBV",
	"XLAT",
/* 0x18C-0x18F */
	"XORPS",
	"XORPD",
	"XRSTOR",
	"XSAVE",
/* 0x190-0x193 */
	"XSETBV",
	"PXOR",
	"UNPCKLPS",
	"UNPCKLPD",
/* 0x194-0x197 */
	"UNPCKHPS",
	"UNPCKHPD",
	"ROUNDPS",
	"ROUNDPD",
/* 0x198-0x19B */
	"ROUNDSS",
	"ROUNDSD",
	"RSM",
	"SAHF",
/* 0x19C-0x19F */
	"RSQRTPS",
	"RSQRTSS",
	"RDTSC",
	"RDMSR",
/* 0x1A0-0x1A3 */
	"RDPMC",
	"RDTSCP",
	"RCPPS",
	"RCPSS",
/* 0x1A4-0x1A7 */
	"INVEPT",
	"INVVPID",
	"VMCLEAR",
	"VMCALL",
/* 0x1A8-0x1AB */
	"VMLAUNCH",
	"VMRESUME",
	"VMPTRLD",
	"VMPTRST",
/* 0x1AC-0x1AF */
	"VMREAD",
	"VMWRITE",
	"VMXOFF",
	"VMXON",
/* 0x1B0-0x1B3 */
	"POR",
	"PREFETCHNTA",
	"PREFETCHT0",
	"PREFETCHT1",
/* 0x1B4-0x1B5 */
	"PREFETCHT2",
	"SWAPGS",
	"PABSB",
	"PABSW",
/* 0x1B8-0x1BB */
	"PABSD",
	"PADDB",
	"PADDW",
	"PADDD",
/* 0x1BC-0x1BF */
	"PADDSB",
	"PADDSW",
	"PADDUSB",
	"PADDUSW",
/* 0x1C0-0x1C3 */
	"PCMPEQB",
	"PCMPEQW",
	"PCMPEQD",
	"PCMPGTB",
/* 0x1C4-0x1C7 */
	"PCMPGTW",
	"PCMPGTD",
	"PCMPESTRM",
	"PCMPESTRI",
/* 0x1C8-0x1CB */
	"PCMPISTRM",
	"PCMPISTRI",
	"PAUSE",
	"PSHUFB",
/* 0x1CC-0x1CF */
	"PHADDW",
	"PHADDD",
	"PHADDSW",
	"PMADDUBSW",
/* 0x1D0-0x1D3 */
	"PHSUBW",
	"PHSUBD",
	"PHSUBSW",
	"PSIGNB",
/* 0x1D4-0x1D7 */
	"PSIGNW",
	"PSIGND",
	"PMULHRSW",
	"PBLENDVB",
/* 0x1D8-0x1DB */
	"PTEST",
	"PMOVSXBW",
	"PMOVSXBD",
	"PMOVSXBQ",
/* 0x1DC-0x1DF */
	"PMOVSXWD",
	"PMOVSXWQ",
	"PMOVSXDQ",
	"PMULDQ",
/* 0x1E0-0x1E3 */
	"PCMPEQQ",
	"PACKUSDW",
	"PMOVZXBW",
	"PMOVZXBD",
	"PMOVZXBQ",
/* 0x1E4-0x1E7 */
	"PMOVZXWD",
	"PMOVZXWQ",
	"PMOVZXDQ",
	"PCMPGTQ",
/* 0x1E8-0x1EB */
	"PMINSB",
	"PMINSD",
	"PMINUW",
	"PMINUD",
/* 0x1EC-0x1EF */
	"PMAXSB",
	"PMAXSD",
	"PMAXUW",
	"PMAXUD",
/* 0x1F0-0x1F3 */
	"PMULLD",
	"PHMINPOSUW",
	"GETSEC",
	"SALC",
/* 0x1F4-0x1F7 */
	"ICEBP",
	"UMOV",
	"LOADALL286",
	"LOADALL386",
/* 0x1F8-0x1FB */
	"ADD4S",
	"BRKEM",
	"CLEAR1",
	"CMP4S",
/* 0x1FC-0x1FF */
	"EXT",
	"FP02",
	"INS",		/* NEC INS */
	"NOT1",
/* 0x200-0x203 */
	"ROL4",
	"ROR4",
	"SET1",
	"SUB4S",
/* 0x204-0x207 */
	"TEST1",
	"RETEM",
	"UNKNOWN_286_0F04",
	"AESENC",
/* 0x208-0x20B */
	"AESENCLAST",
	"ASEDEC",
	"AESDECLAST",
	"AESIMC",
/* 0x20C-0x20F */
	"PBLENDW",
	"PALIGNR",
	"PEXTRB",
	"PEXTRD",
/* 0x210-0x213 */
	"PINSRB",
	"PINSRD",
	"PUNPCKLBW",
	"PUNPCKLWD",
/* 0x214-0x217 */
	"PUNPCKLDQ",
	"PACKSSWB",
	"PACKSSDW",
	"AESKEYGENASSIST",
/* 0x218-0x21B */
	"BROADCASTSS",
	"BROADCASTSD",
	"BROADCASTF128",
	"FEMMS",
/* 0x21C-0x21F */
	"PAVGUSB",
	"PF2ID",
	"PFACC",
	"PFADD",
/* 0x220-0x223 */
	"PFCMPEQ",
	"PFCMPGE",
	"PFCMPGT",
	"PFMAX",
/* 0x224-0x227 */
	"PFMIN",
	"PFMUL",
	"PFRCP",
	"PFRCPIT1",
/* 0x228-0x22B */
	"PFRCPIT2",
	"PFRSQIT1",
	"PFRSQRT",
	"PFSUB",
/* 0x22C-0x22F */
	"PFSUBR",
	"PI2FD",
	"PMULHRWA",
	"PREFETCH",
/* 0x230-0x233 */
	"PREFETCHW",
	"PF2IW",
	"PI2FW",
	"PSWAPD",
/* 0x234-0x237 */
	"PFNACC",
	"PFPNACC",
	"PADDSIW",
	"PAVEB",
/* 0x238-0x23B */
	"PDISTIB",
	"PMACHRIW",
	"PMAGW",
	"PMULHRWC",
/* 0x23C-0x23F */
	"PMULHRIW",
	"PMVZB",
	"PMVNZB",
	"PMVLZB",
/* 0x240-0x243 */
	"PMVGEZB",
	"PSUBSIW",
	"PFRCPV",
	"PFRSQRTV",
/* 0x244-0x247 */
	"MONTMUL",
	"XSTORE",
	"XCRYPTECB",
	"XCRYPTCBC",
/* 0x248-0x24B */
	"XCRYPTCTR",
	"XCRYPTCFB",
	"XCRYPTOFB",
	"XSHA1",
/* 0x24C-0x24F */
	"XSHA256",
	"IBTS",
	"XBTS",
	"FDISI",
/* 0x250-0x253 */
	"FNDISI",
	"FENI",
	"FNENI",
	"FSETPM",
/* 0x254-0x257 */
	"ROL",
	"ROR",
	"FCOM",
	"FCOMP",
/* 0x258-0x25B */
	"FCOMPP",
	"UMC_IDENT",
	"CLGI",
	"INVLPGA",
/* 0x25C-0x25F */
	"SKINIT",
	"STGI",
	"VMLOAD",
	"VMMCALL",
/* 0x260-0x263 */
	"VMRUN",
	"VMSAVE",
	"PACKUSWB",
	"PADDQ",
/* 0x264-0x267 */
	"PAND",
	"PANDN",
	"PAVGB",
	"PAVGW",
/* 0x268-0x26B */
	"PEXTRW",
	"PINSRW",
	"PMADDWD",
	"PMAXSW",
/* 0x26C-0x26F */
	"PMAXUB",
	"PMINSW",
	"PMINUB",
	"PMOVMSKB",
/* 0x270-0x273 */
	"PMULHUW",
	"PMULHW",
	"PMULLW",
	"PMULUDQ",
/* 0x274-0x277 */
	"PSHUFW",
	"PSADBW",
	"PSLLD",
	"PSLLQ",
/* 0x278-0x27B */
	"PSLLW",
	"PSRAD",
	"PSRAW",
	"PSRLD",
/* 0x27C-0x27F */
	"PSRLQ",
	"PSRLW",
	"PSUBB",
	"PSUBW",
/* 0x280-0x283 */
	"PSUBD",
	"PSUBQ",
	"PSUBSB",
	"PSUBSW",
/* 0x284-0x287 */
	"PSUBUSB",
	"PSUBUSW",
	"PUNPCKHBW",
	"PUNPCKHDQ",
/* 0x288-0x28B */
	"PUNPCKHWD",
	"EXTRQ",
	"INSERTQ",

	NULL
};
#else
extern const char *opcode_string[MXOP_LAST+1];
#endif

/* register argv printing (output must be at least 100 chars) */
void minx86dec_regprint(struct minx86dec_argv *a,char *output);
void minx86dec_regprint_x64(struct minx86dec_argv_x64 *a,char *output);

#endif /* */

