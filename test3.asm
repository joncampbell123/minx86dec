; general test code
; all opcodes entered alphabetically from Intel's guide are added here
; the ones we know we support are uncommented.
; if they show up on disassembly correctly, we know it's supported.
org 0

_start:
; FIXME: This does NOT decode correctly at the moment
	jmp	[ebp+eax]

; this once crashed the decoder
	db	0x45,0xF7,0x07
	nop
	nop
	nop

	fld	dword [esi]
	fst	dword [esi]
	fstp	dword [esi]
	fldenv	[esi]
	fldcw	[esi]
	fnstenv	[esi]
	fnstcw	[esi]

	fld	dword [esi+4]
	fst	dword [esi+4]
	fstp	dword [esi+4]
	fldenv	[esi+4]
	fldcw	[esi+4]
	fnstenv	[esi+4]
	fnstcw	[esi+4]

	fld	dword [esi+0x444]
	fst	dword [esi+0x444]
	fstp	dword [esi+0x444]
	fldenv	[esi+0x444]
	fldcw	[esi+0x444]
	fnstenv	[esi+0x444]
	fnstcw	[esi+0x444]

	mov	cr0,eax
	mov	eax,cr0
	mov	cr1,eax
	mov	eax,cr1

	mov	dr0,eax
	mov	eax,dr0
	mov	dr1,eax
	mov	eax,dr1

	mov	tr0,eax
	mov	eax,tr0
	mov	tr1,eax
	mov	eax,tr1

	lds	si,[si]
	les	di,[si]

	fadd	st0,st1
	fmul	st0,st1
	fcom	st0,st1
	fcomp	st0,st1
	fsub	st0,st1
	fsubr	st0,st1
	fdiv	st0,st1
	fdivr	st0,st1

	fadd	st1,st0
	fmul	st1,st0
;	fcom	st1,st0
;	fcomp	st1,st0
	fsub	st1,st0
	fsubr	st1,st0
	fdiv	st1,st0
	fdivr	st1,st0

	fadd	dword [si]
	fmul	dword [si]
	fcom	dword [si]
	fcomp	dword [si]
	fsub	dword [si]
	fsubr	dword [si]
	fdiv	dword [si]
	fdivr	dword [si]

	fadd	dword [si+3]
	fmul	dword [si+3]
	fcom	dword [si+3]
	fcomp	dword [si+3]
	fsub	dword [si+3]
	fsubr	dword [si+3]
	fdiv	dword [si+3]
	fdivr	dword [si+3]

	fadd	qword [si]
	fmul	qword [si]
	fcom	qword [si]
	fcomp	qword [si]
	fsub	qword [si]
	fsubr	qword [si]
	fdiv	qword [si]
	fdivr	qword [si]

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
	adc	bx,byte -4
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

; THE FOLLOWING INSTRUCTIONS ARE NOT RECOGNIZED UNLESS USING A RECENT NASM
    blendpd	xmm1,xmm2,2			; 66 0F 3A 0D /r ib  BLENDPD xmm1, xmm2/m128, imm8
    blendps	xmm1,xmm2,3			; 66 0F 3A 0C /r ib  BLENDPS xmm1, xmm2/m128, imm8
   blendvpd	xmm1,xmm2,xmm0			; 66 0F 38 15 /r     BLENDVPD xmm1, xmm2/m128, xmm0
   blendvps	xmm1,xmm2,xmm0			; 66 0F 38 14 /r     BLENDVPS xmm1, xmm2/m128, xmm0

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

         bt	ax,bx				; 0F A3 ??           BT r/m16, r16
         bt	ecx,edx				; 0F A3 ??           BT r/m32, r32
         bt	si,1				; 0F BA /4 ib        BT r/m16, imm8
         bt	edi,4				; 0F BA /4 ib        BT r/m32, imm8

	btc	ax,bx				; 0F BB ??           BTC r/m16, r16
	btc	esi,edi				; 0F BB ??           BTC r/m32, r32
	btc	cx,1				; 0F BA /7 ib        BTC r/m16, imm8
	btc	edx,8				; 0F BA /7 ib        BTC r/m32, imm8

	btr	ax,bx				; 0F B3 ??           BTR r/m16, r16
	btr	esi,edi				; 0F B3 ??           BTR r/m32, r32
	btr	cx,1				; 0F BA /6 ib        BTR r/m16, imm8
	btr	edx,8				; 0F BA /6 ib        BTR r/m32, imm8

	bts	ax,bx				; 0F AB ??           BTS r/m16, r16
	bts	esi,edi				; 0F AB ??           BTS r/m32, r32
	bts	cx,1				; 0F BA /5 ib        BTS r/m16, imm8
	bts	edx,8				; 0F BA /5 ib        BTS r/m32, imm8

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

	cbw					; 98                 CBW
	cwde					; 98                 CWDE
	clc					; F8                 CLC
	cld					; FC                 CLD
	clflush	[bx+si]				; 0F AE /7           CLFLUSH
	cli					; FA                 CLI
	clts					; 0F 06              CLTS
	cmc					; F5                 CMC

        cmovo   cx,[bx+si]                      ; 0F 40 /r           CMOVO
        cmovno  cx,[bx+si]                      ; 0F 41 /r           CMOVNO
        cmovnae cx,[bx+si]                      ; 0F 42 /r           CMOVNAE
        cmovc   cx,[bx+si]                      ; 0F 42 /r           CMOVC
        cmovb   cx,[bx+si]                      ; 0F 42 /r           CMOVB
        cmovnb  cx,[bx+si]                      ; 0F 43 /r           CMOVNB
        cmovnc  cx,[bx+si]                      ; 0F 43 /r           CMOVNC
        cmovae  cx,[bx+si]                      ; 0F 43 /r           CMOVAE
        cmovz   cx,[bx+si]                      ; 0F 44 /r           CMOVZ
        cmove   cx,[bx+si]                      ; 0F 44 /r           CMOVE
        cmovnz  cx,[bx+si]                      ; 0F 45 /r           CMOVNZ
        cmovne  cx,[bx+si]                      ; 0F 45 /r           CMOVNE
        cmovbe  cx,[bx+si]                      ; 0F 46 /r           CMOVBE
        cmovna  cx,[bx+si]                      ; 0F 46 /r           CMOVNA
        cmova   cx,[bx+si]                      ; 0F 47 /r           CMOVA
        cmovnbe cx,[bx+si]                      ; 0F 47 /r           CMOVNBE
        cmovs   cx,[bx+si]                      ; 0F 48 /r           CMOVS
        cmovns  cx,[bx+si]                      ; 0F 49 /r           CMOVNS
        cmovp   cx,[bx+si]                      ; 0F 4A /r           CMOVP
        cmovpe  cx,[bx+si]                      ; 0F 4A /r           CMOVPE
        cmovpo  cx,[bx+si]                      ; 0F 4B /r           CMOVPO
        cmovnp  cx,[bx+si]                      ; 0F 4B /r           CMOVNP
        cmovnge cx,[bx+si]                      ; 0F 4C /r           CMOVNGE
        cmovl   cx,[bx+si]                      ; 0F 4C /r           CMOVL
        cmovge  cx,[bx+si]                      ; 0F 4D /r           CMOVGE
        cmovnl  cx,[bx+si]                      ; 0F 4D /r           CMOVNL
        cmovle  cx,[bx+si]                      ; 0F 4E /r           CMOVLE
        cmovng  cx,[bx+si]                      ; 0F 4E /r           CMOVNG
        cmovnle cx,[bx+si]                      ; 0F 4F /r           CMOVNLE
        cmovg   cx,[bx+si]                      ; 0F 4F /r           CMOVG

	cmp	al,2				; 3C ib
	cmp	ax,1234				; 3D iw
	cmp	eax,12345678			; 3D iw
	cmp	bl,2				; 80 /7 ib           AND r/m8,imm8     r/m = reg
	cmp	byte [bx+si+4],2		; 80 /7 ib           AND r/m8,imm8     r/m = mem
	cmp	cx,1234				; 81 /7 iw           AND r/m16,imm16   r/m = reg
	cmp	word [bx+si+4],1234		; 81 /7 iw           AND r/m16,imm16   r/m = mem
	cmp	edx,12345678			; 81 /7 id           AND r/m32,imm32   r/m = reg
	cmp	dword [bx+si+4],12345678	; 81 /7 id           AND r/m32,imm32   r/m = mem
	cmp	bx,byte 2			; 83 /7 ib           AND r/m16,imm8    r/m = reg   imm8 = sign extended byte
	cmp	word [bx+si+4],byte 8		; 83 /7 ib           AND r/m16,imm8    r/m = mem   imm8 = sign extended byte
	cmp	ebx,byte -4			; 83 /7 ib           AND r/m32,imm8    r/m = reg   imm8 = sign extended byte
	cmp	dword [bx+si+4],byte 8		; 83 /7 ib           AND r/m32,imm8    r/m = mem   imm8 = sign extended byte
	cmp	bl,bh				; 38 /r              AND r/m8, r8
	cmp	byte [bx+si+4],bl		; 38 /r              AND r/m8, r8
	cmp	bx,cx				; 39 /r	             AND r/m16, r16
	cmp	word [bx+si+4],bx		; 39 /r              AND r/m16, r16
	cmp	ebx,ecx				; 39 /r              AND r/m32, r32
	cmp	dword [bx+si+4],ebx		; 39 /r              AND r/m32, r32
	cmp	bl,byte [bx+si+4]		; 3A /r              AND r8, r/m8
	cmp	bx,word [bx+si+4]		; 3B /r              AND r16, r/m16
	cmp	ebx,dword [bx+si+4]		; 3B /r	             AND r32, r/m32

      cmppd	xmm1,xmm2,2			; 66 0F C2 /r ib     CMPPD xmm1, xmm2/m128, imm8
      cmppd	xmm1,[bx+si],3			; 66 0F C2 /r ib     CMPPD xmm1, xmm2/m128, imm8
      cmpps	xmm1,xmm2,4			; 0F C2 /r ib        CMPPS xmm1, xmm2/m128, imm8
      cmpps	xmm1,[bx+si],5			; 0F C2 /r ib        CMPPS xmm1, xmm2/m128, imm8

      cmpsb					; A6                 CMPSB
      cmpsw					; A7                 CMPSW
      cmpsd					; A7                 CMPSW

      cmpsd	xmm1,xmm2,2			; F2 0F C2 /r ib     CMPSD xmm1, xmm2/m128, imm8
      cmpsd	xmm1,[bx+si],3			; F2 0F C2 /r ib     CMPSD xmm1, xmm2/m128, imm8
      cmpss	xmm1,xmm2,4			; F3 0F C2 /r ib     CMPSS xmm1, xmm2/m128, imm8
      cmpss	xmm1,[bx+si],5			; F3 0F C2 /r ib     CMPSS xmm1, xmm2/m128, imm8

    cmpxchg	al,ah				; 0F B0 /r           CMPXCHG
    cmpxchg	[bx+si],cx			; 0F B0 /r           CMPXCHG
    cmpxchg     [ebx],esi			; 0F B0 /r           CMPXCHG

  cmpxchg8b	[si]				; 0F C7 /1           CMPXCHG8B
  cmpxchg8b	[ebx]				; 0F C7 /1           CMPXCHG8B

     comisd	xmm1,xmm2			; 66 0F 2F /r        COMISD
     comiss	xmm1,xmm2			; 0F 2F /r           COMISS
     
      cpuid

; THESE NEED A RECENT NASM
      crc32	eax,byte [bx+si]		; F2 0F 38 F0 /r     CRC32
      crc32	ebx,word [bx+si]		; F2 0F 38 F0 /r     CRC32
      crc32	ecx,dword [bx+si]		; F2 0F 38 F0 /r     CRC32

   cvtdq2pd	xmm1,xmm2			; F3 0F E6
   cvtdq2ps	xmm1,xmm2			; 0F 5B /r
   cvtpd2dq	xmm1,xmm2			; F2 0F E6
   cvtpd2pi	mm1,xmm2			; 66 0F 2D /r
   cvtpd2ps	xmm1,xmm2			; 66 0F 5A /r
   cvtpi2pd	xmm1,mm2			; 66 0F 2A /r
   cvtpi2ps	xmm1,mm2			; 0F 2A /r
   cvtps2dq	xmm1,xmm2			; 66 0F 5B /r
   cvtps2pd	xmm1,xmm2			; 0F 5A /r
   cvtps2pi	mm1,xmm2			; 0F 2D /r
   cvtsd2si	eax,xmm2			; F2 0F 2D /r
   cvtsd2ss	xmm1,xmm2			; F2 0F 5A /r
   cvtsi2sd	xmm1,eax			; F2 0F 2A /r
   cvtsi2ss	xmm1,ebx			; F3 0F 2A /r
   cvtss2sd	xmm1,xmm2			; F3 0F 5A /r
   cvtss2si	eax,xmm1			; F3 0F 2D /r
  cvttpd2dq	xmm1,xmm2			; 66 0F E6
  cvttpd2pi	mm1,xmm1			; 66 0F 2C /r
  cvttps2dq	xmm1,xmm2			; F3 0F 5B /r
  cvttps2pi	mm1,xmm2			; 0F 2C /r
  cvttsd2si	eax,xmm1			; F2 0F 2C /r
  cvttss2si	eax,xmm1			; F3 0F 2C /r

	cwd					; 99
	cdq					; [66] 99
	daa					; 27
	das					; 2F
	dec	ax				; 48+rw
	dec	eax				; 48+rw
	dec	ebx				; 48+rw
	dec	ecx				; 48+rw
	dec	cx				; 48+rw
	dec	edx				; 48+rw
	dec	dx				; 48+rw
	dec	esi				; 48+rw
	dec	si				; 48+rw
	dec	byte [si]			; FE /1
	dec	byte [bx+si+44h]		; FE /1
	dec	word [di]			; FF /1
	dec	word [bx+di+11h]		; FF /1
	dec	dword [ebx]			; FF /1
	div	al				; F6 /6
	div	ah				; F6 /6
	div	cl				; F6 /6
	div	dh				; F6 /6
	div	byte [si]			; F7 /6
	div	word [bx]			; F7 /6
	div	ax				; F7 /6
	div	bx				; F7 /6
	div	ecx				; F7 /6
	div	dword [esi]			; F7 /6

      divpd	xmm1,xmm2			; 66 0F 5E /r
      divps	xmm1,xmm2			;    0F 5E /r
      divsd	xmm1,xmm2			; F2 0F 5E /r
      divss	xmm1,xmm2			; F3 0F 5E /r

       dppd	xmm1,xmm2,1			; 66 0F 3A 41 /r ib
       dppd	xmm2,xmm3,4			; 66 0F 3A 41 /r ib
       dpps	xmm1,xmm2,2			; 66 0F 3A 40 /r ib
       dpps	xmm1,xmm2,5			; 66 0F 3A 40 /r ib

       emms					; 0F 77
      enter	4,0				; C8 iw 0
      enter	8,1				; C8 iw 1
      enter	14,6				; C8 iw 6

  extractps	eax,xmm1,4			; 66 0F 3A 17 /r ib
  extractps	ebx,xmm2,10			; 66 0F 3A 17 /r ib

      f2xm1					; D9 F0
       fabs					; D9 E1
       fadd	dword [esi]			; D8 /0
       fadd	qword [edi]			; DC /0
       fadd	st0,st2				; D8 C0+i
       fadd	st2,st0				; DC C0+i
      faddp	st2,st0				; DE C0+i
      faddp					; DE C1 (redundant, it's also faddp st(1),st(0) )
      fiadd	dword [eax]			; DA /0
      fiadd	word [ebx]			; DE /0
       fbld	[si]				; DF /4  mem op. is 80 bits
      fbstp	[di]				; DF /6  mem op. is 80 bits
       fchs					; D9 E0
      fclex					; 9B DB E2
     fnclex					; DB E2

     fcmovb	st0,st2				; DA C0+i
     fcmove	st0,st2				; DA C8+i
    fcmovbe	st0,st2				; DA DO+i
     fcmovu	st0,st2				; DA D8+i
    fcmovnb	st0,st2				; DB C0+i
    fcmovne	st0,st2				; DB C8+i
   fcmovnbe	st0,st2				; DB D0+i
    fcmovnu	st0,st2				; DB D8+i

      fcomi	st0,st2				; DB F0+i
     fcomip	st0,st2				; DF F0+i
     fucomi	st0,st2				; DB E8+i
    fucomip	st0,st2				; DF E8+i

       fcos					; D9 FF
    fdecstp					; D9 F6

       fdiv	dword [si]			; D8 /6
       fdiv	qword [si]			; DC /6
       fdiv	st0,st2				; D8 F0+i
       fdiv	st2,st0				; DC F8+i
      fdivp	st2,st0				; DE F8+i
      fdivp					; DE F9 (fdivp st(1),st(0) )
      fdisi
      fndisi
      feni
      fneni
      fidiv	dword [si]			; DA /6
      fidiv	word [si]			; DE /6

     fsetpm					; <- LOL so you had to set the 286 FPU into protected mode separately?

      fdivr	dword [si]			; D8 /7
      fdivr	qword [si]			; DC /7
      fdivr	st0,st2				; D8 F8+i
      fdivr	st2,st0				; DC F0+i
     fdivrp	st2,st0				; DE F0+i
     fdivrp					; DE F1
     fidivr	dword [si]			; DA /7
     fidivr	word [si]			; DE /7

      ffree	st0				; DD C0+i
      ffree	st2				; DD C0+i

      ficom	word [si]			; DE /2
      ficom	dword [si]			; DA /2
     ficomp	word [si]			; DE /3
     ficomp	dword [si]			; DA /3

       fild	word [si]			; DF /0
       fild	dword [si]			; DB /0
       fild	qword [si]			; DF /5

    fincstp					; D9 F7
      finit					; 9B DB E3
     fninit					; DB E3

       fist	word [si]			; DF /2
       fist	dword [si]			; DB /2
      fistp	word [si]			; DF /3
      fistp	dword [si]			; DB /3
      fistp	qword [si]			; DF /7
     fisttp	word [si]			; DF /1
     fisttp	dword [si]			; DB /1
     fisttp	qword [si]			; DD /1
	fld	dword [si]			; D9 /0
	fld	qword [si]			; DD /0
	fld	tword [si]			; DB /5
	fld	st2				; D9 C0+i
       fld1					; D9 E8
     fldl2t					; D9 E9
     fldl2e					; D9 EA
      fldpi					; D9 EB
     fldlg2					; D9 EC
     fldln2					; D9 ED
       fldz					; D9 EE
      fldcw	[si]				; D9 /5 (how big?)
     fldenv	[si]				; D9 /4 (14? or 28? bytes long)
       fmul	dword [si]			; D8 /1
       fmul	qword [si]			; DC /1
       fmul	st0,st2				; D8 C8+i
       fmul	st2,st0				; DC C8+i
      fmulp	st2,st0				; DE C8+i
      fmulp					; DE C9 ( st(1),st(0) )
      fimul	dword [si]			; DA /1
      fimul	word [si]			; DE /1
       fnop					; D9 D0
     fpatan					; D9 F3
      fprem					; D9 F8
     fprem1					; D9 F5
      fptan					; D9 F2
    frndint					; D9 FC
     frstor	[si]				; DD /4 ( 94 or 108 bytes? )
      fsave	[si]				; 9B DD /6
     fnsave	[si]				; DD /6 ( 94 or 108 bytes? )
     fscale					; D9 FD
       fsin					; D9 FE
    fsincos					; D9 FB
      fsqrt					; D9 FA
        fst	dword [si]			; D9 /2
        fst	qword [si]			; DD /2
	fst	st2				; DD D0+i
       fstp	dword [si]			; D9 /3
       fstp	qword [si]			; DD /3
       fstp	tword [si]			; DB /7
       fstp	st2				; DD D8+i
      fstcw	[di]				; 9B D9 /7
     fnstcw	[di]				; D9 /7
     fstenv	[si]				; 9B D9 /6
    fnstenv	[si]				; D9 /6
      fstsw	[si]				; 9B DD /7
      fstsw	ax				; 9B DF E0
     fnstsw	[si]				; DD /7
     fnstsw	ax				; DF E0
       fsub	dword [si]			; D8 /4
       fsub	qword [si]			; DC /4
       fsub	st0,st2				; D8 E0+i
       fsub	st2,st0				; DC E8+i
      fsubp	st2,st0				; DE E8+i
      fsubp					; DE E9
      fisub	dword [si]			; DA /4
      fisub	word [si]			; DE /4
      fsubr	dword [si]			; D8 /5
      fsubr	qword [si]			; DC /5
      fsubr	st0,st2				; D8 E8+i
      fsubr	st2,st0				; DC E0+i
     fsubrp	st2,st0				; DE E0+i
     fsubrp					; DE E1
     fisubr	dword [si]			; DA /5
     fisubr	word [si]			; DE /5
       ftst					; D9 E4
      fucom	st2				; DD E0+i
      fucom					; DD E1
     fucomp	st2				; DD E8+i
     fucomp					; DD E9
    fucompp					; DA E9
       fxam					; D9 E5
       fxch	st2				; D9 C8+i
       fxch					; D9 C9
    fxrstor	[si]				; 0F AE /1 ( 512 byte region )
     fxsave	[si]				; 0F AE /0 ( 512 byte region )
    fxtract					; D9 F4
      fyl2x					; D9 F1
    fyl2xp1					; D9 F9
     haddpd	xmm1,xmm2			; 66 0F 7C /r
     haddps	xmm1,xmm2			; F2 0F 7C /r
        hlt					; F4
     hsubpd	xmm1,xmm2			; 66 0F 7D /r
     hsubps	xmm1,xmm2			; F2 0F 7D /r
       idiv	al				; F6 /7
       idiv	ch				; F6 /7
       idiv	bx				; F6 /7
       idiv	dx				; F6 /7
       idiv	esi				; F6 /7
       idiv	byte [si]			; F6 /7
       idiv	word [di]			; F6 /7
       idiv	dword [ebx]			; F6 /7
       imul	ah				; F6 /5
       imul	cl				; F6 /5
       imul	bx				; F6 /5
       imul	esi				; F6 /5
       imul	byte [si]			; F6 /5
       imul	dword [ecx]			; F6 /5
         in	al,41h				; E4 ib
	 in	ax,72h				; E5 ib
	 in	eax,22h				; E5 ib
	 in	al,dx				; EC
	 in	ax,dx				; ED
	 in	eax,dx				; ED
	inc	eax				; 40+reg
	inc	bx				; 40+reg
	inc	cl				; FE /0
	inc	word [si]			; FF /0
	inc	byte [si]			; FE /0
	inc	dword [di]			; FF /0
	insb					; 6C
	insw					; 6D
	insd					; 6D
    insertps	xmm1,xmm2,2			; 66 0F 3A 21 /r ib
    insertps	xmm1,[esi],4			; 66 0F 3A 21 /r ib
         db     0xCC				; CC   (INT 3---NASM won't encode this)
	int	3				; CD 3
	int	21h				; CD imm8
       into					; CE
       invd					; 0F 08
     invlpg	[esi]				; 0F 01 /7
     invlpg	[44h]				; 0F 01 /7
       iret					; CF
      iretd					; CF
l1:
	jo	l1				; 70 ib
	jno	l1				; 71 ib
	jc	l1				; 72 ib
	jnc	l1				; 73 ib
	jz	l1				; 74 ib
	jnz	l1				; 75 ib
	jna	l1				; 76 ib
	ja	l1				; 77 ib
	js	l1				; 78 ib
	jns	l1				; 79 ib
	jp	l1				; 7A ib
	jnp	l1				; 7B ib
	jl	l1				; 7C ib
	jnl	l1				; 7D ib
	jle	l1				; 7E ib
	jnle	l1				; 7F ib
	jcxz	l1				; E3 ib
l2:
	jo	near l2				; 0F 80 iw
	jno	near l2				; 0F 81 iw
	jc	near l2				; 0F 82 iw
	jnc	near l2				; 0F 83 iw
	jz	near l2				; 0F 84 iw
	jnz	near l2				; 0F 85 iw
	jna	near l2				; 0F 86 iw
	ja	near l2				; 0F 87 iw
	js	near l2				; 0F 88 iw
	jns	near l2				; 0F 89 iw
	jp	near l2				; 0F 8A iw
	jnp	near l2				; 0F 8B iw
	jl	near l2				; 0F 8C iw
	jnl	near l2				; 0F 8D iw
	jle	near l2				; 0F 8E iw
	jnle	near l2				; 0F 8F iw

	jo	dword near l2			; 0F 80 id
	jno	dword near l2			; 0F 81 iw
	jc	dword near l2			; 0F 82 iw
	jnc	dword near l2			; 0F 83 iw
	jz	dword near l2			; 0F 84 iw
	jnz	dword near l2			; 0F 85 iw
	jna	dword near l2			; 0F 86 iw
	ja	dword near l2			; 0F 87 iw
	js	dword near l2			; 0F 88 iw
	jns	dword near l2			; 0F 89 iw
	jp	dword near l2			; 0F 8A iw
	jnp	dword near l2			; 0F 8B iw
	jl	dword near l2			; 0F 8C iw
	jnl	dword near l2			; 0F 8D iw
	jle	dword near l2			; 0F 8E iw
	jnle	dword near l2			; 0F 8F iw
l3:
	jmp	short l3			; EB ib
	jmp	near l3				; E9 iw
	jmp	dword near l3			; E9 id
	jmp	ax				; FF /4
	jmp	ecx				; FF /4
	jmp	[bx]				; FF /4
	jmp	[ecx]				; FF /4
	jmp	0x1234:0x5678			; EA iw:iw
	jmp	0x1234:dword 0x12345678		; EA iw:id

	lahf					; 9F
	lar	ax,ax				; 0F 02 /r
	lar	cx,dx				; 0F 02 /r
	lar	bx,[si]				; 0F 02 /r
	lar	bx,[eax]			; 0F 02 /r
	lddqu	xmm1,[esi]			; F2 0F F0 /r
	lddqu	xmm2,[ebx*4+ecx+8]		; F2 0F F0 /r
	ldmxcsr	[bx]				; 0F AE /2
	ldmxcsr	[esi]				; 0F AE /2
	lds	si,[bx]				; C5 /r
	lds	esi,[eax]			; C5 /r
	lss	si,[bx]				; 0F B2 /r
	lss	esi,[ebx]			; 0F B2 /r
	les	si,[bx]				; C4 /r
	les	esi,[ebx]			; C4 /r
	lfs	si,[bx]				; 0F B4 /r
	lfs	esi,[ebx]			; 0F B4 /r
	lgs	si,[bx]				; 0F B5 /r
	lgs	esi,[ebx]			; 0F B5 /r
	lea	ax,[bx+si]			; 8D /r
	lea	eax,[eax+eax]			; 8D /r
	lea	ebx,[ebx*4+ecx+16]		; 8D /r
	leave					; C9
	lfence					; 0F AE /5
	lgdt	[bx]				; 0F 01 /2
	lidt	[bx]				; 0F 01 /3
	lldt	[bx]				; 0F 00 /2
	lmsw	[bx]				; 0F 01 /6
	lock	add ax,ax			; F0, 01 /r
	lodsb					; AC
	lodsw					; AD
	lodsd					; AD
l4:
	loop	l4				; E2 ib
	loopz	l4				; E1 ib
	loopnz	l4				; E0 ib
	lsl	ax,[bx]				; 0F 03 /r
	lsl	eax,[eax]			; 0F 03 /r
	ltr	ax				; 0F 00 /3
	ltr	[bx]				; 0F 00 /3
 maskmovdqu	xmm1,xmm2			; 66 0F F7 /r
   maskmovq	mm1,mm2				; 0F F7 /r
      maxpd	xmm1,xmm2			; 66 0F 5F /r
      maxpd	xmm1,[ebx]			; 66 0F 5F /r
      maxps	xmm1,xmm2			; 0F 5F /r
      maxps	xmm1,[ebx]			; 0F 5F /r
      maxsd	xmm1,xmm2			; F2 0F 5F /r
      maxsd	xmm1,[ebx]			; F2 0F 5F /r
      maxss	xmm1,xmm2			; F3 0F 5F /r
      maxss	xmm1,[ebx]			; F3 0F 5F /r
     mfence					; 0F AE /6
      minpd	xmm1,xmm2			; 66 0F 5D /r
      minps	xmm1,xmm2			; 0F 5D /r
      minsd	xmm1,xmm2			; F2 0F 5D /r
      minss	xmm1,xmm2			; F3 0F 5D /r
    monitor					; 0F 01 C8
; oh boy! all the various flavors of MOV
	mov	cl,ch				; 88 /r
	mov	bx,si				; 89 /r
	mov	edx,esi				; 8B /r
	mov	cl,[bx]				; 88 /r
	mov	bx,[si]				; 89 /r
	mov	edx,[edx]			; 8B /r
	mov	ax,cs				; 8C /r
	mov	[bx],ds				; 8C /r
	mov	ds,bx				; 8E /r
	mov	ds,[si]				; 8E /r
	mov	al,[0x1234]			; A0 iw
	mov	al,[dword 0x12345568]		; A0 id
	mov	ax,[0x1234]			; A1 iw
	mov	ax,[dword 0x12345678]		; A1 id
	mov	eax,[0x1234]			; A1 iw
	mov	eax,[dword 0x12345678]		; A1 id
	mov	[0x1234],al			; A2 iw
	mov	[dword 0x12345568],al		; A2 id
	mov	[0x1234],ax			; A3 iw
	mov	[dword 0x12345678],ax		; A3 id
	mov	[0x1234],eax			; A3 iw
	mov	[dword 0x12345678],eax		; A3 id
	mov	cl,0x13				; B0+reg ib
	mov	si,0x1234			; B8+reg iw
	mov	ebx,0x12345667			; B8+reg id
	mov	byte [bx],0x12			; C6 /0
	mov	word [bx],0x1234		; C7 /0
	mov	dword [bx],0x12345678		; C7 /0
; now MOV with control regs
	mov	eax,cr0				; 0F 20 /r
	mov	eax,cr3				; 0F 20 /r
	mov	ebx,cr0				; 0F 20 /r
	mov	ecx,cr1				; 0F 20 /r
	mov	cr0,eax				; 0F 22 /r
	mov	cr1,ecx				; 0F 22 /r
	mov	cr3,esi				; 0F 22 /r
	mov	eax,dr0				; 0F 21 /r
	mov	ebx,dr2				; 0F 21 /r
	mov	ecx,dr3				; 0F 21 /r
	mov	dr0,esi				; 0F 23 /r
	mov	dr1,eax				; 0F 23 /r
	mov	dr3,ecx				; 0F 23 /r
;
	movapd	xmm1,xmm2			; 66 0F 28 /r
	movapd	xmm1,[ebx]			; 66 0F 28 /r
	movapd	[ebx],xmm2			; 66 0F 29 /r
	movaps	xmm1,xmm3			; 0F 28 /r
	movaps	xmm1,[ebx]			; 0F 28 /r
	movaps	[ebx],xmm2			; 0F 29 /r
	movbe	cx,word [bx]			; 0F 38 F0 /r
	movbe	ecx,dword [ebx]			; 0F 38 F0 /r
	movbe	word [ebx],ax			; 0F 38 F1 /r
	movbe	dword [ecx],esi			; 0F 38 F1 /r
	movd	mm1,eax				; 0F 6E /r
	movd	eax,mm2				; 0F 7E /r
	movd	mm2,[ebx]			; 0F 6E /r
	movd	[ecx],mm3			; 0F 7E /r
	movd	xmm1,eax			; 66 0F 6E /r
	movd	xmm2,[esi]			; 66 0F 6E /r
	movd	ebx,xmm3			; 66 0F 7E /r
	movd	[ebx],xmm4			; 66 0F 7E /r
	movddup	xmm1,xmm2			; F2 0F 12 /r
	movddup	xmm1,[ebx]			; F2 0F 12 /r
	movdqa	xmm1,xmm2			; 66 0F 6F /r
	movdqa	xmm1,[ebx]			; 66 0F 6F /r
	movdqa	[ebx],xmm2			; 66 0F 7F /r
	movdqu	xmm1,xmm2			; F3 0F 6F /r
	movdqu	xmm1,[ebx]			; F3 0F 6F /r
	movdqu	[ebx],xmm2			; F3 0F 7F /r
	movdq2q	mm1,xmm3			; F2 0F D6 /r
	movhlps	xmm1,xmm2			; 0F 12 /r
	movhpd	xmm1,[ebx]			; 66 0F 16 /r
	movhpd	[ebx],xmm1			; 66 0F 17 /r
	movhps	xmm1,[ebx]			; 0F 16 /r
	movhps	[ebx],xmm1			; 0F 17 /r
	movlhps	xmm1,xmm2			; 0F 16 /r
	movlpd	xmm1,[ebx]			; 66 0F 12 /r
	movlpd	[ebx],xmm1			; 66 0F 13 /r
	movlps	xmm1,[ebx]			; 0F 12 /r
	movlps	[ebx],xmm1			; 0F 13 /r
      movmskpd	eax,xmm1			; 66 0F 50 /r
      movmskpd	ebx,xmm6			; 66 0F 50 /r
      movmskps	ecx,xmm2			; 0F 50 /r
      movmskps  edx,xmm3			; 0F 50 /r
      movntdqa	xmm1,[esi]			; 66 0F 38 2A /r
       movntdq	[esi],xmm1			; 66 0F E7 /r
        movnti	[eax],eax			; 0F C3 /r
       movntpd	[ebx],xmm1			; 66 0F 2B /r
       movntps	[ecx],xmm3			; 0F 2B /r
        movntq	[edx],mm3			; 0F E7 /r
	movq	mm1,mm2				; 0F 6F /r
	movq	mm2,[esi]			; 0F 6F /r
	movq	[esi],mm3			; 0F 7F /r
	movq	xmm1,xmm2			; F3 0F 7E /r
	movq	xmm2,[esi]			; F3 0F 7E /r
	movq	[esi],xmm3			; 66 0F D6 /r
     movq2dq	xmm1,mm3			; F3 0F D6 /r
	movsb					; A4
	movsw					; A5
	movsd					; A6
	movsd	xmm1,xmm2			; F2 0F 10 /r
	movsd	xmm1,[esi]			; F2 0F 10 /r
	movsd	[esi],xmm1			; F2 0F 11 /r
     movshdup	xmm1,xmm2			; F3 0F 16 /r
     movshdup	xmm1,[esi]			; F3 0F 16 /r
     movsldup	xmm2,xmm3			; F3 0F 12 /r
     movsldup	xmm3,[edi]			; F3 0F 12 /r
	movss	xmm1,xmm2			; F3 0F 10 /r
	movss	xmm2,[esi]			; F3 0F 10 /r
	movss	[edi],xmm3			; F3 0F 11 /r
	movsx	ax,bl				; 0F BE /r
	movsx	ax,byte [si]			; 0F BE /r
	movsx	ebx,cl				; 0F BE /r
	movsx	ebx,byte [di]			; 0F BE /r
	movsx	ecx,si				; 0F BF /r
	movsx	edx,word [bx]			; 0F BF /r
       movupd	xmm1,xmm3			; 66 0F 10 /r
       movupd	xmm2,[esi]			; 66 0F 10 /r
       movupd	[esi],xmm4			; 66 0F 11 /r
       movups	xmm2,xmm4			; 0F 10 /r
       movups	xmm2,[esi]			; 0F 10 /r
       movups	[esi],xmm5			; 0F 11 /r
	movzx	ax,bh				; 0F B6 /r
	movzx	ax,byte [bx]			; 0F B6 /r
	movzx	ebx,cl				; 0F B6 /r
	movzx	ecx,byte [si]			; 0F B6 /r
	movzx	edx,si				; 0F B7 /r
	movzx	esi,word [bx]			; 0F B7 /r
      mpsadbw	xmm1,xmm2,3			; 66 0F 3A 42 /r ib
      mpsadbw	xmm2,xmm4,7			; 66 0F 3A 42 /r ib
	mul	bl				; F6 /4
	mul	ch				; F6 /4
	mul	byte [si]			; F6 /4
	mul	cx				; F7 /4
	mul	si				; F7 /4
	mul	word [bx]			; F7 /4
	mul	eax				; F7 /4
	mul	esi				; F7 /4
	mul	dword [bx]			; F7 /4
	mulpd	xmm1,xmm2			; 66 0F 59 /r
	mulps	xmm1,xmm2			; 0F 59 /r
	mulsd	xmm1,xmm2			; F2 0F 59 /r
	mulss	xmm1,xmm2			; F3 0F 59 /r
	mwait					; 0F 01 C9
	neg	ah				; F6 /3
	neg	byte [si]			; F6 /3
	neg	bx				; F7 /3
	neg	word [si]			; F7 /3
	neg	ecx				; F7 /3
	neg	dword [si]			; F7 /3
	nop					; 90
	nop	ax				; 0F 1F /0	WTF?
	nop	eax				; 0F 1F /0	Really?
	nop	word [si]			; 0F 1F /0
	not	dh				; F6 /2
	not	byte [si]			; F6 /2
	not	cx				; F7 /2
	not	word [di]			; F7 /2
	not	edx				; F7 /2
	not	dword [esi]			; F7 /2
	or	al,12h				; 0C ib
	or	ax,1234h			; 0D iw
	or	eax,12345678h			; 0D id
	or	bl,12h				; 80 /1 ib
	or	bx,1234h			; 81 /1 iw
	or	ebx,12345678h			; 81 /1 id
	or	byte [si],12h			; 81 /1 ib
	or	word [di],1234h			; 81 /1 iw
	or	cx,byte 12h			; 83 /1 ib
	or	edx,byte 44h			; 83 /1 ib
	or	dh,ah				; 08 /r
	or	si,di				; 09 /r
	or	dl,byte [si]			; 0A /r
	or	cx,word [di]			; 0B /r
	orpd	xmm1,xmm2			; 66 0F 56 /r
	orps	xmm1,xmm2			; 0F 56 /r
	out	12h,al				; E6 ib
	out	12h,ax				; E7 ib
	out	12h,eax				; E7 ib
	out	dx,al				; EE
	out	dx,ax				; EF
	out	dx,eax				; EF
	outsb					; 6E
	outsw					; 6F
	outsd					; 6F
	pabsb	mm1,mm2				;    0F 38 1C /r
	pabsb	xmm2,xmm3			; 66 0F 38 1C /r
	pabsw	mm2,mm3				;    0F 38 1D /r
	pabsw	xmm2,xmm4			; 66 0F 38 1D /r
	pabsd	mm3,mm1				;    0F 38 1E /r
	pabsd	xmm1,xmm3			; 66 0F 38 1E /r
       pshufb	mm1,mm2				;    0F 38 00 /r
       pshufb	xmm1,xmm2			; 66 0F 38 00 /r
       phaddw	mm1,mm2				;    0F 38 01 /r
       phaddw	xmm1,xmm2			; 66 0F 38 01 /r
       phaddd	mm3,mm4				;    0F 38 02 /r
       phaddd	xmm2,xmm6			; 66 0F 38 02 /r
      phaddsw	mm1,mm2				;    0F 38 03 /r
      phaddsw	xmm1,xmm2			; 66 0F 38 03 /r
    pmaddubsw	mm1,mm2				;    0F 38 04 /r
    pmaddubsw	xmm1,xmm2			; 66 0F 38 04 /r
       phsubw	mm1,mm2				;    0F 38 05 /r
       phsubw	xmm1,xmm2			; 66 0F 38 05 /r
       phsubd	mm1,mm2				;    0F 38 06 /r
       phsubd	xmm1,xmm2			; 66 0F 38 06 /r
      phsubsw	mm1,mm2				;    0F 38 07 /r
      phsubsw	xmm1,xmm2			; 66 0F 38 07 /r
       psignb	mm1,mm2				;    0F 38 08 /r
       psignb	xmm1,xmm2			; 66 0F 38 08 /r
       psignw	mm1,mm2				;    0F 38 09 /r
       psignw	xmm1,xmm2			; 66 0F 38 09 /r
       psignd	mm1,mm2				;    0F 38 0A /r
       psignd	xmm1,xmm2			; 66 0F 38 0A /r
     pmulhrsw	mm1,mm2				;    0F 38 0B /r
     pmulhrsw	xmm1,xmm2			; 66 0F 38 0B /r
     pblendvb	xmm1,xmm2,xmm0			; 66 0F 38 10 /r
        ptest	xmm1,xmm2			; 66 0F 38 17 /r
     pmovsxbw	xmm1,xmm2			; 66 0F 38 20 /r
     pmovsxbd	xmm1,xmm2			; 66 0F 38 21 /r
     pmovsxbq	xmm1,xmm2			; 66 0F 38 22 /r
     pmovsxwd	xmm1,xmm2			; 66 0F 38 23 /r
     pmovsxwq	xmm1,xmm2			; 66 0F 38 24 /r
     pmovsxdq	xmm1,xmm2			; 66 0F 38 25 /r
       pmuldq	xmm1,xmm2			; 66 0F 38 28 /r
      pcmpeqq	xmm2,xmm3			; 66 0F 38 29 /r
     packusdw	xmm1,xmm2			; 66 0F 38 2B /r
     pmovzxbw	xmm1,xmm2			; 66 0F 38 30 /r
     pmovzxbd	xmm1,xmm2			; 66 0F 38 31 /r
     pmovzxbq	xmm1,xmm2			; 66 0F 38 32 /r
     pmovzxwd	xmm1,xmm2			; 66 0F 38 33 /r
     pmovzxwq	xmm1,xmm2			; 66 0F 38 34 /r
     pmovzxdq	xmm1,xmm2			; 66 0F 38 35 /r
      pcmpgtq	xmm1,xmm2			; 66 0F 38 37 /r
       pminsb	xmm1,xmm2			; 66 0F 38 38 /r
       pminsd	xmm1,xmm2			; 66 0F 38 39 /r
       pminuw	xmm1,xmm2			; 66 0F 38 3A /r
       pminud	xmm1,xmm2			; 66 0F 38 3B /r
       pmaxsb	xmm1,xmm2			; 66 0F 38 3C /r
       pmaxsd	xmm2,xmm3			; 66 0F 38 3D /r
       pmaxuw	xmm3,xmm1			; 66 0F 38 3E /r
       pmaxud	xmm2,xmm3			; 66 0F 38 3F /r
       pmulld	xmm1,xmm2			; 66 0F 38 40 /r
   phminposuw	xmm1,xmm2			; 66 0F 38 41 /r
      pblendw	xmm1,xmm2,4			; 66 0F 3A 0E /r ib
      pblendw	xmm2,xmm3,6			; 66 0F 3A 0E /r ib
      palignr	mm1,mm2,2			;    0F 3A 0F /r ib
      palignr	xmm1,xmm2,3			; 66 0F 3A 0F /r ib
       pextrb	eax,xmm1,4			; 66 0F 3A 14 /r ib
       pextrd	ebx,xmm4,2			; 66 0F 3A 16 /r ib
       pinsrb	xmm1,byte [si],2		; 66 0F 3A 20 /r ib
       pinsrb	xmm1,eax,3			; 66 0F 3A 20 /r ib
       pinsrd	xmm1,dword [si],4		; 66 0F 3A 22 /r ib
       pinsrd	xmm1,eax,5			; 66 0F 3A 22 /r ib

    punpcklbw	mm1,mm2				;    0F 60 /r
    punpcklbw	xmm1,xmm2			; 66 0F 60 /r
    punpcklwd	mm1,mm2				;    0F 61 /r
    punpcklwd	xmm1,xmm2			; 66 0F 61 /r
    punpckldq	mm1,mm2				;    0F 62 /r
    punpckldq	xmm1,xmm2			; 66 0F 62 /r
     packsswb	mm1,mm2				;    0F 63 /r
     packsswb	xmm1,xmm2			; 66 0F 63 /r
     packssdw	mm1,mm2				;    0F 6B /r
     packssdw	xmm1,xmm2			; 66 0F 6B /r

%if 0
     packuswb	mm1,mm2				;    0F 67 /r
     packuswb	xmm1,xmm2			; 66 0F 67 /r
    punpckhbw	mm1,mm2				;    0F 68 /r
    punpckhbw	xmm1,xmm2			; 66 0F 68 /r
    punpckhwd	mm1,mm2				;    0F 69 /r
    punpckhwd	xmm1,xmm2			; 66 0F 69 /r
    punpckhdq	xmm1,xmm2			; 66 0F 6A /r
   punpcklqdq	xmm1,xmm2			; 66 0F 6C /r
   punpckhqdq	xmm1,xmm2			; 66 0F 6D /r

       pshufd	xmm1,xmm2,3			; 66 0F 70 /r ib
      pshufhw	xmm1,xmm2,2			; F3 0F 70 /r ib
      pshuflw	xmm1,xmm2,2			; F2 0F 70 /r ib
       pshufw	mm1,mm2,2			;    0F 70 /r ib
        psrlw	mm1,2				;    0F 71 /2 ib
        psrlw	xmm1,2				; 66 0F 71 /2 ib
        psraw	mm1,4				;    0F 71 /4 ib
        psraw	xmm1,5				; 66 0F 71 /4 ib
        psllw	mm1,3				;    0F 71 /6 ib
        psllw	xmm1,3				; 66 0F 71 /6 ib
        psrld	mm1,2				;    0F 72 /2 ib
        psrld	xmm1,3				; 66 0F 72 /2 ib
        psrad	mm1,6				;    0F 72 /4 ib
        psrad	xmm1,4				; 66 0F 72 /4 ib
        pslld	mm1,3				;    0F 72 /6 ib
        pslld	xmm1,3				; 66 0F 72 /6 ib
        psrlq	mm1,2				;    0F 73 /2 ib
        psrlq	xmm1,2				; 66 0F 73 /2 ib
       psrldq	xmm1,2				; 66 0F 73 /3 ib
        psllq	mm1,3				;    0F 73 /6 ib
        psllq	xmm1,3				; 66 0F 73 /6 ib
       pslldq	xmm1,2				; 66 0F 73 /7 ib

       pinsrw	mm1,eax,5			;    0F C4 /r ib
       pinsrw	mm2,word [si],3			;    0F C4 /r ib
       pinsrw	xmm1,ecx,2			; 66 0F C4 /r ib
       pinsrw	xmm3,word [si],1		; 66 0F C4 /r ib
       pextrw	ecx,mm2,2			;    0F C5 /r ib
       pextrw	edx,xmm1,3			; 66 0F C5 /r ib

        psrlw	mm1,mm2				;    0F D1 /r
        psrlw	xmm1,xmm2			; 66 0F D1 /r
        psrld	mm1,mm2				;    0F D2 /r
        psrld	xmm1,xmm2			; 66 0F D2 /r
        psrlq	mm1,mm2				;    0F D3 /r
        psrlq	xmm1,xmm2			; 66 0F D3 /r
	paddq	mm1,mm2				;    0F D4 /r
	paddq	xmm1,xmm2			; 66 0F D4 /r
       pmullw	mm1,mm2				;    0F D5 /r
       pmullw	xmm1,xmm2			; 66 0F D5 /r

     pmovmskb	eax,mm1				;    0F D7 /r
     pmovmskb	ebx,xmm1			; 66 0F D7 /r
      psubusb	mm1,mm2				;    0F D8 /r
      psubusb	xmm1,xmm2			; 66 0F D8 /r
      psubusw	mm1,mm2				;    0F D9 /r
      psubusw	xmm1,xmm2			; 66 0F D9 /r
       pminub	mm1,mm2				;    0F DA /r
       pminub	xmm1,xmm2			; 66 0F DA /r
         pand	mm1,mm2				;    0F DB /r
         pand	xmm1,xmm2			; 66 0F DB /r

       pmaxub	mm1,mm2				;    0F DE /r
       pmaxub	xmm1,xmm2			; 66 0F DE /r
	pandn	mm1,mm2				;    0F DF /r
	pandn	xmm1,xmm2			; 66 0F DF /r
	pavgb	mm1,mm2				;    0F E0 /r
	pavgb	xmm1,xmm2			; 66 0F E0 /r
        psraw	mm1,mm2				;    0F E1 /r
        psraw	xmm1,xmm2			; 66 0F E1 /r
        psrad	mm1,mm2				;    0F E2 /r
        psrad	xmm1,xmm2			; 66 0F E2 /r
	pavgw	mm1,mm2				;    0F E3 /r
	pavgw	xmm1,xmm2			; 66 0F E3 /r
      pmulhuw	mm1,mm2				;    0F E4 /r
      pmulhuw	xmm1,xmm2			; 66 0F E4 /r
       pmulhw	mm1,mm2				;    0F E5 /r
       pmulhw	xmm1,xmm2			; 66 0F E5 /r

       psubsb	mm1,mm2				;    0F E8 /r
       psubsb	xmm1,xmm2			; 66 0F E8 /r
       psubsw	mm1,mm2				;    0F E9 /r
       psubsw	xmm1,xmm2			; 66 0F E9 /r
       pminsw	mm2,mm3				;    0F EA /r
       pminsw	xmm2,xmm4			; 66 0F EA /r

       pmaxsw	mm2,mm4				;    0F EE /r
       pmaxsw	xmm2,xmm4			; 66 0F EE /r

        psllw	mm1,mm2				;    0F F1 /r
        psllw	xmm1,xmm2			; 66 0F F1 /r
        pslld	mm1,mm2				;    0F F2 /r
        pslld	xmm1,xmm2			; 66 0F F2 /r
        psllq	mm1,mm2				;    0F F3 /r
        psllq	xmm1,xmm2			; 66 0F F3 /r
      pmuludq	mm1,mm2				;    0F F4 /r
      pmuludq	xmm1,xmm2			; 66 0F F4 /r
      pmaddwd	mm2,mm3				;    0F F5 /r
      pmaddwd	xmm3,xmm2			; 66 0F F5 /r
       psadbw	mm1,mm2				;    0F F6 /r
       psadbw	xmm1,xmm2			;    0F F6 /r

        psubb	mm1,mm2				;    0F F8 /r
        psubb	xmm1,xmm2			; 66 0F F8 /r
        psubw	mm1,mm2				;    0F F9 /r
        psubw	xmm1,xmm2			; 66 0F F9 /r
        psubd	mm1,mm2				;    0F FA /r
        psubd	xmm1,xmm2			; 66 0F FA /r
        psubq	mm1,mm2				;    0F FB /r
        psubq	xmm1,xmm2			; 66 0F FB /r
%endif

	pause					;    F3 90
	paddb	mm1,mm2				;    0F FC /r
	paddb	xmm1,xmm2			; 66 0F FC /r
	paddw	mm1,mm2				;    0F FD /r
	paddw	xmm1,xmm2			; 66 0F FD /r
	paddd	mm1,mm2				;    0F FE /r
	paddd	xmm1,xmm2			; 66 0F FE /r
	paddsb	mm1,mm2				;    0F EC /r
	paddsb	xmm1,xmm2			; 66 0F EC /r
	paddsw	mm1,mm2				;    0F ED /r
	paddsw	xmm1,xmm2			; 66 0F ED /r
	paddusb	mm1,mm2				;    0F DC /r
	paddusb	xmm1,xmm2			; 66 0F DC /r
	paddusw	mm1,mm2				;    0F DD /r
	paddusw	xmm1,xmm2			; 66 0F DD /r
      pcmpeqb	mm1,mm2				;    0F 74 /r
      pcmpeqb	xmm1,xmm2			; 66 0F 74 /r
      pcmpeqw	mm1,mm3				;    0F 75 /r
      pcmpeqw	xmm2,xmm3			; 66 0F 75 /r
      pcmpeqd	mm2,mm3				;    0F 76 /r
      pcmpeqd	xmm1,xmm4			; 66 0F 76 /r
    pcmpestri	xmm1,xmm2,3			; 66 0F 3A 61 /r ib
    pcmpestrm	xmm1,xmm2,2			; 66 0F 3A 60 /r ib
    pcmpistri	xmm1,xmm2,4			; 66 0F 3A 63 /r ib
    pcmpistrm	xmm1,xmm2,2			; 66 0F 3A 62 /r ib
      pcmpgtb	mm1,mm2				;    0F 64 /r
      pcmpgtb	xmm1,xmm2			; 66 0F 64 /r
      pcmpgtw	mm1,mm2				;    0F 65 /r
      pcmpgtw	xmm1,xmm2			; 66 0F 65 /r
      pcmpgtd	mm1,mm2				;    0F 66 /r
      pcmpgtd	xmm1,xmm2			; 66 0F 66 /r
        pop	word [si]			; 8F /0
        pop	dword [esi]			; 8F /0
	pop	ax				; 58+reg
	pop	bx				; 58+reg
	pop	cx				; 58+reg
	pop	dx				; 58+reg
	pop	ds				; 1F
	pop	es				; 07
	pop	ss				; 17
	pop	fs				; 0F A1
	pop	gs				; 0F A9
	popa					; 61
	popad					; 61
	popcnt	ax,bx				; F3 0F B8 /r
	popcnt	eax,ebx				; F3 0F B8 /r
	popf					; 9D
	popfd					; 9D
	por	mm1,mm2				;    0F EB /r
	por	xmm1,xmm2			; 66 0F EB /r
 prefetcht0	[esi]				; 0F 18 /1
 prefetcht1	[esi]				; 0F 18 /2
 prefetcht2	[esi]				; 0F 18 /3
prefetchnta	[esi]				; 0F 18 /0
       push	word [si]			; FF /6
       push	dword [si]			; FF /6
       push	ax				; 50+reg
       push	cx				; 50+reg
       push	byte 12h			; 6A ib
       push	word 1234h			; 68 iw
       push	dword 12345678h			; 68 id
       push	cs				; 0E
       push	ss				; 16
       push	ds				; 1E
       push	es				; 06
       push	fs				; 0F A0
       push	gs				; 0F A8
      pusha					; 60
     pushad					; 60
      pushf					; 9C
     pushfd					; 9C

       pxor	mm1,mm2				;    0F EF /r
       pxor	xmm1,xmm2			; 66 0F EF /r

	rcl	bh,1				; D0 /2
        rcl	bh,cl				; D2 /2
        rcl	bh,4				; CO /2 ib
        rcl	bx,1				; D1 /2
        rcl	bx,cl				; D3 /2
        rcl	bx,3				; C1 /2 ib
        rcl	ebx,1				; D1 /2
        rcl	ebx,cl				; D3 /2
        rcl	ebx,3				; C1 /2 ib

	rcr	bh,1				; D0 /3
        rcr	bh,cl				; D2 /3
        rcr	bh,4				; CO /3 ib
        rcr	bx,1				; D1 /3
        rcr	bx,cl				; D3 /3
        rcr	bx,3				; C1 /3 ib
        rcr	ebx,1				; D1 /3
        rcr	ebx,cl				; D3 /3
        rcr	ebx,3				; C1 /3 ib
      rcpps	xmm1,xmm2			;    0F 53 /r
      rcpss	xmm1,xmm2			; F3 0F 53 /r
      rdmsr					; 0F 32
      rdpmc					; 0F 33
      rdtsc					; 0F 31
     rdtscp					; 0F 01 F9
	rep	movsb				; F3 A4
	rep	movsw				; F3 A5
	rep	movsd				; 66 F3 A5
	ret					; C3
       retf					; CB
	ret	4				; C2 ib
       retf	4				; CA ib
    roundpd	xmm1,xmm2,2			; 66 0F 3A 09 /r ib
    roundps	xmm1,xmm2,3			; 66 0F 3A 08 /r ib
    roundsd	xmm1,xmm2,2			; 66 0F 3A 0B /r ib
    roundss	xmm1,xmm2,2			; 66 0F 3A 0A /r ib
	rsm					; 0F AA
    rsqrtps	xmm1,xmm2			;    0F 52 /r
    rsqrtss	xmm1,xmm2			; F3 0F 52 /r
       sahf					; 9E

; SAL = SHL
        shl	bl,1				; D0 /4
        shl	bl,cl				; D2 /4
        shl	bl,4				; C0 /4 ib
        shl	bx,1				; D1 /4
        shl	bx,cl				; D3 /4
        shl	bx,4				; C1 /4 ib
        shl	ebx,1				; D1 /4
        shl	ebx,cl				; D3 /4 ib
        shl	ebx,4				; C1 /4 ib

        shr	bl,1				; D0 /5
        shr	bl,cl				; D2 /5
        shr	bl,4				; C0 /5 ib
        shr	bx,1				; D1 /5
        shr	bx,cl				; D3 /5
        shr	bx,4				; C1 /5 ib
        shr	ebx,1				; D1 /5
        shr	ebx,cl				; D3 /5 ib
        shr	ebx,4				; C1 /5 ib

        sar	bl,1				; D0 /7
        sar	bl,cl				; D2 /7
        sar	bl,4				; C0 /7 ib
        sar	bx,1				; D1 /7
        sar	bx,cl				; D3 /7
        sar	bx,4				; C1 /7 ib
        sar	ebx,1				; D1 /7
        sar	ebx,cl				; D3 /7 ib
        sar	ebx,4				; C1 /7 ib

	sbb	al,12h				; 1C ib
	sbb	ax,1234h			; 1D iw
	sbb	eax,12345678h			; 1D id
	sbb	ch,12h				; 80 /3 ib
	sbb	bx,1234h			; 81 /3 iw
	sbb	eax,12345678h			; 81 /3 id
	sbb	ax,12h				; 83 /3 ib
	sbb	eax,12h				; 83 /3 ib
	sbb	cl,bl				; 18 /r
	sbb	cx,bx				; 19 /r
	sbb	cl,byte [si]			; 1A /r
	sbb	cx,word [si]			; 1B /r

      scasb					; AE
      scasw					; AF
      scasd					; AF

	seto	al				; 0F 90
	setno	al				; 0F 91
	setb	al				; 0F 92
	setnb	al				; 0F 93
	sete	al				; 0F 94
	setne	al				; 0F 95
	setbe	al				; 0F 96
	setnbe	al				; 0F 97
	sets	al				; 0F 98
	setns	al				; 0F 99
	setp	al				; 0F 9A
	setnp	al				; 0F 9B
	setl	al				; 0F 9C
	setnl	al				; 0F 9D
	setle	al				; 0F 9E
	setnle	al				; 0F 9F

	seto	bl				; 0F 90
	setno	bl				; 0F 91
	setb	bl				; 0F 92
	setnb	bl				; 0F 93
	sete	bl				; 0F 94
	setne	bl				; 0F 95
	setbe	bl				; 0F 96
	setnbe	bl				; 0F 97
	sets	bl				; 0F 98
	setns	bl				; 0F 99
	setp	bl				; 0F 9A
	setnp	bl				; 0F 9B
	setl	bl				; 0F 9C
	setnl	bl				; 0F 9D
	setle	bl				; 0F 9E
	setnle	bl				; 0F 9F

	seto	[bx]				; 0F 90

	sfence					; 0F AE /7
	sgdt	[bx]				; 0F 01 /0

	shld	ax,bx,3				; 0F A4 /r ib
	shld	ax,bx,cl			; 0F A5 /r
	shld	eax,ebx,3			; 0F A4 /r ib
	shld	eax,ebx,cl			; 0F A5 /r

	shrd	ax,bx,3				; 0F AC /r ib
	shrd	ax,bx,cl			; 0F AD /r
	shrd	eax,ebx,3			; 0F AC /r ib
	shrd	eax,ebx,cl			; 0F AD /r

     shufpd	xmm1,xmm2,3			; 66 0F C6 /r ib
     shufps	xmm1,xmm2,3			;    0F C6 /r ib

       sidt	[bx]				; 0F 01 /1
       sldt	[bx]				; 0F 00 /0
       smsw	[bx]				; 0F 01 /4
     sqrtps	xmm1,xmm2			;    0F 51 /r
     sqrtsd	xmm1,xmm2			; F2 0F 51 /r
     sqrtss	xmm1,xmm2			; F3 0F 51 /r
	stc					; F9
	std					; FD
	sti					; FB
    stmxcsr	[esi]				; 0F AE /3
	stosb					; AA
	stosw					; AB
	stosd					; AB
	str	[bx]				; 0F 00 /1

	sub	al,12h				; 2C ib
	sub	ax,1234h			; 2D iw
	sub	eax,12345678h			; 2D id
	sub	bl,12h				; 80 /5 ib
	sub	bx,1234h			; 81 /5 iw
	sub	ebx,12345678h			; 81 /5 id
	sub	bx,12h				; 83 /5 ib
	sub	ebx,12h				; 83 /5 ib
	sub	bl,cl				; 28 /r
	sub	bx,cx				; 29 /r
	sub	byte [si],cl			; 2A /r
	sub	word [si],cx			; 2B /r

      subpd	xmm1,xmm2			; 66 0F 5C /r
      subps	xmm1,xmm2			;    0F 5C /r
      subsd	xmm1,xmm2			; F2 0F 5C /r
      subss	xmm1,xmm2			; F3 0F 5C /r

; this is here for completeness, we're just abusing NASM's trust to get it in here so we can test decoding it.
; the instruction is not valid in 16- and 32-bit modes, so we have to tell NASM we're assembling 64-bit here.
bits 64
     swapgs					; 0F 01 /7
bits 16

    syscall					; 0F 05
   sysenter					; 0F 34
    sysexit					; 0F 35
     sysret					; 0F 07

	test	al,12h				; A8 ib
	test	ax,1234h			; A9 iw
	test	eax,12345678h			; A9 id
	test	cl,12h				; F6 /0 ib
	test	cx,1234h			; F7 /0 iw
	test	ecx,12345678h			; F7 /0 id
	test	cl,dh				; 84 /r
	test	cx,dx				; 85 /r
	test	ebx,edx				; 85 /r

     ucomisd	xmm1,xmm2			; 66 0F 2E /r
     ucomiss	xmm1,xmm2			;    0F 2E /r

	ud2					; 0F 0B

    unpckhpd	xmm1,xmm2			; 66 0F 15 /r
    unpckhps	xmm1,xmm2			;    0F 15 /r
    unpcklpd	xmm1,xmm2			; 66 0F 14 /r
    unpcklps	xmm1,xmm2			;    0F 14 /r

	verr	ax				; 0F 00 /4
	verr	[bx]				; 0F 00 /4
	verw	ax				; 0F 00 /5
	verw	[bx]				; 0F 00 /5

	wait					; 9B
	fwait					; 9B

	wbinvd					; 0F 09
	wrmsr					; 0F 30
	xadd	cl,ch				; 0F C0 /r
	xadd	bx,cx				; 0F C1 /r
	xadd	eax,ebx				; 0F C1 /r
	xchg	al,ah				; 86 /r
	xchg	ax,bx				; 90+reg
	xchg	eax,ebx				; 90+reg
	xchg	cx,bx				; 87 /r
	xchg	ecx,ebx				; 87 /r
	xgetbv					; 0F 01 D0
	xlat					; D7

	xor	al,12h				; 34 ib
	xor	ax,1234h			; 35 iw
	xor	eax,12345678h			; 35 id
	xor	cl,12h				; 80 /6 ib
	xor	cx,1234h			; 81 /6 iw
	xor	ecx,12345678h			; 81 /6 id
	xor	cx,12h				; 83 /6 ib
	xor	ecx,12h				; 83 /6 ib
	xor	ah,cl				; 30 /r
	xor	bx,dx				; 31 /r
	xor	ebx,edx				; 31 /r
	xor	byte [si],dl			; 32 /r
	xor	word [si],bx			; 33 /r

	xorpd	xmm1,xmm2			; 66 0F 57 /r
	xorps	xmm1,xmm2			;    0F 57 /r
	xrstor	[esi]				; 0F AE /5
	xsave	[esi]				; 0F AE /4
	xsetbv					; 0F 01 D1

; and now the VMX extensions!
	invept	eax,[esi]			; 66 0F 38 80
	invvpid	eax,[esi]			; 66 0F 38 81
	vmcall					; 0F 01 C1
	vmclear	[esi]				; 66 0F C7 /6
	vmlaunch				; 0F 01 C2
	vmresume				; 0F 01 C3
	vmptrld	[esi]				; 0F C7 /6
	vmptrst	[esi]				; 0F C7 /7
	vmread	[esi],eax			; 0F 78
	vmwrite	eax,[esi]			; 0F 79
	vmxoff					; 0F 01 C4
	vmxon	[esi]				; F3 0F C7 /6

; SMX extensions
	getsec					; 0F 37

; AES
	aesenc	xmm1,xmm2
	aesenc	xmm1,[esi]
	aesenc	xmm3,xmm4

	aesenclast xmm1,xmm2
	aesenclast xmm1,[esi]
	aesenclast xmm3,xmm4

	aesdec	xmm1,xmm2
	aesdec	xmm1,[esi]
	aesdec	xmm3,xmm4

	aesdeclast xmm1,xmm2
	aesdeclast xmm1,[esi]
	aesdeclast xmm3,xmm4

	aesimc	xmm1,xmm2
	aesimc	xmm3,xmm4

; AVX/VEX
	vaddpd	xmm1,xmm2,xmm3			; VEX.NDS.128.66.0F 58
	vaddpd	xmm1,xmm2,[esi]			; VEX.NDS.128.66.0F 58
	vaddpd	ymm1,ymm2,ymm3			; VEX.NDS.256.66.0F 58
	vaddpd	ymm1,ymm2,[esi]			; VEX.NDS.256.66.0F 58

	vaddps	xmm1,xmm2,xmm3			; VEX.NDS.128.66.0F 58
	vaddps	xmm1,xmm2,[esi]			; VEX.NDS.128.66.0F 58
	vaddps	ymm1,ymm2,ymm3			; VEX.NDS.256.66.0F 58
	vaddps	ymm1,ymm2,[esi]			; VEX.NDS.256.66.0F 58

	vaddsd	xmm1,xmm2,xmm3			; VEX.NDS.128.66.0F 58
	vaddsd	xmm1,xmm2,[esi]			; VEX.NDS.128.66.0F 58

	vaddss	xmm1,xmm2,xmm3			; VEX.NDS.128.66.0F 58
	vaddss	xmm1,xmm2,[esi]			; VEX.NDS.128.66.0F 58

	vaddsubpd xmm1,xmm2,xmm3
	vaddsubpd xmm1,xmm2,[esi]
	vaddsubpd ymm1,ymm2,ymm3
	vaddsubpd ymm1,ymm2,[esi]

	vaddsubps xmm1,xmm2,xmm3
	vaddsubps xmm1,xmm2,[esi]
	vaddsubps ymm1,ymm2,ymm3
	vaddsubps ymm1,ymm2,[esi]

	vaesenc	xmm1,xmm2,xmm3
	vaesenc	xmm1,xmm2,[esi]
	vaesenc	xmm3,xmm4,xmm5

	vaesenclast xmm1,xmm2,xmm3
	vaesenclast xmm1,xmm2,[esi]
	vaesenclast xmm3,xmm4,xmm5

	vaesdec	xmm1,xmm2,xmm3
	vaesdec	xmm1,xmm2,[esi]
	vaesdec	xmm3,xmm4,xmm5

	vaesdeclast xmm1,xmm2,xmm3
	vaesdeclast xmm1,xmm2,[esi]
	vaesdeclast xmm3,xmm4,xmm5

	vaesimc	xmm1,xmm2
	vaesimc	xmm3,xmm4

	aeskeygenassist xmm1,xmm2,3
	vaeskeygenassist xmm3,xmm4,5

        vandpd	xmm1,xmm2,xmm3
        vandps	xmm1,xmm2,xmm3
        vandnpd	xmm1,xmm2,xmm3
        vandnps	xmm1,xmm2,xmm3

        vandpd	ymm1,ymm2,ymm3
        vandps	ymm1,ymm2,ymm3
        vandnpd	ymm1,ymm2,ymm3
        vandnps	ymm1,ymm2,ymm3

        vblendpd xmm1,xmm2,xmm3,4			; 66 0F 3A 0D /r ib  BLENDPD xmm1, xmm2/m128, imm8
        vblendps xmm1,xmm2,xmm3,4			; 66 0F 3A 0C /r ib  BLENDPS xmm1, xmm2/m128, imm8
        vblendvpd xmm1,xmm2,xmm3,xmm4			; 66 0F 38 15 /r     BLENDVPD xmm1, xmm2/m128, xmm0
        vblendvps xmm1,xmm2,xmm3,xmm4			; 66 0F 38 14 /r     BLENDVPS xmm1, xmm2/m128, xmm0

        vblendpd ymm1,ymm2,ymm3,4			; 66 0F 3A 0D /r ib  BLENDPD xmm1, xmm2/m128, imm8
        vblendps ymm1,ymm2,ymm3,4			; 66 0F 3A 0C /r ib  BLENDPS xmm1, xmm2/m128, imm8
        vblendvpd ymm1,ymm2,ymm3,ymm4			; 66 0F 38 15 /r     BLENDVPD xmm1, xmm2/m128, xmm0
        vblendvps ymm1,ymm2,ymm3,ymm4			; 66 0F 38 14 /r     BLENDVPS xmm1, xmm2/m128, xmm0

	vbroadcastss xmm1,[esi]
	vbroadcastss ymm1,[esi]
	vbroadcastsd ymm1,[esi]
	vbroadcastf128 ymm1,[esi]

        vcmppd	xmm1,xmm2,xmm3,2			; 66 0F C2 /r ib     CMPPD xmm1, xmm2/m128, imm8
        vcmppd	xmm1,xmm2,[bx+si],3			; 66 0F C2 /r ib     CMPPD xmm1, xmm2/m128, imm8
        vcmpps	xmm1,xmm2,xmm3,4			; 0F C2 /r ib        CMPPS xmm1, xmm2/m128, imm8
        vcmpps	xmm1,xmm2,[bx+si],5			; 0F C2 /r ib        CMPPS xmm1, xmm2/m128, imm8

        vcmppd	ymm1,ymm2,ymm3,2			; 66 0F C2 /r ib     CMPPD xmm1, xmm2/m128, imm8
        vcmppd	ymm1,ymm2,[bx+si],3			; 66 0F C2 /r ib     CMPPD xmm1, xmm2/m128, imm8
        vcmpps	ymm1,ymm2,ymm3,4			; 0F C2 /r ib        CMPPS xmm1, xmm2/m128, imm8
        vcmpps	ymm1,ymm2,[bx+si],5			; 0F C2 /r ib        CMPPS xmm1, xmm2/m128, imm8

        vcmpsd	xmm1,xmm2,xmm3,2			; F2 0F C2 /r ib     CMPSD xmm1, xmm2/m128, imm8
        vcmpsd	xmm1,xmm2,[bx+si],3			; F2 0F C2 /r ib     CMPSD xmm1, xmm2/m128, imm8
        vcmpss	xmm1,xmm2,xmm3,4			; F3 0F C2 /r ib     CMPSS xmm1, xmm2/m128, imm8
        vcmpss	xmm1,xmm2,[bx+si],5			; F3 0F C2 /r ib     CMPSS xmm1, xmm2/m128, imm8

        vcomisd	xmm1,xmm2			; 66 0F 2F /r        COMISD
        vcomiss	xmm1,xmm2			; 0F 2F /r           COMISS

        vcvtdq2pd xmm1,xmm2			; F3 0F E6
        vcvtdq2ps xmm1,xmm2			; 0F 5B /r
        vcvtpd2dq xmm1,xmm2			; F2 0F E6

        vcvtdq2pd ymm1,xmm2			; F3 0F E6
        vcvtdq2ps ymm1,ymm2			; 0F 5B /r
        vcvtpd2dq xmm1,ymm2			; F2 0F E6

        vcvtdq2pd xmm1,[esi]			; F3 0F E6
        vcvtdq2ps xmm1,[esi]			; 0F 5B /r
        vcvtpd2dq xmm1,oword [esi]			; F2 0F E6

        vcvtdq2pd ymm1,[esi]			; F3 0F E6
        vcvtdq2ps ymm1,[esi]			; 0F 5B /r

        vcvtdq2pd	xmm1,xmm2			; F3 0F E6
        vcvtdq2ps	xmm1,xmm2			; 0F 5B /r
        vcvtpd2dq	xmm1,xmm2			; F2 0F E6
        vcvtpd2ps	xmm1,xmm2			; 66 0F 5A /r
        vcvtpd2ps	xmm1,ymm2			; 66 0F 5A /r
        vcvtps2dq	xmm1,xmm2			; 66 0F 5B /r
        vcvtps2dq	ymm1,ymm2			; 66 0F 5B /r
        vcvtps2pd	xmm1,xmm2			; 0F 5A /r
        vcvtps2pd	ymm1,xmm2			; 0F 5A /r
        vcvtsd2si	eax,xmm2			; F2 0F 2D /r
        vcvtsd2ss	xmm1,xmm2			; F2 0F 5A /r
        vcvtsi2sd	xmm1,eax			; F2 0F 2A /r
        vcvtsi2ss	xmm1,ebx			; F3 0F 2A /r
        vcvtss2sd	xmm1,xmm2			; F3 0F 5A /r
        vcvtss2si	eax,xmm1			; F3 0F 2D /r
        vcvttpd2dq	xmm1,xmm2			; 66 0F E6
        vcvttps2dq	xmm1,xmm2			; F3 0F 5B /r
        vcvttsd2si	eax,xmm1			; F2 0F 2C /r
        vcvttss2si	eax,xmm1			; F3 0F 2C /r

        vdivpd	xmm1,xmm2,xmm3			; 66 0F 5E /r
        vdivps	xmm1,xmm2,xmm3			;    0F 5E /r
        vdivsd	xmm1,xmm2,xmm3			; F2 0F 5E /r
        vdivss	xmm1,xmm2,xmm3			; F3 0F 5E /r
        vdivpd	ymm1,ymm2,ymm3			; 66 0F 5E /r
        vdivps	ymm1,ymm2,ymm3			;    0F 5E /r

        vdppd	xmm1,xmm2,xmm3,1			; 66 0F 3A 41 /r ib
        vdppd	xmm1,xmm2,xmm3,4			; 66 0F 3A 41 /r ib
        vdpps	xmm1,xmm2,xmm3,2			; 66 0F 3A 40 /r ib
        vdpps	xmm1,xmm2,xmm3,5			; 66 0F 3A 40 /r ib

        vhaddpd	xmm1,xmm2,xmm3			; 66 0F 7C /r
        vhaddps	xmm1,xmm2,xmm3			; F2 0F 7C /r
        vhsubpd	xmm1,xmm2,xmm3			; 66 0F 7D /r
        vhsubps	xmm1,xmm2,xmm3			; F2 0F 7D /r

	vlddqu	xmm1,[esi]			; F2 0F F0 /r
	vlddqu	xmm2,[ebx*4+ecx+8]		; F2 0F F0 /r
	vlddqu	ymm1,[esi]			; F2 0F F0 /r
	vlddqu	ymm2,[ebx*4+ecx+8]		; F2 0F F0 /r

        vmaxpd	xmm1,xmm2,xmm3			; 66 0F 5F /r
        vmaxpd	xmm1,xmm2,[ebx]			; 66 0F 5F /r
        vmaxps	xmm1,xmm2,xmm3			; 0F 5F /r
        vmaxps	xmm1,xmm2,[ebx]			; 0F 5F /r
        vmaxsd	xmm1,xmm2,xmm3			; F2 0F 5F /r
        vmaxsd	xmm1,xmm2,[ebx]			; F2 0F 5F /r
        vmaxss	xmm1,xmm2,xmm3			; F3 0F 5F /r
        vmaxss	xmm1,xmm2,[ebx]			; F3 0F 5F /r

        vmaxpd	ymm1,ymm2,ymm3			; 66 0F 5F /r
        vmaxpd	ymm1,ymm2,[ebx]			; 66 0F 5F /r
        vmaxps	ymm1,ymm2,ymm3			; 0F 5F /r
        vmaxps	ymm1,ymm2,[ebx]			; 0F 5F /r

        vminpd	xmm1,xmm2,xmm3			; 66 0F 5D /r
        vminps	xmm1,xmm2,xmm3			; 0F 5D /r
        vminsd	xmm1,xmm2,xmm3			; F2 0F 5D /r
        vminss	xmm1,xmm2,xmm3			; F3 0F 5D /r

        vminpd	ymm1,ymm2,ymm3			; 66 0F 5D /r
        vminps	ymm1,ymm2,ymm3			; 0F 5D /r

	vmovapd	xmm1,xmm2			; 66 0F 28 /r
	vmovapd	xmm1,[ebx]			; 66 0F 28 /r
	vmovapd	[ebx],xmm2			; 66 0F 29 /r
	vmovaps	xmm1,xmm3			; 0F 28 /r
	vmovaps	xmm1,[ebx]			; 0F 28 /r
	vmovaps	[ebx],xmm2			; 0F 29 /r

	vmovapd	ymm1,ymm2			; 66 0F 28 /r
	vmovapd	ymm1,[ebx]			; 66 0F 28 /r
	vmovapd	[ebx],ymm2			; 66 0F 29 /r
	vmovaps	ymm1,ymm3			; 0F 28 /r
	vmovaps	ymm1,[ebx]			; 0F 28 /r
	vmovaps	[ebx],ymm2			; 0F 29 /r

	vmovq	xmm1,xmm2			; F3 0F 7E /r
	vmovq	xmm2,[esi]			; F3 0F 7E /r
	vmovq	[esi],xmm3			; 66 0F D6 /r

	vmovd	xmm1,eax			; 66 0F 6E /r
	vmovd	xmm2,[esi]			; 66 0F 6E /r
	vmovd	ebx,xmm3			; 66 0F 7E /r
	vmovd	[ebx],xmm4			; 66 0F 7E /r

	vmovddup xmm1,xmm2			; F2 0F 12 /r
	vmovddup xmm1,[ebx]			; F2 0F 12 /r

	vmovddup ymm1,ymm2			; F2 0F 12 /r
	vmovddup ymm1,[ebx]			; F2 0F 12 /r

	vmovdqa	xmm1,xmm2			; 66 0F 6F /r
	vmovdqa	xmm1,[ebx]			; 66 0F 6F /r
	vmovdqa	[ebx],xmm2			; 66 0F 7F /r

	vmovdqa	ymm1,ymm2			; 66 0F 6F /r
	vmovdqa	ymm1,[ebx]			; 66 0F 6F /r
	vmovdqa	[ebx],ymm2			; 66 0F 7F /r

	vmovdqu	xmm1,xmm2			; F3 0F 6F /r
	vmovdqu	xmm1,[ebx]			; F3 0F 6F /r
	vmovdqu	[ebx],xmm2			; F3 0F 7F /r

	vmovdqu	ymm1,ymm2			; F3 0F 6F /r
	vmovdqu	ymm1,[ebx]			; F3 0F 6F /r
	vmovdqu	[ebx],ymm2			; F3 0F 7F /r

	vmovhlps xmm1,xmm2,xmm3			; 0F 12 /r
        vmovsldup xmm1,xmm2			; F3 0F 12 /r
        vmovsldup xmm1,[edi]			; F3 0F 12 /r
        vmovsldup ymm1,ymm2			; F3 0F 12 /r
        vmovsldup ymm1,[edi]			; F3 0F 12 /r

; 3DNow!
	femms
	pavgusb    mm0,mm1
	pavgusb    mm1,[ebx]
	pf2id      mm0,mm1
	pf2id      mm1,[ebx]
	pfacc      mm0,mm1
	pfacc      mm1,[ebx]
	pfadd      mm0,mm1
	pfadd      mm1,[ebx]
	pfcmpeq    mm0,mm1
	pfcmpeq    mm1,[ebx]
	pfcmpge    mm0,mm1
        pfcmpgt    mm0,mm1
        pfmax      mm0,mm1
        pfmin      mm0,mm1
        pfmul      mm0,mm1
        pfrcp      mm0,mm1
        pfrcpit1   mm0,mm1
        pfrcpit2   mm0,mm1
        pfrsqit1   mm0,mm1
        pfrsqrt    mm0,mm1
        pfsub      mm0,mm1
        pfsubr     mm0,mm1
        pi2fd      mm0,mm1
        pmulhrwa   mm0,mm1			; NASM docs say the 'A' is to differentiate from Cyrix's version
	prefetch   [esi]
	prefetchw  [esi]
; 3DNow extensions
	pf2iw      mm0,mm1
        pi2fw      mm0,mm1
        pswapd     mm0,mm1
        pfnacc     mm0,mm1
        pfpnacc    mm0,mm1
; Geode instructions
	pfrcpv     mm0,mm1
        pfrsqrtv   mm0,mm1
; VIA padlock
	montmul
	rep montmul
	xstore
	rep xstore
	xcryptecb
	rep xcryptecb
	xcryptcbc
	rep xcryptcbc
	xcryptctr
	rep xcryptctr
	xcryptcfb
	rep xcryptcfb
	xcryptofb
	rep xcryptofb
	xsha1
	rep xsha1
	xsha256
	rep xsha256
; segment prefixes
	mov	ax,[cs:si]
	mov	ax,[ds:si]
	mov	ax,[es:si]
	mov	ax,[fs:si]
	mov	ax,[gs:si]
	mov	ax,[ss:si]
; UMC
	db 0x64,0xD6			; SALC FS:
; more
	movq2dq	xmm1,mm0
	movdq2q	mm0,xmm1
	vpermil2ps	xmm0,xmm1,xmm2,xmm3,1
	vpermil2ps	xmm0,xmm1,[esi],xmm3,1
	vpermil2pd	xmm0,xmm1,xmm2,xmm3,1
	vpermil2pd	xmm0,xmm1,[esi],xmm3,1

