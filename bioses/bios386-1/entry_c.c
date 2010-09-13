
/*=========================BIOS C ENTRY POINT==========================*/
void _cpu_c_entry() {
	*((unsigned char*)0xB8000) = 'C';
}

