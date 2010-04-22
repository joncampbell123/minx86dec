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
	
