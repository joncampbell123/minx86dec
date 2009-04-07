; general test code
; all opcodes entered alphabetically from Intel's guide are added here
; the ones we know we support are uncommented.
; if they show up on disassembly correctly, we know it's supported.
org 0

_start:
	aaa					; 37

	aad					; D5 0A
	aad	8				; D5 ib

	aam					; D4 0A
	aam	9				; D4 ib

	aas					; 3F

	adc	al,2				; 14 ib
	adc	ax,1234				; 15 iw
	adc	eax,12345678			; 15 iw
	adc	bl,2				; 80 /2 ib           ADC r/m8,imm8     r/m = reg
	adc	byte [bx+si+4],2		; 80 /2 ib           ADC r/m8,imm8     r/m = mem
	adc	cx,1234				; 81 /2 iw           ADC r/m16,imm16   r/m = reg
	adc	word [bx+si+4],1234		; 81 /2 iw           ADC r/m16,imm16   r/m = mem
	adc	edx,12345678			; 81 /2 id           ADC r/m32,imm32   r/m = reg
	adc	dword [bx+si+4],12345678	; 81 /2 id           ADC r/m32,imm32   r/m = mem
	adc	bx,byte 2			; 83 /2 ib           ADC r/m16,imm8    r/m = reg   imm8 = sign extended byte
	adc	word [bx+si+4],byte 8		; 83 /2 ib           ADC r/m16,imm8    r/m = mem   imm8 = sign extended byte
	adc	ebx,byte -4			; 83 /2 ib           ADC r/m32,imm8    r/m = reg   imm8 = sign extended byte
	adc	dword [bx+si+4],byte 8		; 83 /3 ib           ADC r/m32,imm8    r/m = mem   imm8 = sign extended byte
	adc	bl,bh				; 10 /r              ADC r/m8, r8
	adc	byte [bx+si+4],bl		; 10 /r              ADC r/m8, r8
	adc	bx,cx				; 11 /r	             ADC r/m16, r16
	adc	word [bx+si+4],bx		; 11 /r              ADC r/m16, r16
	adc	ebx,ecx				; 11 /r              ADC r/m32, r32
	adc	dword [bx+si+4],ebx		; 11 /r              ADC r/m32, r32
	adc	bl,byte [bx+si+4]		; 12 /r              ADC r8, r/m8
	adc	bx,word [bx+si+4]		; 13 /r              ADC r16, r/m16
	adc	ebx,dword [bx+si+4]		; 13 /r	             ADC r32, r/m32

	add	al,2				; 04 ib
	add	ax,1234				; 05 iw
	add	eax,12345678			; 05 iw
	add	bl,2				; 80 /0 ib           ADC r/m8,imm8     r/m = reg
	add	byte [bx+si+4],2		; 80 /0 ib           ADC r/m8,imm8     r/m = mem
	add	cx,1234				; 81 /0 iw           ADC r/m16,imm16   r/m = reg
	add	word [bx+si+4],1234		; 81 /0 iw           ADC r/m16,imm16   r/m = mem
	add	edx,12345678			; 81 /0 id           ADC r/m32,imm32   r/m = reg
	add	dword [bx+si+4],12345678	; 81 /0 id           ADC r/m32,imm32   r/m = mem
	add	bx,byte 2			; 83 /0 ib           ADC r/m16,imm8    r/m = reg   imm8 = sign extended byte
	add	word [bx+si+4],byte 8		; 83 /0 ib           ADC r/m16,imm8    r/m = mem   imm8 = sign extended byte
	add	ebx,byte -4			; 83 /0 ib           ADC r/m32,imm8    r/m = reg   imm8 = sign extended byte
	add	dword [bx+si+4],byte 8		; 83 /0 ib           ADC r/m32,imm8    r/m = mem   imm8 = sign extended byte
	add	bl,bh				; 00 /r              ADC r/m8, r8
	add	byte [bx+si+4],bl		; 00 /r              ADC r/m8, r8
	add	bx,cx				; 01 /r	             ADC r/m16, r16
	add	word [bx+si+4],bx		; 01 /r              ADC r/m16, r16
	add	ebx,ecx				; 01 /r              ADC r/m32, r32
	add	dword [bx+si+4],ebx		; 01 /r              ADC r/m32, r32
	add	bl,byte [bx+si+4]		; 02 /r              ADC r8, r/m8
	add	bx,word [bx+si+4]		; 03 /r              ADC r16, r/m16
	add	ebx,dword [bx+si+4]		; 03 /r	             ADC r32, r/m32

;	addpd	xmm1,xmm2			; 66 0F 58 /r        ADDPD xmm1, xmm2/m128
;	addpd	xmm1,[bx+si]			; 66 0F 58 /r        ADDPD xmm1, xmm2/m128
;	addps	xmm1,xmm2			; 0F 58 /r           ADDPS xmm1, xmm2/m128
;	addps	xmm1,[bx+si]			; 0F 58 /r           ADDPS xmm1, xmm2/m128
;	addsd	xmm1,xmm2			; F2 0F 58 /r        ADDSD xmm1, xmm2/m128
;	addsd	xmm1,[bx+si]			; F2 0F 58 /r        ADDSD xmm1, xmm2/m128
;	addss	xmm1,xmm2			; F3 0F 58 /r        ADDSS xmm1, xmm2/m128
;	addss	xmm1,[bx+si]			; F3 0F 58 /r        ADDSS xmm1, xmm2/m128
;    addsubpd	xmm1,xmm2			; 66 0F D0 /r        ADDSUBPD xmm1, xmm2/m128
;    addsubps	xmm1,xmm2			; F2 0F D0 /r        ADDSUBPS xmm1, xmm2/m128

	and	al,2				; 24 ib
	and	ax,1234				; 25 iw
	and	eax,12345678			; 25 iw
	and	bl,2				; 80 /4 ib           ADC r/m8,imm8     r/m = reg
	and	byte [bx+si+4],2		; 80 /4 ib           ADC r/m8,imm8     r/m = mem
	and	cx,1234				; 81 /4 iw           ADC r/m16,imm16   r/m = reg
	and	word [bx+si+4],1234		; 81 /4 iw           ADC r/m16,imm16   r/m = mem
	and	edx,12345678			; 81 /4 id           ADC r/m32,imm32   r/m = reg
	and	dword [bx+si+4],12345678	; 81 /4 id           ADC r/m32,imm32   r/m = mem
	and	bx,byte 2			; 83 /4 ib           ADC r/m16,imm8    r/m = reg   imm8 = sign extended byte
	and	word [bx+si+4],byte 8		; 83 /4 ib           ADC r/m16,imm8    r/m = mem   imm8 = sign extended byte
	and	ebx,byte -4			; 83 /4 ib           ADC r/m32,imm8    r/m = reg   imm8 = sign extended byte
	and	dword [bx+si+4],byte 8		; 83 /4 ib           ADC r/m32,imm8    r/m = mem   imm8 = sign extended byte
	and	bl,bh				; 20 /r              ADC r/m8, r8
	and	byte [bx+si+4],bl		; 20 /r              ADC r/m8, r8
	and	bx,cx				; 21 /r	             ADC r/m16, r16
	and	word [bx+si+4],bx		; 21 /r              ADC r/m16, r16
	and	ebx,ecx				; 21 /r              ADC r/m32, r32
	and	dword [bx+si+4],ebx		; 21 /r              ADC r/m32, r32
	and	bl,byte [bx+si+4]		; 22 /r              ADC r8, r/m8
	and	bx,word [bx+si+4]		; 23 /r              ADC r16, r/m16
	and	ebx,dword [bx+si+4]		; 23 /r	             ADC r32, r/m32

