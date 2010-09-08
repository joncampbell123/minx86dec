
.extern _cpu_c_entry

/* DO NOT CHANGE */
.equ		CODE_SEL,8
.equ		DATA_SEL,16

#define RELBASE(x)	(x - 0xFFF80)

.code16
.section	.cpuentry

.org		RELBASE(0xFFF80)
_cpu_entry2:	cli

		cs
		lgdt		0xFFC0

		movl		%cr0,%eax
		orb		$1,%al
		movl		%eax,%cr0

		movw		$DATA_SEL,%ax
		movw		%ax,%ds

		ljmpl		$CODE_SEL,$_cpu_entry3

.code32
_cpu_entry3:	movw		%ax,%es
		movw		%ax,%fs
		movw		%ax,%gs
		movw		%ax,%ss
		movl		$0xFFF0,%esp
		xorl		%ebp,%ebp
		call		_cpu_c_entry	/* the beauty of older motherboards and emulators is RAM is always present */
_oops:		jmp		_oops

.code16
/* protected mode GDT */
.org		RELBASE(0xFFFC0)
_rom_gdtr:
.word		0x38			/* size */
.long		_rom_gdt
.align		8
_rom_gdt:
/* dummy zero */
.long		0,0
/* code */
.word		0xFFFF				/* limit 0-15 */
.word		0				/* base 0-15 */
.byte		0				/* base 16-23 */
.byte		0x9A				/* Present=1, DPL=0, code, readable */
.byte		0xCF				/* G=1, D=1, limit=16-19 */
.byte		0				/* base 24-31 */
/* data */
.word		0xFFFF				/* limit 0-15 */
.word		0				/* base 0-15 */
.byte		0				/* base 16-23 */
.byte		0x92				/* Present=1, DPL=0, data, readable */
.byte		0xCF				/* G=1, D=1, limit=16-19 */
.byte		0				/* base 24-31 */

.org		RELBASE(0xFFFF0)
.global _cpu_entry
_cpu_entry:
		ljmp	$0xF000,$0xFF80		/* NTS: Even though we could code offset - 0xF0000 the ELF linker doesn't like that */

.org		RELBASE(0xFFFF5)
.ascii		"09/05/10"

.org		RELBASE(0xFFFFE)
.byte		0xFC,0x90

