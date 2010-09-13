
#include "x86_realmode.h"
#include "bios_stdint.h"
#include "bios_ioport.h"
#include "bios_memio.h"

static inline void call_far(const uint16_t seg,const uint16_t offset) {
	/* it's not safe to call into other code when the stack pointer is 32-bit wide
	 * and pointing within our segment. It needs to be within 16-bit range down
	 * in a memory region. so  we have to switch the stack down first, then call */
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
	call_far(0xC000,0x0003);

	memw_b(0xB8000,'H');
	memw_b(0xB8002,'e');
	memw_b(0xB8004,'l');
	memw_b(0xB8006,'l');
	memw_b(0xB8008,'o');

	for (;;);
}

