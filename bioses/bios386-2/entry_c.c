
#include "x86_realmode.h"
#include "bios_stdint.h"
#include "bios_ioport.h"
#include "bios_memio.h"

static inline void call_far(const uint16_t seg,const uint16_t offset) {
	/* it's not safe to rely on our funky 32-bit stack scheme when we're
	 * calling other real-mode subroutines. switch the stack pointer around
	 * and then call the far routine */
	__asm__ __volatile__(	"pushal\n"
				"pushfl\n"
				"cli\n"
				"addl	$0xF0000,%%esp\n"
				"xorw	%%ax,%%ax\n"
				"movw	%%ax,%%ss\n"
				"lcallw	%0,%1\n"
				"movw	$0xF000,%%ax\n"
				"movw	%%ax,%%ss\n"
				"movw	%%ax,%%ds\n"
				"xorw	%%ax,%%ax\n"
				"movw	%%ax,%%es\n"
				"subl	$0xF0000,%%esp\n"
				"popfl\n"
				"popal\n" : : "i" (seg), "i" (offset));
}

/*=========================BIOS C ENTRY POINT==========================*/
void __attribute__((noreturn)) _cpu_c_entry() {
	/* bring the VGA BIOS online (POST) */
	call_far(0xC000,0x0003);

	/* hang */
	for (;;);
}

