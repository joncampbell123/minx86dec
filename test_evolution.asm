; "evolutionary" decode testing.
; each section targets the 8086, then 286, etc.
; this is used to make sure that CPU/FPU decoding is properly segregated
; to the processor the core is emulating i.e. a 386 cannot decode instructions
; that first appeared on the 486.
org 0
bits 16

; HERE WE GO
_start:

;=================================== 8086 ====================================
	aaa
	aad
	aad	3
	aam
	aam	4
	aas
	adc	al,ah
	adc	ax,bx
	add	al,ah
	add	ax,bx
	and	al,ah
	and	ax,bx
	call	ax
	call	[si]
	call	[0x1234]
	call	near $
	call far [di]
	call far [0x1234]
	call	0x1234:0x5678
	cbw
	clc
	cld
	cli
	cmc
	cmp	al,ah
	cmp	ax,bx
	cmpsb
	cmpsw
	cwd
	daa
	das
	dec	al
	dec	ax
	div	al
	div	ax
	hlt
	idiv	al
	idiv	ax
	in	al,0x12
	in	ax,0x12
	inc	al
	inc	ax
	int	0x21
	int	0x34
	into
	iret
	jc	short $
	jnc	short $
	jz	short $
	jmp	short $
	jmp	near $
	lahf
	lds	ax,[si]
	les	ax,[si]
	lea	ax,[si]
	lock	xchg al,ah
	lodsb
	lodsw
	loopnz	$
	loopz	$
	mov	al,ah
	mov	ax,bx
	movsb
	movsw
	mul	al
	mul	ax
	neg	al
	neg	ax
	nop
	not	al
	not	ax
	or	al,al
	or	ax,ax
	out	0x12,al
	out	0x12,ax
	pop	ax
	popf
	push	ax
	pushf
	rcl	al,1
	rcl	al,cl
	rcr	al,1
	rcr	al,cl
	rep	lodsb
	repe	lodsb
	repne	lodsb
	ret
	ret	3
	retf
	retf	3
	rol	al,1
	rol	al,cl
	ror	al,1
	ror	al,cl
	sahf
	sal	al,1
	sal	al,cl
	sar	al,1
	sar	al,cl
	sbb	al,al
	sbb	ax,ax
	scasb
	scasw
	shl	al,1
	shl	al,cl
	shr	al,1
	shr	al,cl
	stc
	std
	sti
	stosb
	stosw
	sub	al,al
	sub	ax,ax
	test	al,al
	test	ax,ax
	wait
	xchg	al,ah
	xlat
	xor	al,al
	xor	ax,ax
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
;=================================== 8087 ====================================
	f2xm1
	fabs
	fadd
	faddp
	fbld	[si]
	fbstp	[si]
	fchs
	fclex
	fcom
	fcomp
	fcompp
	fdecstp
	fdisi
	fdiv	st0
	fdivp	st0
	fdivr	st0
	fdivrp	st0
	feni
	ffree
	fiadd	word [si]
	ficom	word [si]
	ficomp	word [si]
	fidiv	word [si]
	fidivr	word [si]
	fild	word [si]
	fimul	word [si]
	fincstp
	finit
	fist	word [si]
	fistp	word [si]
	fisub	word [si]
	fisubr	word [si]
	fld	dword [si]
	fld1
	fldcw	word [si]
	fldenv	[si]
	fldl2e
	fldl2t
	fldlg2
	fldln2
	fldpi
	fldz
	fmul
	fmulp
	fnclex
	fndisi
	fneni
	fninit
	fnop
	fdisi
	feni
	fnsave	[si]
	fnstcw	word [si]
	fnstenv	[si]
	fnstsw	word [si]
	fpatan
	fprem
	fptan
	frndint
	frstor	[si]
	fsave	[si]
	fscale
	fsqrt
	fst	dword [si]
	fstcw	word [si]
	fstenv	[si]
	fstp	dword [si]
	fstsw	word [si]
	fsub
	fsubp
	fsubr
	fsubrp
	ftst
	fwait
	fxam
	fxch
	fxtract
	fyl2x
	fyl2xp1
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
;=================================== 80186 ====================================
	bound	ax,[si]
	enter	1,1
	insb
	insw
	leave
	outsb
	outsw
	popa
	pusha
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
;=================================== 80286 ====================================
	arpl	ax,ax
	clts
	lar	ax,ax
	lgdt	[si]
	lidt	[si]
	lldt	ax
	lmsw	ax
	loadall286
	lsl	ax,ax
	ltr	ax
	sgdt	[si]
	sidt	[si]
	sldt	ax
	smsw	ax
	str	ax
	verr	ax
	verw	ax
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
;=================================== 80287 ====================================
	fsetpm
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
;=================================== 80386 ====================================
	bsf	ax,ax
	bsr	ax,ax
	bt	ax,ax
	btc	ax,ax
	btr	ax,ax
	bts	ax,ax
	cdq
	cmpsd
	cwde
	insb
	insw
	insb
	iretd
	jcxz	$
	jecxz	$
	lfs	ax,[si]
	lgs	ax,[si]
	lss	ax,[si]
	lodsd
	o32 loop $
	o32 loopz $
	o32 loopnz $
	movsd
	movsx	ax,al
	movzx	ax,al
	movsx	eax,ax
	movzx	eax,ax
	popad
	popfd
	pushad
	pushfd
	scasd
	setz	al
	setnz	al
	shld	cx,dx,1
	shld	eax,ebx,1
	shrd	cx,dx,1
	shrd	ecx,edx,1
	stosd
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
;=================================== 80387 ====================================
	fcos
	fprem1
	fsin
	fsincos
	fucom
	fucomp
	fucompp
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
;=================================== 80486 ====================================
	bswap	eax
	cmpxchg	eax,ebx
	invd
	invlpg	[si]
	wbinvd
	xadd	[si],ax
	cpuid				; <- NOTE: Officially on the Pentium, but late 486's had this
;============================ visible pause in decoding ==========================
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

