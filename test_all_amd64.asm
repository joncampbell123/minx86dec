; requires NASM 2.09.10

	org	0
	bits	16

_start:

	push	byte 0x12
	push	word 0x1234
	push	dword 0x12345678

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
	insd
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
	lzcnt	ax,[si]
	lzcnt	eax,eax
	lzcnt	eax,[si]
	lzcnt	eax,[esi]
	mfence
	mov	al,al
	movd	mm0,eax
	movd	xmm0,eax
	movd	eax,mm0
	movd	eax,xmm0
	mov	[0x1234],ax
	mov	[a32 0x12345678],eax
	db	0x66,0x0F,0x50,0xC0		; movmkspd eax,xmm0
	db	0x0F,0x50,0xC0			; movmksps eax,xmm0
	movnti	[esi],eax
	movsb
	movsw
	movsx	bx,al
	movsx	ebx,al
	movsx	ebx,ax
	movzx	bx,al
	movzx	ebx,al
	movzx	ebx,ax
	movsx	bx,byte [si]
	movsx	ebx,byte [si]
	movsx	ebx,word [si]
	movzx	bx,byte [si]
	movzx	ebx,byte [si]
	movzx	ebx,word [si]
	mul	ax
	neg	ax
	nop
	not	ax
	or	ax,ax
	out	0xCC,al
	outsb
	outsw
	outsd
	pause
	pop	ax
	pop	eax
	pop	si
	pop	esi
	pop	es
	popa
	popad
	popcnt	ax,ax
	popcnt	ax,[si]
	popcnt	eax,eax
	popcnt	eax,[si]
	popcnt	eax,[esi]
	popf
	popfd
	prefetch [esi]
	prefetchw [esi]
	prefetchnta [esi]
	prefetcht0 [esi]
	prefetcht1 [esi]
	prefetcht2 [esi]
	push	ax
	push	eax
	push	cs
	pusha
	pushad
	pushf
	pushfd
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
	rol	ax,1
	rol	ax,cl
	rol	ax,3
	rol	byte [si],1
	rol	byte [si],cl
	rol	byte [si],3
	ror	al,1
	ror	al,cl
	ror	al,3
	ror	ax,1
	ror	ax,cl
	ror	ax,3
	ror	byte [si],1
	ror	byte [si],cl
	ror	byte [si],3
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
	shld	[si],bx,1
	shld	[si],bx,cl
	shr	al,1
	shr	al,cl
	shr	al,3
	shrd	ax,bx,1
	shrd	ax,bx,cl
	shrd	[si],bx,1
	shrd	[si],bx,cl
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
	xadd	eax,eax
	xadd	[si],al
	xadd	[si],ax
	xadd	[si],eax
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
;===================FPU=================
	f2xm1
	fabs
	fadd	st0,st1
	fadd	st1,st0
	fadd	dword [si]
	fadd	qword [si]
	faddp	st3,st0
	faddp	st1,st0
	fiadd	word [si]
	fiadd	dword [si]
	fbld	[si]
	fbstp	[si]
	fchs
	fclex
	fnclex
	fcmovb	st2
	fcmovbe	st2
	fcmove	st2
	fcmovnb	st2
	fcmovnbe st2
	fcmovne	st2
	fcmovnu	st2
	fcmovu	st2
	fcom	st2
	fcom	dword [si]
	fcom	qword [si]
	fcomp	st2
	fcomp	dword [si]
	fcomp	qword [si]
	fcompp
	fcomi	st2
	fcomip	st2
	fcos
	fdecstp
	fdiv	st0,st2
	fdiv	st2,st0
	fdiv	dword [si]
	fdiv	qword [si]
	fdivp	st2
	fidiv	word [si]
	fidiv	dword [si]
	fdivr	st0,st2
	fdivr	st2,st0
	fdivr	dword [si]
	fdivr	qword [si]
	fdivrp	st2
	fidivr	word [si]
	fidivr	dword [si]
	ffree	st2
	ficom	word [si]
	ficom	dword [si]
	ficomp	word [si]
	ficomp	dword [si]
	fild	word [si]
	fild	dword [si]
	fild	qword [si]
	finit
	fninit
	fist	word [si]
	fist	dword [si]
	fistp	word [si]
	fistp	dword [si]
	fistp	qword [si]
	fld	st2
	fld	dword [si]
	fld	qword [si]
	fld	tword [si]
	fld1
	fldcw	[si]
	fldenv	[si]
	fldl2e
	fldl2t
	fldlg2
	fldln2
	fldpi
	fldz
	fmul	st0,st2
	fmul	st2,st0
	fmul	dword [si]
	fmul	qword [si]
	fmulp	st2
	fimul	word [si]
	fimul	dword [si]
	fnop
	fpatan
	fprem
	fprem1
	fptan
	frndint
	frstor	[si]
	fsave	[si]
	fnsave	[si]
	fscale
	fsin
	fsincos
	fsqrt
	fst	st2
	fst	dword [si]
	fst	qword [si]
	fstp	st2
	fstp	dword [si]
	fstp	qword [si]
	fstp	tword [si]
	fstcw	[si]
	fnstcw	[si]
	fstenv	[si]
	fnstenv	[si]
	fstsw	ax
	fstsw	[si]
	fnstsw	ax
	fnstsw	[si]
	fsub	st0,st2
	fsub	st2,st0
	fsub	dword [si]
	fsub	qword [si]
	fsubp	st2
	fisub	word [si]
	fisub	dword [si]
	fsubr	st0,st2
	fsubr	st2,st0
	fsubr	dword [si]
	fsubr	qword [si]
	fsubrp	st2
	fisubr	word [si]
	fisubr	dword [si]
	ftst
	fucom	st2
	fucomp	st2
	fucompp
	fucomi	st2
	fucomip	st2
	fwait
	fxam
	fxch	st2
	fxrstor	[si]
	fxsave	[si]
	fxtract
	fyl2x
	fyl2xp1
;===================MMX=================
	cvtpd2pi	mm0,xmm1
	cvtpd2pi	mm0,[si]
	cvtpi2pd	xmm0,mm1
	cvtpi2pd	xmm0,[si]
	cvtpi2ps	xmm0,mm1
	cvtpi2ps	xmm0,[si]
	cvtps2pi	mm0,xmm1
	cvtps2pi	mm0,[si]
	cvttpd2pi	mm0,xmm1
	cvttpd2pi	mm0,[si]
	cvttps2pi	mm0,xmm1
	cvttps2pi	mm0,[si]
	emms
	femms
	fnsave		[si]
	fsave		[si]
	frstor		[si]
	fxrstor		[si]
	fxsave		[si]
	maskmovq	mm0,mm1
	maskmovq	mm1,mm0
	movdq2q		mm0,xmm1
	movntq		[si],mm1
	movd		mm0,eax
	movd		mm0,[si]
	movd		eax,mm1
	movd		[si],mm1
	movq		mm0,mm1
	movq		mm1,[si]
	movq		[si],mm0
	movq2dq		xmm1,mm0
	packssdw	mm0,mm1
	packssdw	mm0,[si]
	packsswb	mm0,mm1
	packsswb	mm0,[si]
	packuswb	mm0,mm1
	packuswb	mm0,[si]
	paddb		mm0,mm1
	paddb		mm0,[si]
	paddd		mm0,mm1
	paddd		mm0,[si]
	paddq		mm0,mm1
	paddq		mm0,[si]
	paddsb		mm0,mm1
	paddsb		mm0,[si]
	paddsw		mm0,mm1
	paddsw		mm0,[si]
	paddusb		mm0,mm1
	paddusb		mm0,[si]
	paddusw		mm0,mm1
	paddusw		mm0,[si]
	paddw		mm0,mm1
	paddw		mm0,[si]
	pand		mm0,mm1
	pand		mm0,[si]
	pandn		mm0,mm1
	pandn		mm0,[si]
	pavgb		mm0,mm1
	pavgb		mm0,[si]
	pavgusb		mm0,mm1
	pavgusb		mm0,[si]
	pavgw		mm0,mm1
	pavgw		mm0,[si]
	pcmpeqb		mm0,mm1
	pcmpeqb		mm0,[si]
	pcmpeqd		mm0,mm1
	pcmpeqd		mm0,[si]
	pcmpeqw		mm0,mm1
	pcmpeqw		mm0,[si]
	pcmpgtb		mm0,mm1
	pcmpgtb		mm0,[si]
	pcmpgtd		mm0,mm1
	pcmpgtd		mm0,[si]
	pcmpgtw		mm0,mm1
	pcmpgtw		mm0,[si]
	pextrw		eax,mm0,0
	pextrw		ebx,mm1,1
	pf2id		mm0,mm1
	pf2id		mm0,[si]
	pf2iw		mm0,mm1
	pf2iw		mm0,[si]
	pfacc		mm0,mm1
	pfacc		mm0,[si]
	pfadd		mm0,mm1
	pfadd		mm0,[si]
	pfcmpeq		mm0,mm1
	pfcmpeq		mm0,[si]
	pfcmpge		mm0,mm1
	pfcmpge		mm0,[si]
	pfcmpgt		mm0,mm1
	pfcmpgt		mm0,[si]
	pfmax		mm0,mm1
	pfmax		mm0,[si]
	pfmin		mm0,mm1
	pfmin		mm0,[si]
	pfmul		mm0,mm1
	pfmul		mm0,[si]
	pfnacc		mm0,mm1
	pfnacc		mm0,[si]
	pfpnacc		mm0,mm1
	pfpnacc		mm0,[si]
	pfrcp		mm0,mm1
	pfrcp		mm0,[si]
	pfrcpit1	mm0,mm1
	pfrcpit1	mm0,[si]
	pfrcpit2	mm0,mm1
	pfrcpit2	mm0,[si]
	pfrsqit1	mm0,mm1
	pfrsqit1	mm0,[si]
	pfrsqrt		mm0,mm1
	pfrsqrt		mm0,[si]
	pfsub		mm0,mm1
	pfsub		mm0,[si]
	pfsubr		mm0,mm1
	pfsubr		mm0,[si]
	pi2fd		mm0,mm1
	pi2fd		mm0,[si]
	pi2fw		mm0,mm1
	pi2fw		mm0,[si]
	pinsrw		mm0,eax,0
	pinsrw		mm1,ebx,1
	pmaddwd		mm0,mm1
	pmaddwd		mm0,[si]
	pmaxsw		mm0,mm1
	pmaxsw		mm0,[si]
	pmaxub		mm0,mm1
	pmaxub		mm0,[si]
	pminsw		mm0,mm1
	pminsw		mm0,[si]
	pminub		mm0,mm1
	pminub		mm0,[si]
	pmovmskb	eax,mm0
	pmulhrwa	mm0,mm1
	pmulhrwa	mm0,[si]
	pmulhuw		mm0,mm1
	pmulhuw		mm0,[si]
	pmulhw		mm0,mm1
	pmulhw		mm0,[si]
	pmullw		mm0,mm1
	pmullw		mm0,[si]
	pmuludq		mm0,mm1
	pmuludq		mm0,[si]
	por		mm0,mm1
	por		mm0,[si]
	psadbw		mm0,mm1
	psadbw		mm0,[si]
	pshufw		mm0,mm1,0
	pshufw		mm0,[si],0
	pshufw		mm0,mm1,2
	pshufw		mm0,[si],2
	pslld		mm0,mm1
	pslld		mm0,[si]
	pslld		mm0,1
	psllq		mm0,mm1
	psllq		mm0,[si]
	psllq		mm0,1
	psllw		mm0,mm1
	psllw		mm0,[si]
	psllw		mm0,1
	psrad		mm0,mm1
	psrad		mm0,1
	psraw		mm0,mm1
	psraw		mm0,1
	psrld		mm0,mm1
	psrld		mm0,1
	psrlq		mm0,mm1
	psrlq		mm0,1
	psrlw		mm0,mm1
	psrlw		mm0,1
	psubb		mm0,mm1
	psubb		mm0,[si]
	psubw		mm0,mm1
	psubw		mm0,[si]
	psubd		mm0,mm1
	psubd		mm0,[si]
	psubq		mm0,mm1
	psubq		mm0,[si]
	psubsb		mm0,mm1
	psubsb		mm0,[si]
	psubsw		mm0,mm1
	psubsw		mm0,[si]
	psubusb		mm0,mm1
	psubusb		mm0,[si]
	psubusw		mm0,mm1
	psubusw		mm0,[si]
	pswapd		mm0,mm1
	pswapd		mm0,[si]
	punpckhbw	mm0,mm1
	punpckhbw	mm0,[si]
	punpckhdq	mm0,mm1
	punpckhdq	mm0,[si]
	punpckhwd	mm0,mm1
	punpckhwd	mm0,[si]
	punpcklbw	mm0,mm1
	punpcklbw	mm0,[si]
	punpckldq	mm0,mm1
	punpckldq	mm0,[si]
	punpcklwd	mm0,mm1
	punpcklwd	mm0,[si]
	pxor		mm0,mm1
	pxor		mm0,[si]
;===================SSE=================
	addpd		xmm1,xmm2
	addpd		xmm1,[si]
	addps		xmm1,xmm2
	addps		xmm1,[si]
	addsd		xmm1,xmm2
	addsd		xmm1,[si]
	addss		xmm1,xmm2
	addss		xmm1,[si]
	addsubpd	xmm1,xmm2
	addsubpd	xmm1,[si]
	addsubps	xmm1,xmm2
	addsubps	xmm1,[si]
	andnpd		xmm1,xmm2
	andnpd		xmm1,[si]
	andnps		xmm1,xmm2
	andnps		xmm1,[si]
	andpd		xmm1,xmm2
	andpd		xmm1,[si]
	andps		xmm1,xmm2
	andps		xmm1,[si]
	cmppd		xmm1,xmm2,0
	cmppd		xmm1,[si],0
	cmpps		xmm1,xmm2,0
	cmpps		xmm1,[si],0
	cmpsd		xmm1,xmm2,0
	cmpsd		xmm1,[si],0
	cmpss		xmm1,xmm2,0
	cmpss		xmm1,[si],0
	comisd		xmm1,xmm2
	comisd		xmm1,[si]
	comiss		xmm1,xmm2
	comiss		xmm1,[si]
	cvtdq2pd	xmm1,xmm2
	cvtdq2pd	xmm1,[si]
	cvtdq2ps	xmm1,xmm2
	cvtdq2ps	xmm1,[si]
	cvtpd2dq	xmm1,xmm2
	cvtpd2dq	xmm1,[si]
	cvtpd2pi	mm1,xmm2
	cvtpd2pi	mm1,[si]
	cvtpd2ps	xmm1,xmm2
	cvtpd2ps	xmm1,[si]
	cvtpi2pd	xmm1,mm2
	cvtpi2pd	xmm1,[si]
	cvtpi2ps	xmm1,mm2
	cvtpi2ps	xmm1,[si]
	cvtps2dq	xmm1,xmm2
	cvtps2dq	xmm1,[si]
	cvtps2pd	xmm1,xmm2
	cvtps2pd	xmm1,[si]
	cvtps2pi	mm1,xmm2
	cvtps2pi	mm1,[si]
	cvtsd2si	eax,xmm1
	cvtsd2si	eax,[si]
	cvtsd2ss	xmm1,xmm2
	cvtsd2ss	xmm1,[si]
	cvtsi2sd	xmm1,eax
	cvtsi2sd	xmm1,[si]
	cvtsi2ss	xmm1,eax
	cvtsi2ss	xmm1,[si]
	cvtss2sd	xmm1,xmm2
	cvtss2sd	xmm1,[si]
	cvtss2si	eax,xmm2
	cvtss2si	eax,[si]
	cvttpd2dq	xmm1,xmm2
	cvttpd2dq	xmm1,[si]
	cvttpd2pi	mm1,xmm2
	cvttpd2pi	mm1,[si]
	cvttps2dq	xmm1,xmm2
	cvttps2dq	xmm1,[si]
	cvttps2pi	mm1,xmm2
	cvttps2pi	mm1,[si]
	cvttsd2si	eax,xmm1
	cvttsd2si	eax,[si]
	cvttss2si	eax,xmm1
	cvttss2si	eax,[si]
	divpd		xmm1,xmm2
	divpd		xmm1,[si]
	divps		xmm1,xmm2
	divps		xmm1,[si]
	divsd		xmm1,xmm2
	divsd		xmm1,[si]
	divss		xmm1,xmm2
	divss		xmm1,[si]
	extrq		xmm1,2,3
	extrq		xmm1,xmm2
	fxrstor		[si]
	fxsave		[si]
	haddpd		xmm1,xmm2
	haddpd		xmm1,[si]
	haddps		xmm1,xmm2
	haddps		xmm1,[si]
	hsubpd		xmm1,xmm2
	hsubpd		xmm1,[si]
	hsubps		xmm1,xmm2
	hsubps		xmm1,[si]
	insertq		xmm1,xmm2,3,4
	insertq		xmm1,xmm2
	lddqu		xmm1,[si]
	ldmxcsr		[si]
	maskmovdqu	xmm1,xmm2
	maxpd		xmm1,xmm2
	maxpd		xmm1,[si]
	maxps		xmm1,xmm2
	maxps		xmm1,[si]
	maxsd		xmm1,xmm2
	maxsd		xmm1,[si]
	maxss		xmm1,xmm2
	maxss		xmm1,[si]
	minpd		xmm1,xmm2
	minpd		xmm1,[si]
	minps		xmm1,xmm2
	minps		xmm1,[si]
	minsd		xmm1,xmm2
	minsd		xmm1,[si]
	minss		xmm1,xmm2
	minss		xmm1,[si]
	movapd		xmm1,xmm2
	movapd		xmm1,[si]
	movapd		[si],xmm1
	movaps		xmm1,xmm2
	movaps		xmm1,[si]
	movaps		[si],xmm1
	movd		xmm1,eax
	movd		xmm1,dword [si]
	movd		eax,xmm1
	movd		dword [si],xmm1
	movddup		xmm1,xmm2
	movddup		xmm1,[si]
	movdq2q		mm0,xmm1
	movdqa		xmm1,xmm2
	movdqa		xmm1,[si]
	movdqa		[si],xmm1
	movdqu		xmm1,xmm2
	movdqu		xmm1,[si]
	movdqu		[si],xmm1
	movhlps		xmm1,xmm2
	movhpd		xmm1,[si]
	movhpd		[si],xmm1
	movhps		xmm1,[si]
	movhps		[si],xmm1
	movlhps		xmm1,xmm2
	movlpd		xmm1,[si]
	movlpd		[si],xmm1
	movlps		xmm1,[si]
	movlps		[si],xmm1
	movmskpd	eax,xmm1
	movmskps	eax,xmm1
	movntdq		[si],xmm1
	movntpd		[si],xmm1
	movntps		[si],xmm1
	movntsd		[si],xmm1
	movntss		[si],xmm1
	movq		xmm1,xmm2
	movq		xmm1,[si]
	movq		[si],xmm1
	movq2dq		xmm1,mm2
	movsd		xmm1,xmm2
	movsd		xmm1,[si]
	movshdup	xmm1,xmm2
	movshdup	xmm1,[si]
	movsldup	xmm1,xmm2
	movsldup	xmm1,[si]
	movss		xmm1,xmm2
	movss		xmm1,[si]
	movss		[si],xmm1
	movupd		xmm1,xmm2
	movupd		xmm1,[si]
	movupd		[si],xmm1
	movups		xmm1,xmm2
	movups		xmm1,[si]
	movups		[si],xmm1
	mulpd		xmm1,xmm2
	mulpd		xmm1,[si]
	mulps		xmm1,xmm2
	mulps		xmm1,[si]
	mulsd		xmm1,xmm2
	mulsd		xmm1,[si]
	mulss		xmm1,xmm2
	mulss		xmm1,[si]
	orpd		xmm1,xmm2
	orpd		xmm1,[si]
	orps		xmm1,xmm2
	orps		xmm1,[si]
	packssdw	xmm1,xmm2
	packssdw	xmm1,[si]
	packsswb	xmm1,xmm2
	packsswb	xmm1,[si]
	packuswb	xmm1,xmm2
	packuswb	xmm1,[si]
	paddb		xmm1,xmm2
	paddb		xmm1,[si]
	paddd		xmm1,xmm2
	paddd		xmm1,[si]
	paddq		xmm1,xmm2
	paddq		xmm1,[si]
	paddsb		xmm1,xmm2
	paddsb		xmm1,[si]
	paddsw		xmm1,xmm2
	paddsw		xmm1,[si]
	paddusb		xmm1,xmm2
	paddusb		xmm1,[si]
	paddusw		xmm1,xmm2
	paddusw		xmm1,[si]
	paddw		xmm1,xmm2
	paddw		xmm1,[si]
	pand		xmm1,xmm2
	pand		xmm1,[si]
	pandn		xmm1,xmm2
	pandn		xmm1,[si]
	pavgb		xmm1,xmm2
	pavgb		xmm1,[si]
	pavgw		xmm1,xmm2
	pavgw		xmm1,[si]
	pcmpeqb		xmm1,xmm2
	pcmpeqb		xmm1,[si]
	pcmpeqd		xmm1,xmm2
	pcmpeqd		xmm1,[si]
	pcmpeqw		xmm1,xmm2
	pcmpeqw		xmm1,[si]
	pcmpgtb		xmm1,xmm2
	pcmpgtb		xmm1,[si]
	pcmpgtd		xmm1,xmm2
	pcmpgtd		xmm1,[si]
	pcmpgtw		xmm1,xmm2
	pcmpgtw		xmm1,[si]
	pextrw		eax,xmm1,2
	pinsrw		xmm1,eax,3
	pmaddwd		xmm1,xmm2
	pmaddwd		xmm1,[si]
	pmaxsw		xmm1,xmm2
	pmaxsw		xmm1,[si]
	pmaxub		xmm1,xmm2
	pmaxub		xmm1,[si]
	pminsw		xmm1,xmm2
	pminsw		xmm1,[si]
	pminub		xmm1,xmm2
	pminub		xmm1,[si]
	pmovmskb	eax,xmm1
	pmulhuw		xmm1,xmm2
	pmulhuw		xmm1,[si]
	pmulhw		xmm1,xmm2
	pmulhw		xmm1,[si]
	pmullw		xmm1,xmm2
	pmullw		xmm1,[si]
	pmuludq		xmm1,xmm2
	pmuludq		xmm1,[si]
	por		xmm1,xmm2
	por		xmm1,[si]
	psadbw		xmm1,xmm2
	psadbw		xmm1,[si]
	pshufd		xmm1,xmm2,3
	pshufd		xmm1,[si],3
	pshufhw		xmm1,xmm2,3
	pshufhw		xmm1,[si],3
	pshuflw		xmm1,xmm2,3
	pshuflw		xmm1,[si],3
	pslld		xmm1,xmm2
	pslld		xmm1,[si]
	pslld		xmm1,2
	pslldq		xmm1,2
	psllq		xmm1,xmm2
	psllq		xmm1,[si]
	psllq		xmm1,2
	psllw		xmm1,xmm2
	psllw		xmm1,[si]
	psllw		xmm1,2
	psrad		xmm1,xmm2
	psrad		xmm1,[si]
	psrad		xmm1,2
	psraw		xmm1,xmm2
	psraw		xmm1,[si]
	psraw		xmm1,2
	psrld		xmm1,xmm2
	psrld		xmm1,[si]
	psrld		xmm1,2
	psrldq		xmm1,2
	psrlq		xmm1,xmm2
	psrlq		xmm1,[si]
	psrlq		xmm1,2
	psrlw		xmm1,xmm2
	psrlw		xmm1,[si]
	psrlw		xmm1,2
	psubb		xmm1,xmm2
	psubb		xmm1,[si]
	psubd		xmm1,xmm2
	psubd		xmm1,[si]
	psubq		xmm1,xmm2
	psubq		xmm1,[si]
	psubsb		xmm1,xmm2
	psubsb		xmm1,[si]
	psubsw		xmm1,xmm2
	psubsw		xmm1,[si]
	psubusb		xmm1,xmm2
	psubusb		xmm1,[si]
	psubusw		xmm1,xmm2
	psubusw		xmm1,[si]
	psubw		xmm1,xmm2
	psubw		xmm1,[si]
	punpckhbw	xmm1,xmm2
	punpckhbw	xmm1,[si]
	punpckhdq	xmm1,xmm2
	punpckhdq	xmm1,[si]
	punpckhqdq	xmm1,xmm2
	punpckhqdq	xmm1,[si]
	punpckhwd	xmm1,xmm2
	punpckhwd	xmm1,[si]
	punpcklbw	xmm1,xmm2
	punpcklbw	xmm1,[si]
	punpckldq	xmm1,xmm2
	punpckldq	xmm1,[si]
	punpcklqdq	xmm1,xmm2
	punpcklqdq	xmm1,[si]
	punpcklwd	xmm1,xmm2
	punpcklwd	xmm1,[si]
	pxor		xmm1,xmm2
	pxor		xmm1,[si]
	rcpps		xmm1,xmm2
	rcpps		xmm1,[si]
	rcpss		xmm1,xmm2
	rcpss		xmm1,[si]
	rsqrtps		xmm1,xmm2
	rsqrtps		xmm1,[si]
	rsqrtss		xmm1,xmm2
	rsqrtss		xmm1,[si]
	shufpd		xmm1,xmm2,3
	shufpd		xmm1,[si],3
	shufps		xmm1,xmm2,3
	shufps		xmm1,[si],3
	sqrtpd		xmm1,xmm2
	sqrtpd		xmm1,[si]
	sqrtps		xmm1,xmm2
	sqrtps		xmm1,[si]
	sqrtsd		xmm1,xmm2
	sqrtsd		xmm1,[si]
	sqrtss		xmm1,xmm2
	sqrtss		xmm1,[si]
	stmxcsr		[si]
	subpd		xmm1,xmm2
	subpd		xmm1,[si]
	subps		xmm1,xmm2
	subps		xmm1,[si]
	subsd		xmm1,xmm2
	subsd		xmm1,[si]
	subss		xmm1,xmm2
	subss		xmm1,[si]
	ucomisd		xmm1,xmm2
	ucomisd		xmm1,[si]
	ucomiss		xmm1,xmm2
	ucomiss		xmm1,[si]
	unpckhpd	xmm1,xmm2
	unpckhpd	xmm1,[si]
	unpckhps	xmm1,xmm2
	unpckhps	xmm1,[si]
	unpcklpd	xmm1,xmm2
	unpcklpd	xmm1,[si]
	unpcklps	xmm1,xmm2
	unpcklps	xmm1,[si]
	xorpd		xmm1,xmm2
	xorpd		xmm1,[si]
	xorps		xmm1,xmm2
	xorps		xmm1,[si]
;===================VEX=================
	vfmaddpd	xmm1,xmm2,xmm3,xmm4
	vfmaddpd	xmm1,xmm2,[si],xmm4
	vfmaddpd	ymm1,ymm2,ymm3,ymm4
	vfmaddpd	ymm1,ymm2,[si],ymm4
	vfmaddps	xmm1,xmm2,xmm3,xmm4
	vfmaddps	xmm1,xmm2,[si],xmm4
	vfmaddps	ymm1,ymm2,ymm3,ymm4
	vfmaddps	ymm1,ymm2,[si],ymm4
	vfmaddsd	xmm1,xmm2,xmm3,xmm4
	vfmaddsd	xmm1,xmm2,[si],xmm4
	vfmaddss	xmm1,xmm2,xmm3,xmm4
	vfmaddss	xmm1,xmm2,[si],xmm4
	vfmaddsubpd	xmm1,xmm2,xmm3,xmm4
	vfmaddsubpd	xmm1,xmm2,[si],xmm4
	vfmaddsubpd	ymm1,ymm2,ymm3,ymm4
	vfmaddsubpd	ymm1,ymm2,[si],ymm4
	vfmaddsubps	xmm1,xmm2,xmm3,xmm4
	vfmaddsubps	xmm1,xmm2,[si],xmm4
	vfmaddsubps	ymm1,ymm2,ymm3,ymm4
	vfmaddsubps	ymm1,ymm2,[si],ymm4
	vfmsubaddpd	xmm1,xmm2,xmm3,xmm4
	vfmsubaddpd	xmm1,xmm2,[si],xmm4
	vfmsubaddpd	ymm1,ymm2,ymm3,ymm4
	vfmsubaddpd	ymm1,ymm2,[si],ymm4
	vfmsubaddps	xmm1,xmm2,xmm3,xmm4
	vfmsubaddps	xmm1,xmm2,[si],xmm4
	vfmsubaddps	ymm1,ymm2,ymm3,ymm4
	vfmsubaddps	ymm1,ymm2,[si],ymm4
	vfmsubpd	xmm1,xmm2,xmm3,xmm4
	vfmsubpd	xmm1,xmm2,[si],xmm4
	vfmsubpd	ymm1,ymm2,ymm3,ymm4
	vfmsubpd	ymm1,ymm2,[si],ymm4
	vfmsubps	xmm1,xmm2,xmm3,xmm4
	vfmsubps	xmm1,xmm2,[si],xmm4
	vfmsubps	ymm1,ymm2,ymm3,ymm4
	vfmsubps	ymm1,ymm2,[si],ymm4
	vfnmaddpd	xmm1,xmm2,xmm3,xmm4
	vfnmaddpd	xmm1,xmm2,[si],xmm4
	vfnmaddpd	ymm1,ymm2,ymm3,ymm4
	vfnmaddpd	ymm1,ymm2,[si],ymm4
	vfnmsubpd	xmm1,xmm2,xmm3,xmm4
	vfnmsubpd	xmm1,xmm2,[si],xmm4
	vfnmsubpd	ymm1,ymm2,ymm3,ymm4
	vfnmsubpd	ymm1,ymm2,[si],ymm4
	vfrczpd		xmm1,xmm2
	vfrczpd		xmm1,[si]
	vfrczpd		ymm1,ymm2
	vfrczpd		ymm1,[si]
	vfrczps		xmm1,xmm2
	vfrczps		xmm1,[si]
	vfrczps		ymm1,ymm2
	vfrczps		ymm1,[si]
	vfrczsd		xmm1,xmm2
	vfrczsd		xmm1,[si]
	vfrczss		xmm1,xmm2
	vfrczss		xmm1,[si]
	vpcmov		xmm1,xmm2,xmm3,xmm4
	vpcmov		xmm1,xmm2,[si],xmm4
	vpcmov		ymm1,ymm2,ymm3,ymm4
	vpcmov		ymm1,ymm2,[si],ymm4
	vpcomb		xmm1,xmm2,xmm3,4
	vpcomb		xmm1,xmm2,[si],4
	vpcomd		xmm1,xmm2,xmm3,4
	vpcomd		xmm1,xmm2,[si],4
	vpcomw		xmm1,xmm2,xmm3,4
	vpcomw		xmm1,xmm2,[si],4
	vpcomq		xmm1,xmm2,xmm3,4
	vpcomq		xmm1,xmm2,[si],4
	vpcomub		xmm1,xmm2,xmm3,4
	vpcomub		xmm1,xmm2,[si],4
	vpcomuw		xmm1,xmm2,xmm3,4
	vpcomuw		xmm1,xmm2,[si],4
	vpcomud		xmm1,xmm2,xmm3,4
	vpcomud		xmm1,xmm2,[si],4
	vpcomuq		xmm1,xmm2,xmm3,4
	vpcomuq		xmm1,xmm2,[si],4
%if 0 ; What the hell NASM you used to support encoding this instruction
	vpermil2pd	xmm1,xmm2,xmm3,xmm4,5
	vpermil2pd	xmm1,xmm2,[si],xmm4,5
	vpermil2pd	ymm1,ymm2,ymm3,ymm4,5
	vpermil2pd	ymm1,ymm2,[si],ymm4,5
%endif
	vphaddbw	xmm1,xmm2
	vphaddbw	xmm1,[si]
	vphaddbd	xmm1,xmm2
	vphaddbd	xmm1,[si]
	vphaddbq	xmm1,xmm2
	vphaddbq	xmm1,[si]
	vphadddq	xmm1,xmm2
	vphadddq	xmm1,[si]
	db	0x8F,0xE9,0x78,0xD1,0xCA	; vphaddubw	xmm1,xmm2
	db	0x8F,0xE9,0x78,0xD1,0x0C	; vphaddubw	xmm1,[si]
	db	0x8F,0xE9,0x78,0xD2,0xCA	; vphaddubd	xmm1,xmm2
	db	0x8F,0xE9,0x78,0xD2,0x0C	; vphaddubd	xmm1,[si]
	db	0x8F,0xE9,0x78,0xD3,0xCA	; vphaddubq	xmm1,xmm2
	db	0x8F,0xE9,0x78,0xD3,0x0C	; vphaddubq	xmm1,[si]
	vphaddudq	xmm1,xmm2
	vphaddudq	xmm1,[si]
	vphadduwd	xmm1,xmm2
	vphadduwd	xmm1,[si]
	vphadduwq	xmm1,xmm2
	vphadduwq	xmm1,[si]
	vphaddwd	xmm1,xmm2
	vphaddwd	xmm1,[si]
	vphaddwq	xmm1,xmm2
	vphaddwq	xmm1,[si]
	vphsubbw	xmm1,xmm2
	vphsubbw	xmm1,[si]
	vphsubwd	xmm1,xmm2
	vphsubwd	xmm1,[si]
	vphsubdq	xmm1,xmm2
	vphsubdq	xmm1,[si]
	vpmacsdd	xmm1,xmm2,xmm3,xmm4
	vpmacsdd	xmm1,xmm2,[si],xmm4
;================================================================================
; TODO: Either AMD mis-documented these, or NASM has the opcodes backwards, argh!
;	vpmacsdql	xmm1,xmm2,xmm3,xmm4
;	vpmacsdql	xmm1,xmm2,[si],xmm4
;	vpmacsdqh	xmm1,xmm2,xmm3,xmm4
;	vpmacsdqh	xmm1,xmm2,[si],xmm4
;================================================================================
	vpmacssdd	xmm1,xmm2,xmm3,xmm4
	vpmacssdd	xmm1,xmm2,[si],xmm4
;================================================================================
; TODO: Either AMD mis-documented these, or NASM has the opcodes backwards, argh!
;	vpmacssdqh	xmm1,xmm2,xmm3,xmm4
;	vpmacssdqh	xmm1,xmm2,[si],xmm4
;	vpmacssdql	xmm1,xmm2,xmm3,xmm4
;	vpmacssdql	xmm1,xmm2,[si],xmm4
;================================================================================
	vpmacsswd	xmm1,xmm2,xmm3,xmm4
	vpmacsswd	xmm1,xmm2,[si],xmm4
	vpmacssww	xmm1,xmm2,xmm3,xmm4
	vpmacssww	xmm1,xmm2,[si],xmm4
	vpmacswd	xmm1,xmm2,xmm3,xmm4
	vpmacswd	xmm1,xmm2,[si],xmm4
	vpmacsww	xmm1,xmm2,xmm3,xmm4
	vpmacsww	xmm1,xmm2,[si],xmm4
	vpmadcsswd	xmm1,xmm2,xmm3,xmm4
	vpmadcsswd	xmm1,xmm2,[si],xmm4
	vpmadcswd	xmm1,xmm2,xmm3,xmm4
	vpmadcswd	xmm1,xmm2,[si],xmm4
	vpperm		xmm1,xmm2,xmm3,xmm4
	vpperm		xmm1,xmm2,[si],xmm4
	vpperm		xmm1,xmm2,xmm3,[si]
	vprotb		xmm1,xmm2,xmm3
	vprotb		xmm1,[si],xmm3
	vprotb		xmm1,xmm2,[si]
	vprotb		xmm1,xmm2,3
	vprotw		xmm1,xmm2,xmm3
	vprotw		xmm1,[si],xmm3
	vprotw		xmm1,xmm2,[si]
	vprotw		xmm1,xmm2,3
	vprotd		xmm1,xmm2,xmm3
	vprotd		xmm1,[si],xmm3
	vprotd		xmm1,xmm2,[si]
	vprotd		xmm1,xmm2,3
	vprotq		xmm1,xmm2,xmm3
	vprotq		xmm1,[si],xmm3
	vprotq		xmm1,xmm2,[si]
	vprotq		xmm1,xmm2,3
	vpshab		xmm1,xmm2,xmm3
	vpshab		xmm1,[si],xmm3
	vpshab		xmm1,xmm2,[si]
	vpshaw		xmm1,xmm2,xmm3
	vpshaw		xmm1,[si],xmm3
	vpshaw		xmm1,xmm2,[si]
	vpshad		xmm1,xmm2,xmm3
	vpshad		xmm1,[si],xmm3
	vpshad		xmm1,xmm2,[si]
	vpshaq		xmm1,xmm2,xmm3
	vpshaq		xmm1,[si],xmm3
	vpshaq		xmm1,xmm2,[si]
	vpshlb		xmm1,xmm2,xmm3
	vpshlb		xmm1,[si],xmm3
	vpshlb		xmm1,xmm2,[si]
	vpshlw		xmm1,xmm2,xmm3
	vpshlw		xmm1,[si],xmm3
	vpshlw		xmm1,xmm2,[si]
	vpshld		xmm1,xmm2,xmm3
	vpshld		xmm1,[si],xmm3
	vpshld		xmm1,xmm2,[si]
	vpshlq		xmm1,xmm2,xmm3
	vpshlq		xmm1,[si],xmm3
	vpshlq		xmm1,xmm2,[si]
;===================MORE=================
	blendpd		xmm1,xmm2,1
	blendps		xmm1,xmm2,1
	blendvpd	xmm1,xmm2,xmm0
	blendvps	xmm1,xmm2,xmm0
	crc32		eax,al
	crc32		eax,ax
	crc32		eax,eax
	crc32		eax,byte [si]
	crc32		eax,word [si]
	crc32		eax,dword [si]
	dppd		xmm1,xmm2,3
	dpps		xmm1,xmm2,3
	extractps	eax,xmm2,3
	insertps	xmm1,xmm2,3
	movntdqa	xmm1,[si]
	mpsadbw		xmm1,xmm2,3
	mpsadbw		xmm1,[si],3
	pabsb		mm1,mm2
	pabsb		xmm1,xmm2
	pabsw		mm1,mm2
	pabsw		xmm1,xmm2
	pabsd		mm1,mm2
	pabsd		xmm1,xmm2
	packusdw	xmm1,xmm2
	palignr		mm1,mm2,3
	palignr		mm1,[si],3
	palignr		xmm1,xmm2,3
	palignr		xmm1,[si],3
	pblendvb	xmm1,xmm2,xmm0
	pblendvb	xmm1,[si],xmm0
	pblendw		xmm1,xmm2,3
	pblendw		xmm1,[si],3
	pcmpestri	xmm1,xmm2,3
	pcmpestri	xmm1,[si],3
	pcmpestrm	xmm1,xmm2,3
	pcmpestrm	xmm1,[si],3
	pcmpistrm	xmm1,xmm2,3
	pcmpistrm	xmm1,[si],3
	pcmpgtq		xmm1,xmm2
	pextrb		eax,xmm2,3
	phaddw		mm1,mm2
	phaddw		xmm1,xmm2
	phaddd		mm1,mm2
	phaddd		xmm1,xmm2
	phaddsw		mm1,mm2
	phaddsw		xmm1,xmm2
	phminposuw	xmm1,xmm2
	pinsrb		xmm1,eax,3
	pinsrd		xmm1,eax,3
	pmaddubsw	mm1,mm2
	pmaddubsw	xmm1,xmm2
	pmaddwd		mm1,mm2
	pmaddwd		xmm1,xmm2
	pmaxsb		xmm1,xmm2
	pmaxsw		xmm1,xmm2
	pmaxub		xmm1,xmm2
	pmaxud		xmm1,xmm2
	pmaxuw		xmm1,xmm2
	pminsb		xmm1,xmm2
	pminsd		xmm1,xmm2
	pminsw		xmm1,xmm2
	pmovsxbw	xmm1,xmm2
	pmovsxbd	xmm1,xmm2
	pmovsxbq	xmm1,xmm2
	pmovsxwd	xmm1,xmm2
	pmovsxwq	xmm1,xmm2
	pmovsxdq	xmm1,xmm2
	pmovzxbw	xmm1,xmm2
	pmovzxbd	xmm1,xmm2
	pmovzxbq	xmm1,xmm2
	pmovzxwd	xmm1,xmm2
	pmovzxwq	xmm1,xmm2
	pmovzxdq	xmm1,xmm2
	pmuldq		xmm1,xmm2
	pmulhrsw	mm1,mm2
	pmulhrsw	xmm1,xmm2
	pmulld		xmm1,xmm2
	pshufb		mm1,mm2
	pshufb		xmm1,xmm2
	psignb		mm1,mm2
	psignb		xmm1,xmm2
	psignw		mm1,mm2
	psignw		xmm1,xmm2
	psignd		mm1,mm2
	psignd		xmm1,xmm2
	ptest		xmm1,xmm2
	roundpd		xmm1,xmm2,3
	roundps		xmm1,xmm2,3
	roundsd		xmm1,xmm2,3
	roundss		xmm1,xmm2,3
; force NASM to emit this
bits 64
	swapgs
bits 16
	xgetbv
	xlat
	xorpd		xmm1,xmm2
	xorps		xmm1,xmm2
	xrstor		[si]
	lfence
	xsave		[si]
	xsetbv
	invept		eax,[si]
	invvpid		eax,[si]
	vmcall
	vmclear		[si]
	vmlaunch
	vmresume
	vmptrld		[si]
	vmptrst		[si]
	vmread		eax,eax
	vmread		[si],eax
	vmwrite		eax,eax
	vmwrite		eax,[si]
	vmxoff
	vmxon		[si]
	getsec

