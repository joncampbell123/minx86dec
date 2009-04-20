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
	add	bl,2				; 80 /0 ib           ADD r/m8,imm8     r/m = reg
	add	byte [bx+si+4],2		; 80 /0 ib           ADD r/m8,imm8     r/m = mem
	add	cx,1234				; 81 /0 iw           ADD r/m16,imm16   r/m = reg
	add	word [bx+si+4],1234		; 81 /0 iw           ADD r/m16,imm16   r/m = mem
	add	edx,12345678			; 81 /0 id           ADD r/m32,imm32   r/m = reg
	add	dword [bx+si+4],12345678	; 81 /0 id           ADD r/m32,imm32   r/m = mem
	add	bx,byte 2			; 83 /0 ib           ADD r/m16,imm8    r/m = reg   imm8 = sign extended byte
	add	word [bx+si+4],byte 8		; 83 /0 ib           ADD r/m16,imm8    r/m = mem   imm8 = sign extended byte
	add	ebx,byte -4			; 83 /0 ib           ADD r/m32,imm8    r/m = reg   imm8 = sign extended byte
	add	dword [bx+si+4],byte 8		; 83 /0 ib           ADD r/m32,imm8    r/m = mem   imm8 = sign extended byte
	add	bl,bh				; 00 /r              ADD r/m8, r8
	add	byte [bx+si+4],bl		; 00 /r              ADD r/m8, r8
	add	bx,cx				; 01 /r	             ADD r/m16, r16
	add	word [bx+si+4],bx		; 01 /r              ADD r/m16, r16
	add	ebx,ecx				; 01 /r              ADD r/m32, r32
	add	dword [bx+si+4],ebx		; 01 /r              ADD r/m32, r32
	add	bl,byte [bx+si+4]		; 02 /r              ADD r8, r/m8
	add	bx,word [bx+si+4]		; 03 /r              ADD r16, r/m16
	add	ebx,dword [bx+si+4]		; 03 /r	             ADD r32, r/m32

      addpd	xmm1,xmm2			; 66 0F 58 /r        ADDPD xmm1, xmm2/m128
      addpd	xmm1,[bx+si]			; 66 0F 58 /r        ADDPD xmm1, xmm2/m128
      addps	xmm1,xmm2			; 0F 58 /r           ADDPS xmm1, xmm2/m128
      addps	xmm1,[bx+si]			; 0F 58 /r           ADDPS xmm1, xmm2/m128
      addsd	xmm1,xmm2			; F2 0F 58 /r        ADDSD xmm1, xmm2/m128
      addsd	xmm1,[bx+si]			; F2 0F 58 /r        ADDSD xmm1, xmm2/m128
      addss	xmm1,xmm2			; F3 0F 58 /r        ADDSS xmm1, xmm2/m128
      addss	xmm1,[bx+si]			; F3 0F 58 /r        ADDSS xmm1, xmm2/m128
   addsubpd	xmm1,xmm2			; 66 0F D0 /r        ADDSUBPD xmm1, xmm2/m128
   addsubpd	xmm1,[bx+si]			; 66 0F D0 /r        ADDSUBPD xmm1, xmm2/m128
   addsubps	xmm1,xmm2			; F2 0F D0 /r        ADDSUBPS xmm1, xmm2/m128
   addsubps	xmm1,[bx+si]			; F2 0F D0 /r        ADDSUBPS xmm1, xmm2/m128

	and	al,2				; 24 ib
	and	ax,1234				; 25 iw
	and	eax,12345678			; 25 iw
	and	bl,2				; 80 /4 ib           AND r/m8,imm8     r/m = reg
	and	byte [bx+si+4],2		; 80 /4 ib           AND r/m8,imm8     r/m = mem
	and	cx,1234				; 81 /4 iw           AND r/m16,imm16   r/m = reg
	and	word [bx+si+4],1234		; 81 /4 iw           AND r/m16,imm16   r/m = mem
	and	edx,12345678			; 81 /4 id           AND r/m32,imm32   r/m = reg
	and	dword [bx+si+4],12345678	; 81 /4 id           AND r/m32,imm32   r/m = mem
	and	bx,byte 2			; 83 /4 ib           AND r/m16,imm8    r/m = reg   imm8 = sign extended byte
	and	word [bx+si+4],byte 8		; 83 /4 ib           AND r/m16,imm8    r/m = mem   imm8 = sign extended byte
	and	ebx,byte -4			; 83 /4 ib           AND r/m32,imm8    r/m = reg   imm8 = sign extended byte
	and	dword [bx+si+4],byte 8		; 83 /4 ib           AND r/m32,imm8    r/m = mem   imm8 = sign extended byte
	and	bl,bh				; 20 /r              AND r/m8, r8
	and	byte [bx+si+4],bl		; 20 /r              AND r/m8, r8
	and	bx,cx				; 21 /r	             AND r/m16, r16
	and	word [bx+si+4],bx		; 21 /r              AND r/m16, r16
	and	ebx,ecx				; 21 /r              AND r/m32, r32
	and	dword [bx+si+4],ebx		; 21 /r              AND r/m32, r32
	and	bl,byte [bx+si+4]		; 22 /r              AND r8, r/m8
	and	bx,word [bx+si+4]		; 23 /r              AND r16, r/m16
	and	ebx,dword [bx+si+4]		; 23 /r	             AND r32, r/m32

      andpd	xmm1,xmm2			; 66 0F 54 /r        ANDPD xmm1, xmm2/m128
      andps	xmm1,xmm2			; 0F 54 /r           ANDPS xmm1, xmm2/m128
     andnpd	xmm1,xmm2			; 66 0F 55 /r        ANDNPD xmm1, xmm2/m128
     andnps	xmm1,xmm2			; 0F 55 /r           ANDNPS xmm1, xmm2/m128

       arpl	ax,bx				; 63 /r              ARPL r/m16, r16
       arpl	[si],bx				; 63 /r              ARPL r/m16, r16

; THE FOLLOWING INSTRUCTIONS ARE NOT YET RECOGNIZED BY NASM
;    blendpd	xmm1,xmm2,2			; 66 0F 3A 0D /r ib  BLENDPD xmm1, xmm2/m128, imm8
;    blendps	xmm1,xmm2,3			; 66 0F 3A 0C /r ib  BLENDPS xmm1, xmm2/m128, imm8
;   blendvpd	xmm1,xmm2			; 66 0F 38 15 /r     BLENDVPD xmm1, xmm2/m128, xmm0
;   blendvps	xmm1,xmm2			; 66 0F 38 14 /r     BLENDVPS xmm1, xmm2/m128, xmm0

	bound	ax,[si]				; 62 /r              BOUND r16, m16
	bound	ebx,[eax]			; 62 /r              BOUND r32, m32

	bsf	ax,bx				; 0F BC /r           BSF r16, r/m16
	bsf	ax,[si]				; 0F BC /r           BSF r16, r/m16
	bsf	eax,ecx				; 0F BC /r           BSF r32, r/m32
	bsr	cx,dx				; 0F BD /r           BSR r16, r/m16
	bsr	cx,[si]				; 0F BD /r           BSR r16, r/m16
	bsr	ebx,edx				; 0F BC /r           BSR r32, r/m32

      bswap	eax				; 0F C8+rd           BSWAP r32
      bswap	ebx				; 0F C8+rd           BSWAP r32

;         bt	ax,bx				; 0F A3 ??           BT r/m16, r16
;         bt    ecx,edx				; 0F A3 ??           BT r/m32, r32
;         bt    si,1				; 0F BA /4 ib        BT r/m16, imm8
;         bt    edi,4				; 0F BA /4 ib        BT r/m32, imm8

;        btc	ax,bx				; 0F BB ??           BTC r/m16, r16
;        btc	esi,edi				; 0F BB ??           BTC r/m32, r32
;        btc	cx,1				; 0F BA /7 ib        BTC r/m16, imm8
;        btc	edx,8				; 0F BA /7 ib        BTC r/m32, imm8

;        btr	ax,bx				; 0F B3 ??           BTR r/m16, r16
;        btr	esi,edi				; 0F B3 ??           BTR r/m32, r32
;        btr	cx,1				; 0F BA /6 ib        BTR r/m16, imm8
;        btr	edx,8				; 0F BA /6 ib        BTR r/m32, imm8

;        bts	ax,bx				; 0F AB ??           BTS r/m16, r16
;        bts	esi,edi				; 0F AB ??           BTS r/m32, r32
;        bts	cx,1				; 0F BA /5 ib        BTS r/m16, imm8
;        bts	edx,8				; 0F BA /5 ib        BTS r/m32, imm8

	call	call1				; E8 iw              CALL rel16
call1:	call	dword call1			; E8 id              CALL rel32

	call	0x1234:0x5678			; 9A iw_ofs:iw_seg   CALL seg:offset
	call	0x1234:dword 0x56789ABC		; 9A id_ofs:iw_seg   CALL seg:offset

	call	bx				; FF /2              CALL r/m16
	call	word [si]			; FF /2              CALL r/m16
	call	dword [ebx]			; FF /2              CALL r/m32

	call	far word [si]			; FF /3              CALL r/m32
	call	far word [ebx]			; FF /3              CALL r/m32
	call	far dword [si]			; FF /3              CALL r/m48
	call	far dword [edi]			; FF /3              CALL r/m48

;	cbw					; 98                 CBW
;	cwde					; 98                 CWDE
;	clc					; F8                 CLC
;	cld					; FC                 CLD
;	clflush					; 0F AE /7           CLFLUSH
;	cli					; FA                 CLI
;	clts					; 0F 06              CLTS
;	cmc					; F5                 CMC

;	cmova					; 0F 47 /r           CMOVA
;	cmovae					; 0F 43 /r           CMOVAE
;	cmovb					; 0F 42 /r           CMOVB
;	cmovbe					; 0F 46 /r           CMOVBE
;	cmovc					; 0F 42 /r           CMOVC
;	cmove					; 0F 44 /r           CMOVE
;	cmovg					; 0F 4F /r           CMOVG
;	cmovge					; 0F 4D /r           CMOVGE
;	cmovl					; 0F 4C /r           CMOVL
;	cmovle					; 0F 4E /r           CMOVLE
;	cmovna					; 0F 46 /r           CMOVNA
;	cmovnae					; 0F 42 /r           CMOVNAE
;	cmovnb					; 0F 43 /r           CMOVNB
;	cmovnbe					; 0F 47 /r           CMOVNBE
;	cmovnc					; 0F 43 /r           CMOVNC
;	cmovne					; 0F 45 /r           CMOVNE
;	cmovng					; 0F 4E /r           CMOVNG
;	cmovnge					; 0F 4C /r           CMOVNGE
;	cmovnl					; 0F 4D /r           CMOVNL
;	cmovnle					; 0F 4F /r           CMOVNLE
;	cmovno					; 0F 41 /r           CMOVNO
;	cmovnp					; 0F 4B /r           CMOVNP
;	cmovns					; 0F 49 /r           CMOVNS
;	cmovnz					; 0F 45 /r           CMOVNZ
;	cmovo					; 0F 40 /r           CMOVO
;	cmovp					; 0F 4A /r           CMOVP
;	cmovpe					; 0F 4A /r           CMOVPE
;	cmovpo					; 0F 4B /r           CMOVPO
;	cmovs					; 0F 48 /r           CMOVS
;	cmovz					; 0F 44 /r           CMOVZ

