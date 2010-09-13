
#include "x86_realmode.h"
#include "bios_stdint.h"

static inline void memw_b(uint32_t addr,uint8_t b) {
	__asm__ __volatile__ (	"es\n"
				"movb	%0,%1"
				:
				: "r" (b), "m" (addr));
				
}

/*=========================BIOS C ENTRY POINT==========================*/
void __attribute__((noreturn)) _cpu_c_entry() {
	memw_b(0xB8000,'C');
}

