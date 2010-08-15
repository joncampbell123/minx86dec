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

