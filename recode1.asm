; recompiler test code #1
org 0

_start:
; JMP [reg] 16-bit
	jmp	ax
	jmp	bx
	jmp	cx
	jmp	dx
	jmp	si
	jmp	di
	jmp	bp
	jmp	sp
; JMP [reg] 32-bit
	jmp	eax
	jmp	ebx
	jmp	ecx
	jmp	edx
	jmp	esi
	jmp	edi
	jmp	ebp
	jmp	esp
; JMP [addr] 16-bit single-memref
	jmp	[bx]
	jmp	[bp]
	jmp	[si]
	jmp	[di]
; JMP [addr] 16-bit single-memref with offset
	jmp	[bx+3]
	jmp	[bp+4]
	jmp	[si+5]
	jmp	[di+6]
; JMP [addr] 16-bit single-memref with large offset
	jmp	[bx+0x1111]
	jmp	[bp+0x2222]
	jmp	[si+0x3333]
	jmp	[di+0x4444]
; JMP [addr] 16-bit single-memref with negative offset
	jmp	[bx-3]
	jmp	[bp-4]
	jmp	[si-5]
	jmp	[di-6]
; JMP [addr] 16-bit single-memref with large negative offset
	jmp	[bx-0x1111]
	jmp	[bp-0x2222]
	jmp	[si-0x3333]
	jmp	[di-0x4444]
; JMP [addr] 16-bit double-memref
	jmp	[bx+si]
	jmp	[bx+di]
	jmp	[bp+si]
	jmp	[bp+di]
; JMP [addr] 16-bit double-memref and offset
	jmp	[bx+si+3]
	jmp	[bx+di+4]
	jmp	[bp+si+5]
	jmp	[bp+di+6]

