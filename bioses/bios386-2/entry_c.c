
#include "x86_realmode.h"
#include "bios_stdint.h"

static inline void memw_b(const uint32_t addr,const uint8_t b) {
#ifdef __NO_INLINE__
	__asm__ __volatile__ (	"es\n"
				"movb	%%al,(%%esi)"
				: /* no outs */
				: "a" ((unsigned int)b), "S" ((unsigned int)addr));
#else
	__asm__ __volatile__ (	"es\n"
				"movb	%0,(%%esi)"
				: /* no outs */
				: "r" (b), "S" ((unsigned int)addr));
#endif				
}

/*=========================BIOS C ENTRY POINT==========================*/
void __attribute__((noreturn)) _cpu_c_entry() {
	memw_b(0xB8000,'C');
	memw_b(0xB8001,0x07);
}

