; 8086 test code
org 0

_start:
	lea	bx,[si+3]
	lea	cx,[bx+di+33h]
	lea	eax,[ebx+0x1234]
	lea	ebx,[esi*4+ecx+0x22]

; test implied SS register
	mov	[si],ax
	mov	[di],bx
	mov	[bp],dx
	mov	[bx],si
; test explicit segment override
	mov	[cs:si],ax
	mov	[ds:di],bx
	mov	[es:bp],dx
	mov	[ss:bx],si

; 486 decoder crash test
	db	0xFF,0xE5,0x00,0x00

	nop
	nop
	nop

	add	bl,[esp]
	add	bl,[a32 12345678h]
	add	bl,[a32 12345678h+edx+eax]
	add	bl,[ebp]
	add	bl,[ebp+42h]

	add	[esp],bl
	add	[a32 12345678h],bl
	add	[a32 12345678h+edx+eax],bl
	add	[ebp],bl
	add	[ebp+42h],bl

	add	bx,[esp]
	add	bx,[a32 12345678h]
	add	bx,[a32 12345678h+edx+eax]
	add	bx,[ebp]
	add	bx,[ebp+42h]

	add	[esp],bx
	add	[a32 12345678h],bx
	add	[a32 12345678h+edx+eax],bx
	add	[ebp],bx
	add	[ebp+42h],bx

	add	ebx,[esp]
	add	ebx,[a32 12345678h]
	add	ebx,[a32 12345678h+edx+eax]
	add	ebx,[ebp]
	add	ebx,[ebp+42h]

	add	[esp],ebx
	add	[a32 12345678h],ebx
	add	[a32 12345678h+edx+eax],ebx
	add	[ebp],ebx
	add	[ebp+42h],ebx

	cmp	bl,[esp]
	cmp	bl,[a32 12345678h]
	cmp	bl,[a32 12345678h+edx+eax]
	cmp	bl,[ebp]
	cmp	bl,[ebp+42h]

	cmp	[esp],bl
	cmp	[a32 12345678h],bl
	cmp	[a32 12345678h+edx+eax],bl
	cmp	[ebp],bl
	cmp	[ebp+42h],bl

	cmp	bx,[esp]
	cmp	bx,[a32 12345678h]
	cmp	bx,[a32 12345678h+edx+eax]
	cmp	bx,[ebp]
	cmp	bx,[ebp+42h]

	cmp	[esp],bx
	cmp	[a32 12345678h],bx
	cmp	[a32 12345678h+edx+eax],bx
	cmp	[ebp],bx
	cmp	[ebp+42h],bx

	cmp	ebx,[esp]
	cmp	ebx,[a32 12345678h]
	cmp	ebx,[a32 12345678h+edx+eax]
	cmp	ebx,[ebp]
	cmp	ebx,[ebp+42h]

	cmp	[esp],ebx
	cmp	[a32 12345678h],ebx
	cmp	[a32 12345678h+edx+eax],ebx
	cmp	[ebp],ebx
	cmp	[ebp+42h],ebx

	idiv	al
	idiv	ah
	idiv	ax
	idiv	bx
	idiv	eax
	idiv	ebx
	idiv	ecx
	idiv	edx
	idiv	esi
	idiv	edi
	idiv	word [eax]
	idiv	word [ebx]
	idiv	word [ecx]
	idiv	word [edx]
	idiv	word [esi]
	idiv	word [edi]

	imul	al
	imul	ah
	imul	ax
	imul	bx
	imul	eax
	imul	ebx
	imul	ecx
	imul	edx
	imul	esi
	imul	edi
	imul	word [eax]
	imul	word [ebx]
	imul	word [ecx]
	imul	word [edx]
	imul	word [esi]
	imul	word [edi]

	nop
	lea	bx,[si+3]
	lea	cx,[bx+di+33h]
	lea	eax,[ebx+0x1234]
	lea	ebx,[esi*4+ecx+0x22]
	mov	ax,es
	mov	ds,ax
	mov	bx,cs
	mov	cs,bx
	mov	cx,ds
	mov	dx,ss
	mov	[di],es
	mov	[si],ds
	test	ax,bx
	test	si,cx
	test	cl,bh
	test	[di],bx
	test	[si+bx+43],cx
	test	bx,[di]
	test	cx,[si+bx+43]

	xchg	bl,dl
	xchg	[si],cl
	xchg	[di],si
	cmp	bl,44h
	cmp	bx,2446h
	cmp	cx,7Eh
	cmp	cx,0xFF99
	cmp	byte [bx+3],35h
	cmp	word [bp-3],2244h
	cmp	word [di],62h
	mov	ah,dh
	mov	bl,[si+bx+3]
	mov	si,[di+bx+34h]
	inc	ax
	inc	bx
	inc	cx
	inc	dx
	inc	si
	inc	di
	inc	bp
	inc	sp
	dec	sp
	dec	bp
	dec	di
	dec	si
	dec	dx
	dec	cx
	dec	bx
	dec	ax
	push	ax
	push	bx
	push	cx
	push	dx
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	jo	j2
j2:	jno	j2
	ja	j2
	jz	j2
	xchg	ax,bx
	xchg	ax,cx
	xchg	ax,dx
	xchg	ax,si
	mov	al,13h
	mov	bl,44h
	mov	dh,11h
	mov	si,2456h
	mov	bp,2222h
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
	pop	cs
	nop
	nop
	nop
	nop
	pop	ds

	mov	cr0,eax
	mov	cr0,ebx
	mov	eax,cr0
	mov	cr2,eax
	mov	eax,cr2
	mov	dr0,eax
	mov	eax,dr0
	mov	dr3,eax
	mov	eax,dr3
	mov	tr0,eax
	mov	eax,tr0
	mov	tr3,eax
	mov	eax,tr3

	salc
	icebp
; 386/486 only
	umov	eax,ebx
	umov	[edi],ebx
	umov	esi,[edi]
	umov	al,cl
	umov	ah,[esi]

	nop
	nop
	nop
	db	0x0F,0x05	; 286 loadall

	nop
	nop
	nop
	db	0x0F,0x07	; 386 loadall

	nop
	nop
	nop
	movupd	xmm0,xmm3
	movupd	[di],xmm3
	movupd	[edi],xmm3
	movupd	xmm3,[edi]
	movupd	xmm3,[di]
	movups	[edi],xmm6
	movups	xmm6,[edi]
	movups	xmm1,xmm4
	movlpd	[edi],xmm6
	movlpd	xmm6,[edi]
	movlps	[esi],xmm4
	movlps	xmm4,[esi]

