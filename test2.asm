; 386 test code
org 0

_start:
	; test: this seems to crash our decompiler!
	db 0x8e, 0x70, 0xe9

	db 0x2e
	lodsb

	bsf	ax,bx
	bsf	cx,[si]
	bsf	dx,[ebx]
	bsf	edx,eax
	bsr	ax,bx
	bsr	cx,[si]
	bsr	dx,[ebx]
	bsr	edx,eax

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
	lgdt	[bx+si+42]
	lgdt	[eax]
	o32 lgdt [si]
	lidt	[di-4]
	lidt	[bx+di]
	lidt	[ecx]
	o32 lidt [si]
	lldt	ax
	lldt	[bx]
	lmsw	ax
	lmsw	bx
	lmsw	[si]
	lsl	ax,bx
	lsl	bx,[si]
	lsl	bx,[si+bx]
	lsl	eax,bx
	lsl	ebx,[si]
	lsl	ebx,[si+bx]
	ltr	ax
	ltr	bx
	ltr	[bx+si]
	sgdt	[si+2]
	sgdt	[bx+si+42]
	sgdt	[esi]
	o32 sgdt [si]
	sidt	[di-4]
	sidt	[bx+di]
	sidt	[ebx]
	o32 sidt [si]
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

