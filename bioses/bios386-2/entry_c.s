	.file	"entry_c.c"
#APP
	.code16gcc
	.globl _asm_realcall_prepare
_asm_realcall_prepare:
pushw	%ax
cli
addl	$0xF0000,%esp
xorw	%ax,%ax
movw	%ax,%ss
popw	%ax
retw

	.globl _asm_realcall_leave
_asm_realcall_leave:
pushw	%ax
cli
movw	$0xF000,%ax
movw	%ax,%ss
movw	%ax,%ds
xorw	%ax,%ax
movw	%ax,%es
subl	$0xF0000,%esp
popw	%ax
retw

#NO_APP
	.text
	.globl	int10_putc
	.type	int10_putc, @function
int10_putc:
.LFB19:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
#APP
# 74 "entry_c.c" 1
	callw	_asm_realcall_prepare
# 0 "" 2
# 75 "entry_c.c" 1
	xorw	%bx,%bx
movb	$0x0E,%ah
movb	8(%esp),%al
int	$0x10

# 0 "" 2
# 82 "entry_c.c" 1
	callw	_asm_realcall_leave
# 0 "" 2
#NO_APP
	popl	%ebx
	.cfi_def_cfa_offset 4
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE19:
	.size	int10_putc, .-int10_putc
	.globl	int10_setmode
	.type	int10_setmode, @function
int10_setmode:
.LFB20:
	.cfi_startproc
#APP
# 86 "entry_c.c" 1
	callw	_asm_realcall_prepare
# 0 "" 2
# 87 "entry_c.c" 1
	xorb	%ah,%ah
movb	4(%esp),%al
int	$0x10

# 0 "" 2
# 93 "entry_c.c" 1
	callw	_asm_realcall_leave
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE20:
	.size	int10_setmode, .-int10_setmode
	.globl	int10_puts
	.type	int10_puts, @function
int10_puts:
.LFB21:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	pushl	%edx
	.cfi_def_cfa_offset 12
	movl	12(%esp), %ebx
	jmp	.L4
.L5:
	incl	%ebx
	movsbl	%al, %eax
	movl	%eax, (%esp)
	call	int10_putc
.L4:
	movb	(%ebx), %al
	testb	%al, %al
	jne	.L5
	popl	%eax
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_def_cfa_offset 4
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE21:
	.size	int10_puts, .-int10_puts
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Hello world\r\n"
	.text
	.globl	_cpu_c_entry
	.type	_cpu_c_entry, @function
_cpu_c_entry:
.LFB22:
	.cfi_startproc
	pushl	%ecx
	.cfi_def_cfa_offset 8
#APP
# 63 "entry_c.c" 1
	callw	_asm_realcall_prepare
# 0 "" 2
# 64 "entry_c.c" 1
	pushal
pushfl
lcallw	$-16384,$3
popfl
popal

# 0 "" 2
# 70 "entry_c.c" 1
	callw	_asm_realcall_leave
# 0 "" 2
#NO_APP
	movl	$3, (%esp)
	call	int10_setmode
	movl	$.LC0, (%esp)
	call	int10_puts
.L7:
	jmp	.L7
	.cfi_endproc
.LFE22:
	.size	_cpu_c_entry, .-_cpu_c_entry
	.ident	"GCC: (GNU) 4.6.2"
	.section	.note.GNU-stack,"",@progbits
