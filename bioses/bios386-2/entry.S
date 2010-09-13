
.extern _cpu_c_entry

/* DO NOT CHANGE */
.equ		CODE_SEL,8
.equ		DATA_SEL,16
.equ		CODE16_SEL,24

#define RELBASE(x)	(x - 0xFFF00)

.code16
.section	.cpuentry

.org		RELBASE(0xFFF00)
_cpu_entry2:	cli

		cs
		lgdt		0xFFC0

		movl		%cr0,%eax
		orb		$1,%al
		movl		%eax,%cr0

		movw		$DATA_SEL,%ax
		movw		%ax,%ds

		ljmpl		$CODE_SEL,$_cpu_entry3
/* NTS: the whole point of this code is to switch the CPU into 32-bit protected mode,
 *      load the segment registers with selectors that have 4GB limit, and then switch
 *      back into real mode, WITHOUT reloading the segment registers. This effectively
 *      leaves the CPU in "flat unreal mode" so that the BIOS has full access to memory
 *      from real mode.
 *
 *      Well... on the way we have to jump back through a 16-bit code segment though...
 */
.code32
_cpu_entry3:	movw		%ax,%es
		movw		%ax,%fs
		movw		%ax,%gs
		movw		%ax,%ss

		ljmpl		$CODE16_SEL,$(_cpu_entry4 - 0xF0000)
.code16
_cpu_entry4:
		movl		%cr0,%eax
		andb		$0xFE,%al
		movl		%eax,%cr0
		ljmpl		$0xF000,$(_cpu_entry5 - 0xF0000)
_cpu_entry5:
/* CS = DS = SS = BIOS segment */
		movw		$0xF000,%ax
		movw		%ax,%ds
		movw		%ax,%ss
/* ES = FS = GS = segment relative to base of memory */
		xorw		%ax,%ax
		movw		%ax,%es
		movw		%ax,%fs
		movw		%ax,%gs
/* We "hack" the stack pointer to wrap around the 4GB limit back into the first 64KB of RAM
 * as opposed to hoping and praying that GCC will not compile in local stack references */
		movl		$(0xFFF0 - 0xF0000),%esp
/* OK go */
		ljmp		$0xF000,$_cpu_c_entry

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
/* code16 (limit=0xFFFF base=0xF0000) */
.word		0xFFFF				/* limit 0-15 */
.word		0				/* base 0-15 */
.byte		0x0F				/* base 16-23 */
.byte		0x9A				/* Present=1, DPL=0, code, readable */
.byte		0x80				/* G=1, D=0, limit=16-19 */
.byte		0				/* base 24-31 */

.org		RELBASE(0xFFFF0)
.global _cpu_entry
_cpu_entry:
		ljmp	$0xF000,$0xFF00		/* NTS: Even though we could code offset - 0xF0000 the ELF linker doesn't like that */

.org		RELBASE(0xFFFF5)
.ascii		"09/05/10"

.org		RELBASE(0xFFFFE)
.byte		0xFC,0x90

