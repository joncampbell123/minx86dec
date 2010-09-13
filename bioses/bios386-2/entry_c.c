
#include "x86_realmode.h"
#include "bios_stdint.h"
#include "bios_ioport.h"
#include "bios_memio.h"

/*=========================BIOS C ENTRY POINT==========================*/
void __attribute__((noreturn)) _cpu_c_entry() {
	unsigned int c;

	iori_b(0x41);
	c = ior_b(0x3CF);

	iowi_b(0x41,0x22);
	iow_b(0x3CF,0x44);

	iori_w(0x41);
	c = ior_w(0x3CF);

	iowi_w(0x41,0x2222);
	iow_w(0x3CF,0x4444);

	iori_l(0x41);
	c = ior_l(0x3CF);

	iowi_l(0x41,0x22222222);
	iow_l(0x3CF,0x44444444);

	memw_b(0xB8000,'C');
	memw_b(0xB8001,0x07);
	c = memr_b(0xB8000);
	memw_w(0xB8002,0x1234);
	c = memr_w(0xB8002);
	memw_l(0xB8004,0x12345678);
	c = memr_l(0xB8004);
}

