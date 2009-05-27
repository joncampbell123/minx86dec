#ifndef __MINX86DEC_STATE_H
#define __MINX86DEC_STATE_H
/* minx86dec decoder state */

#include "minx86dec/types.h"

struct minx86dec_state {
	uint32_t		ip_value;	/* IP instruction pointer value */
	minx86_read_ptr_t	read_ip;	/* decoding reads from here */
	minx86_read_ptr_t	prefetch_fence,fence; /* decoding stops at prefetch_fence, so that no reads go past fence */
	uint8_t			data32:1;	/* 386+ 32-bit opcode data operand decoding */
	uint8_t			addr32:1;	/* 386+ 32-bit opcode address operand decoding */
	uint8_t			code64:1;	/* AMD64 long mode decoding (for your reference) */
};

/* 8-bit, CPU reg order */
enum {	MX86_REG_AL=0,	MX86_REG_CL,	MX86_REG_DL,	MX86_REG_BL,
	MX86_REG_AH,	MX86_REG_CH,	MX86_REG_DH,	MX86_REG_BH };

/* 16-bit, CPU reg order */
enum {	MX86_REG_AX=0,	MX86_REG_CX,	MX86_REG_DX,	MX86_REG_BX,
	MX86_REG_SP,	MX86_REG_BP,	MX86_REG_SI,	MX86_REG_DI };

/* 32-bit, CPU reg order */
enum {	MX86_REG_EAX=0,	MX86_REG_ECX,	MX86_REG_EDX,	MX86_REG_EBX,
	MX86_REG_ESP,	MX86_REG_EBP,	MX86_REG_ESI,	MX86_REG_EDI };

#if defined(ENABLE_64BIT)
/* 64-bit, CPU reg order */
enum {	MX86_REG_RAX=0,	MX86_REG_RCX,	MX86_REG_RDX,	MX86_REG_RBX,
	MX86_REG_RSP,	MX86_REG_RBP,	MX86_REG_RSI,	MX86_REG_RDI,
	MX86_REG_R8,	MX86_REG_R9,	MX86_REG_R10,	MX86_REG_R11,
	MX86_REG_R12,	MX86_REG_R13,	MX86_REG_R14,	MX86_REG_R15,

	MX86_REG_RIP /* AMD64 rip support */};
#endif

/* MMX registers */
enum {	MX86_MMX0=0,	MX86_MMX1,	MX86_MMX2,	MX86_MMX3,
	MX86_MMX4,	MX86_MMX5,	MX86_MMX6,	MX86_MMX7 };
/* or simply */
#define MX86_MMX(x) (MX86_MMX0+(x))

/* SSE registers */
enum {	MX86_XMM0=0,	MX86_XMM1,	MX86_XMM2,	MX86_XMM3,
	MX86_XMM4,	MX86_XMM5,	MX86_XMM6,	MX86_XMM7,
	MX86_XMM8,	MX86_XMM9,	MX86_XMM10,	MX86_XMM11,
	MX86_XMM12,	MX86_XMM13,	MX86_XMM14,	MX86_XMM15 };
/* or simply */
#define MX86_XMM(x) (MX86_XMM0+(x))

/* FPU registers */
enum {	MX86_ST0,	MX86_ST1,	MX86_ST2,	MX86_ST3,
	MX86_ST4,	MX86_ST5,	MX86_ST6,	MX86_ST7   };
#define MX86_ST(x) (MX86_ST0+(x))

/* segment registers */
enum {	MX86_SEG_ES=0,	MX86_SEG_CS,	MX86_SEG_SS,	MX86_SEG_DS };
#define MX86_SEG_IMM -2

/* REP modes */
enum {	MX86_REP_NONE=0,MX86_REPE,	MX86_REPNE };	/* NOTE: REP = REPE */

/* register type */
enum {	MX86_RT_NONE=0,	/* not a register reference */
	MX86_RT_IMM,	/* immediate value */
	MX86_RT_REG,	/* general register */
	MX86_RT_SREG,	/* segment register */
	MX86_RT_ST,	/* FPU register */
	MX86_RT_MMX,	/* MMX register */
	MX86_RT_SSE,	/* SSE register */
	MX86_RT_CR,	/* CR0...CR7 control registers */
	MX86_RT_DR,	/* DR0...DR7 debug registers */
};

/* decoded instruction operand (32-bit or 16-bit) */
struct minx86dec_argv {
	int				segment;	/* which segment */
	uint16_t			segval;		/* segment value (if segment == -2) */
	unsigned int			size;		/* size of the operand (1=byte 2=word 4=dword etc) */
	int				regtype;	/* type of register (0 if memory ref) */
	int				reg;
	uint32_t			memref_base;	/* base immediate offset */
	uint32_t			value;
	int				memregsz;
	int				memregs;	/* number of memory references to add together */
	int				memreg[4];	/* memory register offsets to add together. note that [0] is scaled by SIB scalar part */
	int				scalar;
};

/* decoded instruction (32-bit or 16-bit) */
struct minx86dec_instruction {
	unsigned int			opcode;		/* MXOP_... */
	minx86_read_ptr_t		start;		/* read_ip value at start of decoding */
	minx86_read_ptr_t		end;		/* read_ip after decoding (first byte of next instruction) */

	uint8_t				lock;		/* LOCK prefix */
	int				segment;	/* segment override (used during decode) or -1 */
	int				rep;		/* REP prefix or 0 */

	int				argc;		/* number of instruction operands */
	struct minx86dec_argv		argv[4];	/* instruction operands */

	uint8_t				addr32:1;	/* 32-bit addr */
	uint8_t				data32:1;	/* 32-bit data */
};

#endif

