	org	0
	bits	16

_start:

;===================general================
	aaa
	aad
	aam
	aas
	adc	al,al
	adc	al,0x10
	add	al,al
	add	al,0x10
	and	al,al
	and	al,0x10
	bound	si,[si]
	bsf	ax,ax
	bsr	ax,ax
	bswap	eax
	bt	ax,ax
	btc	ax,ax
	btr	ax,ax
	bts	ax,ax
	call	0x1234
	call	0x1234:0x1234
	cbw
	cwde
	cwd
	cdq
	clc
	cld
	clflush	[si]
	cmc
	cmovz	ax,ax
	cmovnz	ax,ax
	cmp	al,al
	cmp	al,0x10
	cmpsb
	cmpsw
	cmpxchg	al,al
	cmpxchg	ax,ax
	cmpxchg8b [si]
	cpuid
	daa
	das
	dec	al
	dec	ax
	div	ax
	enter	1,1
	idiv	ax
	imul	ax
	in	al,dx
	in	al,0xCC
	inc	al
	inc	ax
	insb
	insw
	int	0x21
	into
	jz	short $
	jnz	short $
	jz	near $
	jnz	near $
	jcxz	$
	jecxz	$
	jmp	short $
	jmp	near $
	jmp	[si]
	jmp	0x1234:0x5678
	lahf
	lds	ax,[si]
	les	ax,[si]
	lfs	ax,[si]
	lgs	ax,[si]
	lss	ax,[si]
	lea	ax,[si]
	leave
	lfence
	lodsb
	lodsw
	loop	$
	loopz	$
	loopnz	$
	lzcnt	ax,ax
	mfence
	mov	al,al
	movd	mm0,eax
	movd	xmm0,eax
	movd	eax,mm0
	movd	eax,xmm0
	db	0x66,0x0F,0x50,0xC0		; movmkspd eax,xmm0
	db	0x0F,0x50,0xC0			; movmksps eax,xmm0
	movnti	[esi],eax
	movsb
	movsw
	movsx	eax,ax
	movzx	eax,ax
	mul	ax
	neg	ax
	nop
	not	ax
	or	ax,ax
	out	0xCC,al
	outsb
	outsw
	pause
	pop	ax
	pop	si
	pop	es
	popa
	popcnt	ax,ax
	popf
	prefetch [esi]
	prefetchw [esi]
	prefetchnta [esi]
	prefetcht0 [esi]
	prefetcht1 [esi]
	prefetcht2 [esi]
	push	ax
	push	cs
	pusha
	pushf
	rcl	al,1
	rcl	al,cl
	rcl	al,3
	rcr	al,1
	rcr	al,cl
	rcr	al,3
	ret
	ret	3
	retf
	retf	3
	rol	al,1
	rol	al,cl
	rol	al,3
	ror	al,1
	ror	al,cl
	ror	al,3
	sahf
	sal	al,1
	sal	al,cl
	sal	al,3
	sar	al,1
	sar	al,cl
	sar	al,3
	sbb	al,al
	sbb	al,0x10
	scasb
	scasw
	setz	al
	setnz	al
	sfence
	shl	al,1
	shl	al,cl
	shl	al,3
	shld	ax,bx,1
	shld	ax,bx,cl
	shr	al,1
	shr	al,cl
	shr	al,3
	shrd	ax,bx,1
	shrd	ax,bx,cl
	stc
	std
	stosb
	stosw
	sub	al,al
	sub	al,0x10
	test	al,al
	test	al,0x10
	xadd	al,al
	xadd	ax,ax
	xchg	ax,bx
	xlat
	xor	al,al
	xor	al,0x10
;===================system================
	arpl	ax,ax
	db	0x0F,0x01,0xDD		; clgi
	cli
	clts
	hlt
	int	3
	invd
	invlpg	[si]
	invlpga	eax,ecx
	iret
	iretd
	lar	ax,ax
	lgdt	[si]
	lidt	[si]
	lldt	ax
	lmsw	ax
	lsl	ax,ax
	ltr	ax
	monitor
	mov	cr0,eax
	mov	eax,cr0
	mov	dr0,eax
	mov	eax,dr0
	mwait
	rdmsr
	rdpmc
	rdtsc
	rdtscp
	rsm
	sgdt	[si]
	sidt	[si]
	db	0x0F,0x01,0xDE		; skinit eax
	sldt	ax
	smsw	ax
	sti
	db	0x0F,0x01,0xDC		; stgi
	str	ax
	db	0x0F,0x01,0xF8		; swapgs
	syscall
	sysenter
	sysexit
	sysret
	ud2
	verr	ax
	verw	ax
	db	0x0F,0x01,0xDA		; vmload
	db	0x0F,0x01,0xD9		; vmmcall
	db	0x0F,0x01,0xD8		; vmrun
	db	0x0F,0x01,0xDB		; vmsave
	wbinvd
	wrmsr

