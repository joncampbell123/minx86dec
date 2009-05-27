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
      fidiv	dword [si]			; DA /6
      fidiv	word [si]			; DE /6

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

