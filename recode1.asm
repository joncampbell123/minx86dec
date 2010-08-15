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
; JMP [addr] 16-bit double-memref and large offset
	jmp	[bx+si+0x1111]
	jmp	[bx+di+0x2222]
	jmp	[bp+si+0x3333]
	jmp	[bp+di+0x4444]
; JMP [addr] 32-bit single-memref
	jmp	[eax]
	jmp	[ebx]
	jmp	[ecx]
	jmp	[edx]
	jmp	[esi]
	jmp	[edi]
	jmp	[esp]
	jmp	[ebp]
; JMP [addr] 32-bit single-memref with offset
	jmp	[eax+3]
	jmp	[ebx+4]
	jmp	[ecx+5]
	jmp	[edx+6]
	jmp	[esi+7]
	jmp	[edi+8]
	jmp	[esp+9]
	jmp	[ebp+10]
; JMP [addr] 32-bit single-memref with large offset
	jmp	[eax+0x11111111]
	jmp	[ebx+0x22222222]
	jmp	[ecx+0x33333333]
	jmp	[edx+0x44444444]
	jmp	[esi+0x55555555]
	jmp	[edi+0x66666666]
	jmp	[esp+0x77777777]
	jmp	[ebp+0x88888888]
; JMP [addr] 32-bit double-memref
	jmp	[eax+eax]
	jmp	[ebx+eax]
	jmp	[ecx+eax]
	jmp	[edx+eax]
	jmp	[esi+eax]
	jmp	[edi+eax]
	jmp	[esp+eax]
	jmp	[ebp+eax]
; JMP [addr] 32-bit double-memref
	jmp	[eax+ebx]
	jmp	[ebx+ebx]
	jmp	[ecx+ebx]
	jmp	[edx+ebx]
	jmp	[esi+ebx]
	jmp	[edi+ebx]
	jmp	[esp+ebx]
	jmp	[ebp+ebx]
; JMP [addr] 32-bit double-memref and scalar
	jmp	[eax*2+ebx]
	jmp	[ebx*2+ebx]
	jmp	[ecx*2+ebx]
	jmp	[edx*2+ebx]
	jmp	[esi*2+ebx]
	jmp	[edi*2+ebx]
	jmp	[ebp*2+ebx]
; JMP [addr] 32-bit double-memref and scalar
	jmp	[eax*2+ebx+4]
	jmp	[ebx*2+ebx+5]
	jmp	[ecx*2+ebx+6]
	jmp	[edx*2+ebx+7]
	jmp	[esi*2+ebx+8]
	jmp	[edi*2+ebx+9]
	jmp	[ebp*2+ebx+10]

