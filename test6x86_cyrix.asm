; 6x86 cyrix test code
org 0

_start:

	bsf	ax,bx
	bsf	cx,[si]
	bsf	dx,[ebx]
	bsf	edx,eax
	bsr	ax,bx
	bsr	cx,[si]
	bsr	dx,[ebx]
	bsr	edx,eax

	nop
	nop

	db	0xDD,0xFC		; FRICHOP
	db	0xDF,0xFC		; FRINEAR
	db	0xDB,0xFC		; FRINT2
	db	0x0F,0xFF		; "Official Undefined Opcode"

	nop
	nop
	nop
	nop
	nop
	nop

	cmpsb
	cmpsw
	cmpsd
	a32 cmpsb
	a32 cmpsw
	a32 cmpsd
	scasb
	scasw
	scasd
	a32 scasb
	a32 scasw
	a32 scasd
	lodsb
	lodsw
	lodsd
	a32 lodsb
	a32 lodsw
	a32 lodsd
	stosb
	stosw
	stosd
	a32 stosb
	a32 stosw
	a32 stosd
	movsb
	movsw
	movsd
	a32 movsb
	a32 movsw
	a32 movsd
	pusha
	popa
	pushad
	popad
	enter 1,2
	leave
	enter 400,20
	leave
	insb
	insw
	insd
	a32 insb
	a32 insw
	a32 insd
	outsb
	outsw
	outsd
	a32 outsb
	a32 outsw
	a32 outsd
	nop
	arpl	si,bx
	arpl	[si],cx
	arpl	[bx+di+3],ax
	bound	ax,[bx]
	bound	bx,[bx+si+4]
	bound	cx,[eax+ebx+3]
	bound	esi,[ebx+42h]
	lgdt	[si+2]
	lidt	[di-4]
	lgdt	[bx+si+42]
	lidt	[bx+di]
	lldt	ax
	lldt	[bx]
	lmsw	ax
	lmsw	bx
	lmsw	[si]
	lsl	ax,bx
	lsl	bx,[si]
	lsl	bx,[si+bx]
	ltr	ax
	ltr	bx
	ltr	[bx+si]
	sgdt	[si+2]
	sidt	[di-4]
	sgdt	[bx+si+42]
	sidt	[bx+di]
	sldt	ax
	sldt	[bx]
	str	ax
	str	bx
	smsw	ax
	smsw	bx
	verr	bx
	verr	cx
	verr	[si]
	verw	ax
	verw	bx
	clts
	lar	ax,bx
	lar	si,[si]
	lar	cx,[esi+edi+41]
	lea	ebx,[esi+32000h]
	lea	ecx,[ebx+edi+33222h]
	mov	ax,es
	mov	ds,ax
	mov	bx,cs
	mov	cs,bx
	mov	cx,ds
	mov	dx,ss
	mov	[edi],es
	mov	[esi],ds
	test	eax,ebx
	test	esi,ecx
	test	cl,bh
	test	[edi],bx
	test	[esi+ebx+43],cx
	xchg	bl,dl
	xchg	[esi*4+edi-82h],cl
	xchg	[edi],edx
	cmp	bl,44h
	cmp	ebx,11224466h
	cmp	byte [ebx+3],35h
	cmp	word [ebp-3],2244h
	cmp	word [edi],62h
	mov	ah,dh
	mov	bl,[si+bx+3]
	mov	si,[di+bx+34h]
	mov	edi,[ebx*8+edi+222h]
	inc	eax
	inc	ebx
	inc	ecx
	inc	edx
	inc	esi
	inc	edi
	inc	ebp
	inc	esp
	dec	esp
	dec	ebp
	dec	edi
	dec	esi
	dec	edx
	dec	ecx
	dec	ebx
	dec	eax
	push	eax
	push	ebx
	push	ecx
	push	edx
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	jo	j2
j2:	jno	j2
	ja	j2
	jz	j2
	xchg	eax,ebx
	xchg	eax,ecx
	xchg	eax,edx
	xchg	eax,esi
	mov	al,13h
	mov	bl,44h
	mov	dh,11h
	mov	esi,112456h
	mov	ebp,112222h
	daa
	das
	aaa
	aas
	add	al,ah
	add	cl,bh
	add	bl,byte [345h]
	add	byte [123h],dh
	adc	bh,byte [bx+si]
	sub	si,word [si]
	sub	di,word [bp+di+0x1234]
	add	al,12h
	add	ax,1245h
	mov	al,[1234h]
	mov	ax,[5050h]
	mov	[1111h],al
	mov	[5421h],ax
	mov	ax,[cs:3]
	mov	[ss:4444h],ax
	push	cs
	push	es
	pop	ds

; Cyrix MMX extensions
	paddsiw	mm1,mm2		; implied destination mm0
	paddsiw	mm2,mm4		; implied destination mm3
	paddsiw mm0,[esi]	; implied destination mm1

	paveb	mm0,mm2		; implied=mm1
	paveb	mm1,mm3		; implied=mm0
	paveb	mm2,[esi]	; implied=mm3

	pdistib	mm5,[esi]	; implied=mm4

	pmachriw mm1,[esi]	; implied=mm0

	pmagw	mm2,mm4		; implied=mm3
	pmagw	mm3,mm5		; implied=mm2
	pmagw	mm1,[esi]	; implied=mm0

	pmulhrwc mm0,mm2	; implied=mm1
	pmulhriw mm0,mm2	; implied=mm1
	pmulhrwc mm0,[esi]	; implied=mm1
	pmulhriw mm0,[esi]	; implied=mm1

	pmvzb	mm0,[esi]	; implied=mm1
	pmvnzb	mm0,[esi]	; implied=mm1
	pmvlzb	mm0,[esi]	; implied=mm1
	pmvgezb	mm0,[esi]	; implied=mm1

	psubsiw	mm0,mm2		; implied=mm1
	psubsiw	mm1,mm3		; implied=mm0
	psubsiw	mm1,[esi]	; implied=mm0

