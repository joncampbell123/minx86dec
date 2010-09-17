
#include "x86_realmode.h"
#include "bios_stdint.h"
#include "bios_ioport.h"
#include "bios_memio.h"

/* this hack allows assembly language PUSHA followed by direct access via
 * stack pointer. Kinda gross, but avoids a lot of duplicate code too */
struct x86_pusha {
	uint16_t	di,si,bp,sp,bx,dx,cx,ax;		/* <- this lines up with what the CPU will put on the stack */
} __attribute__((packed));

struct x86_pushad {
	uint32_t	edi,esi,ebp,esp,ebx,edx,ecx,eax;	/* <- this lines up with what the CPU will put on the stack */
} __attribute__((packed));

struct x86_saved_stackd {
	uint16_t		ds;
	uint16_t		es;
	uint32_t		flags;
	struct x86_pushad	reg;
} __attribute__((packed));

/* CAUTION: call this using a real-mode 16-bit wide CALL (callw) NOT a 32-bit wide CALL (calll) */
/* stack occupation: 4 x (1 + 8) + 2 = 36 + 2 = 38 bytes */
/* you must use the corresponding undo function to complete this safely */
__asm__ (		".globl _asm_realcall_prepare\n"
			"_asm_realcall_prepare:\n"
				"pushw	%ax\n"
				"cli\n"				/* we're going to screw with the stack, disable interrupts */
				"addl	$0xF0000,%esp\n"	/* BIOS stack is relative to 0xF0000, and we set ESP+base to overflow to 0xF0000 */
				"xorw	%ax,%ax\n"		/* so we add 0xF0000 and zero SS to make the equivalent realmode stack ptr */
				"movw	%ax,%ss\n"
				"popw	%ax\n"
				"retw\n"
);
#define realcall_prepare() \
	__asm__ __volatile__(	"callw	_asm_realcall_prepare");

__asm__ (		".globl _asm_realcall_leave\n"
			"_asm_realcall_leave:\n"
				"pushw	%ax\n"
				"cli\n"
				"movw	$0xF000,%ax\n"		/* restore stack pointer, DS, ES, SS */
				"movw	%ax,%ss\n"
				"movw	%ax,%ds\n"
				"xorw	%ax,%ax\n"
				"movw	%ax,%es\n"
				"subl	$0xF0000,%esp\n"
				"popw	%ax\n"
				"retw\n"
);
#define realcall_leave() \
	__asm__ __volatile__(	"callw	_asm_realcall_leave");

/* call a far subroutine.
 * This is designed for subroutines that don't necessary care what the registers contain.
 * If the subroutine DOES care, use a different version of this function! */
static inline void call_far(const uint16_t seg,const uint16_t offset) {
	/* it's not safe to rely on our funky 32-bit stack scheme when we're
	 * calling other real-mode subroutines. switch the stack pointer around
	 * and then call the far routine */
	realcall_prepare();
	__asm__ __volatile__(	"pushal\n"		/* save regs and flags, the routine may very well trash them all */
				"pushfl\n"
				"lcallw	%0,%1\n"
				"popfl\n"
				"popal\n"
				: : "i" (seg), "i" (offset));
	realcall_leave();
}

void int10_putc(const char c) {
	realcall_prepare();
	__asm__ __volatile__(	"xorw	%%bx,%%bx\n"
				"movb	$0x0E,%%ah\n"
				"movb	%0,%%al\n"
				"int	$0x10\n"
				: /* no outputs */
				: "g" (c) /* input */
				: "%eax", "%ebx" /* we trashd these */);
	realcall_leave();
}

void int10_setmode(unsigned char mode) {
	realcall_prepare();
	__asm__ __volatile__(	"xorb	%%ah,%%ah\n"
				"movb	%0,%%al\n"
				"int	$0x10\n"
				: /* no outputs */
				: "g" (mode) /* input */
				: "%eax" /* we trashd these */);
	realcall_leave();
}

void int10_puts(const char *str) {
	while (*str != '\0') int10_putc(*str++);
}

/*=========================BIOS C ENTRY POINT==========================*/
void __attribute__((noreturn)) _cpu_c_entry() {
	struct x86_saved_stackd stk;

	/* bring the VGA BIOS online (POST) */
	call_far(0xC000,0x0003);

	/* print something on-screen */
	int10_setmode(3);
	int10_puts("Hello world\r\n");

	/* hang */
	for (;;);
}

