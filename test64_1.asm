; x86-64 test assembly language
bits 64
	mov		rax,[rsi]
	mov		[rsi],rax
	mov		rax,[es:rdi]
	mov		[es:rdi],rax
	mov		rax,[rdi]
	mov		[rdi],rax

	mov		cr0,rax
	mov		rax,cr0
	mov		cr1,rax
	mov		rax,cr1
	mov		cr8,rax
	mov		rax,cr8
	mov		cr15,rax
	mov		rax,cr15

	mov		dr0,rax
	mov		rax,dr0
	mov		dr1,rax
	mov		rax,dr1
	mov		dr8,rax
	mov		rax,dr8
	mov		dr11,rax
	mov		rax,dr11
	mov		dr15,rax
	mov		rax,dr15

	mov		[ds:4],rax

	push		byte 0x12
	push		word 0x1234
	push		qword 0x12345678

	mov		byte [abs 0x12345678],al
	mov		word [abs 0x12345678],ax
	mov		dword [abs 0x12345678],eax
	mov		qword [abs 0x12345678],rax

	mov		al,byte [abs dword a32 0x12345678]
	mov		ax,word [abs dword a32 0x12345678]
	mov		eax,dword [abs dword a32 0x12345678]
	mov		rax,qword [abs dword a32 0x12345678]

	mov		byte [abs dword a32 0x12345678],al
	mov		word [abs dword a32 0x12345678],ax
	mov		dword [abs dword a32 0x12345678],eax
	mov		qword [abs dword a32 0x12345678],rax

	mov		al,byte [abs qword a64 0x123456789ABCDEF]
	mov		ax,word [abs qword a64 0x123456789ABCDEF]
	mov		eax,dword [abs qword a64 0x123456789ABCDEF]
	mov		rax,qword [abs qword a64 0x123456789ABCDEF]

	mov		byte [abs qword a64 0x123456789ABCDEF],al
	mov		word [abs qword a64 0x123456789ABCDEF],ax
	mov		dword [abs qword a64 0x123456789ABCDEF],eax
	mov		qword [abs qword a64 0x123456789ABCDEF],rax

	pop		word [rsi]
	pop		qword [rsi]

	vprotb		xmm1,xmm2,xmm3
	vprotb		xmm1,[rsi],xmm3
	vprotw		xmm1,xmm2,xmm3
	vprotw		xmm1,[rsi],xmm3
	vprotd		xmm1,xmm2,xmm3
	vprotd		xmm1,[rsi],xmm3
	vprotq		xmm1,xmm2,xmm3
	vprotq		xmm1,[rsi],xmm3

	vpshlb		xmm1,xmm2,xmm3
	vpshlb		xmm1,[rsi],xmm3
	vpshlw		xmm1,xmm2,xmm3
	vpshlw		xmm1,[rsi],xmm3
	vpshld		xmm1,xmm2,xmm3
	vpshld		xmm1,[rsi],xmm3
	vpshlq		xmm1,xmm2,xmm3
	vpshlq		xmm1,[rsi],xmm3

	vpshab		xmm1,xmm2,xmm3
	vpshab		xmm1,[rsi],xmm3
	vpshaw		xmm1,xmm2,xmm3
	vpshaw		xmm1,[rsi],xmm3
	vpshad		xmm1,xmm2,xmm3
	vpshad		xmm1,[rsi],xmm3
	vpshaq		xmm1,xmm2,xmm3
	vpshaq		xmm1,[rsi],xmm3

	vphsubbw	xmm1,xmm2
	vphsubbw	xmm1,[rsi]
	vphsubwd	xmm1,xmm2
	vphsubwd	xmm1,[rsi]
	vphsubdq	xmm1,xmm2
	vphsubdq	xmm1,[rsi]

	vphaddudq	xmm1,xmm2
	vphaddudq	xmm1,[rsi]

	vphadduwd	xmm1,xmm2
	vphadduwd	xmm1,[rsi]
	vphadduwq	xmm1,xmm2
	vphadduwq	xmm1,[rsi]

	vphaddubw	xmm1,xmm2
	vphaddubw	xmm1,[rsi]
	vphaddubd	xmm1,xmm2
	vphaddubd	xmm1,[rsi]
	vphaddubq	xmm1,xmm2
	vphaddubq	xmm1,[rsi]

	vphadddq	xmm1,xmm2
	vphadddq	xmm1,[rsi]

	vphaddwd	xmm1,xmm2
	vphaddwd	xmm1,[rsi]
	vphaddwq	xmm1,xmm2
	vphaddwq	xmm1,[rsi]

	vphaddbw	xmm1,xmm2
	vphaddbw	xmm1,[rsi]
	vphaddbd	xmm1,xmm2
	vphaddbd	xmm1,[rsi]
	vphaddbq	xmm1,xmm2
	vphaddbq	xmm1,[rsi]

	vpmacssww	xmm1,xmm2,xmm3,xmm4
	vpmacssww	xmm1,xmm2,[rsi],xmm4
	vpmacsswd	xmm1,xmm2,xmm3,xmm4
	vpmacsswd	xmm1,xmm2,[rsi],xmm4
	vpmacssdd	xmm1,xmm2,xmm3,xmm4
	vpmacssdd	xmm1,xmm2,[rsi],xmm4

	vpmacsww	xmm1,xmm2,xmm3,xmm4
	vpmacsww	xmm1,xmm2,[rsi],xmm4
	vpmacswd	xmm1,xmm2,xmm3,xmm4
	vpmacswd	xmm1,xmm2,[rsi],xmm4
	vpmacsdd	xmm1,xmm2,xmm3,xmm4
	vpmacsdd	xmm1,xmm2,[rsi],xmm4

	vpcmov		xmm1,xmm2,xmm3,xmm4
	vpcmov		xmm1,xmm2,[rsi],xmm4
	vpperm		xmm1,xmm2,xmm3,xmm4
	vpperm		xmm1,xmm2,[rsi],xmm4
	vpmadcsswd	xmm1,xmm2,xmm3,xmm4
	vpmadcsswd	xmm1,xmm2,[rsi],xmm4
	vpmadcswd	xmm1,xmm2,xmm3,xmm4
	vpmadcswd	xmm1,xmm2,[rsi],xmm4

	vpcomb		xmm1,xmm2,xmm3,4
	vpcomb		xmm1,xmm2,[rsi],4
	vpcomw		xmm1,xmm2,xmm3,4
	vpcomw		xmm1,xmm2,[rsi],4
	vpcomd		xmm1,xmm2,xmm3,4
	vpcomd		xmm1,xmm2,[rsi],4
	vpcomq		xmm1,xmm2,xmm3,4
	vpcomq		xmm1,xmm2,[rsi],4

	vpcomb		xmm1,xmm2,xmm3,0xA5
	vpcomb		xmm1,xmm2,[rsi],0xA5
	vpcomw		xmm1,xmm2,xmm3,0xA5
	vpcomw		xmm1,xmm2,[rsi],0xA5
	vpcomd		xmm1,xmm2,xmm3,0xA5
	vpcomd		xmm1,xmm2,[rsi],0xA5
	vpcomq		xmm1,xmm2,xmm3,0xA5
	vpcomq		xmm1,xmm2,[rsi],0xA5

	vpcomub		xmm1,xmm2,xmm3,0xA5
	vpcomub		xmm1,xmm2,[rsi],0xA5
	vpcomuw		xmm1,xmm2,xmm3,0xA5
	vpcomuw		xmm1,xmm2,[rsi],0xA5
	vpcomud		xmm1,xmm2,xmm3,0xA5
	vpcomud		xmm1,xmm2,[rsi],0xA5
	vpcomuq		xmm1,xmm2,xmm3,0xA5
	vpcomuq		xmm1,xmm2,[rsi],0xA5

	vprotb		xmm1,xmm2,3
	vprotb		xmm1,[rsi],3
	vprotw		xmm1,xmm2,3
	vprotw		xmm1,[rsi],3
	vprotd		xmm1,xmm2,3
	vprotd		xmm1,[rsi],3
	vprotq		xmm1,xmm2,3
	vprotq		xmm1,[rsi],3

	vfrczps		xmm1,xmm2
	vfrczps		xmm1,[rsi]

	vfrczpd		xmm1,xmm2
	vfrczpd		xmm1,[rsi]

	vfrczss		xmm1,xmm2
	vfrczss		xmm1,[rsi]

	vfrczsd		xmm1,xmm2
	vfrczsd		xmm1,[rsi]

	vfmaddpd	xmm1,xmm2,xmm3,xmm4
	vfmaddpd	xmm1,xmm2,[rsi],xmm4
	vfmaddpd	ymm1,ymm2,ymm3,ymm4
	vfmaddpd	ymm1,ymm2,[rsi],ymm4
	vfmaddps	xmm1,xmm2,xmm3,xmm4
	vfmaddps	xmm1,xmm2,[rsi],xmm4
	vfmaddps	ymm1,ymm2,ymm3,ymm4
	vfmaddps	ymm1,ymm2,[rsi],ymm4

	mov	bl,12h
	mov	bx,1234h
	mov	ebx,12345678h
	mov	rbx,123456789ABCDEF0h

	mov	byte [rsi],12h
	mov	word [rsi],1234h
	mov	dword [rsi],12345678h
	mov	qword [rsi],12345678h
	mov	qword [rsi],0xFFFFFFFFEDCBA987

	test	al,12h
	test	ax,1234h
	test	eax,12345678h
	test	rax,12345678h
	test	rax,0xFFFFFFFFEDCBA987

	not	al
	not	ax
	not	eax
	not	rax
	mul	al
	mul	ax
	mul	eax
	mul	rax

	in	al,21h
	in	ax,21h
	in	eax,21h
	in	al,dx
	in	ax,dx
	in	eax,dx
	out	21h,al
	out	21h,ax
	out	21h,eax
	out	dx,al
	out	dx,ax
	out	dx,eax
	cli
	sti
	cmc
	hlt

	mov	al,[1]
	mov	ax,[1]
	mov	eax,[1]
	mov	rax,[1]

	xchg	al,ah
	xchg	ax,bx
	xchg	eax,ebx
	xchg	rax,rbx

	cwd
	cdq
	cqo

	cbw
	cwde
	cdqe

	jecxz	$
	jrcxz	$

	jz	$
	jnz	$
	jc	$
	jnc	$

	rol	byte [cs:esi],1
	rol	byte [ds:esi],1
	rol	byte [es:esi],1
	rol	byte [fs:esi],1
	rol	byte [gs:esi],1
	rol	byte [ss:esi],1

	pop	ax
	pop	rax
	push	ax
	push	rax

	rol	al,1
	rol	ax,1
	rol	eax,1
	rol	rax,1

	rol	al,2
	rol	ax,2
	rol	eax,2
	rol	rax,2

	rol	byte [esi],1
	rol	word [esi],1
	rol	dword [esi],1
	rol	qword [esi],1

	rol	byte [rsi],1
	rol	word [rsi],1
	rol	dword [rsi],1
	rol	qword [rsi],1

	ror	al,1
	ror	ax,1
	ror	eax,1
	ror	rax,1

	rcl	al,1
	rcl	ax,1
	rcl	eax,1
	rcl	rax,1

	rcr	al,1
	rcr	ax,1
	rcr	eax,1
	rcr	rax,1

	shl	al,1
	shl	ax,1
	shl	eax,1
	shl	rax,1

	shr	al,1
	shr	ax,1
	shr	eax,1
	shr	rax,1

	sar	al,1
	sar	ax,1
	sar	eax,1
	sar	rax,1

	call	12345678h
	jmp	12345678h
	jmp	short $

	test	al,12h
	test	ax,1234h
	test	eax,12345678h
	test	rax,12345678h
	test	rax,0FFFFFFFFFFFFFFFFh

	lea	rsi,[rel mark2]
	lea	esi,[rel mark2]
	lea	si,[rel mark2]
mark2:

	lea	eax,[abs 1234h]
	lea	eax,[rax-3]
	lea	eax,[rbx+7fh]
	lea	eax,[rcx+0x180]
	lea	rax,[rbx*4+rcx-5]
	lea	ax,[esi+ebx]
	lea	r14,[r8d+esi]
	lea	r8,[r9+rdi]

	lea	rsi,[r8*8+r9+22224444h]
	lea	r15,[r15*2+r14-7ffffffh]

	mov	ax,es
	mov	ds,ax
	mov	bx,cs
	mov	cs,bx
	mov	cx,ds
	mov	dx,ss
	mov	[edi],es
	mov	[esi],ds

	a32	movsb

	movsb
	movsw
	movsd
	movsq

	cmpsb
	cmpsw
	cmpsd
	cmpsq

	stosb
	stosw
	stosd
	stosq

	lodsb
	lodsw
	lodsd
	lodsq

	xchg	bl,dl
	xchg	[esi],cl
	xchg	[edi],si

	mov	al,12h
	mov	ah,12h
	mov	bl,34h
	mov	bh,34h
	mov	cl,34h
	mov	ch,34h
	mov	dl,34h
	mov	dh,34h
	mov	sil,23h
	mov	dil,23h
	mov	spl,23h
	mov	bpl,23h
	mov	r8b,24h
	mov	r14b,24h
	mov	ax,1234h
	mov	bx,1234h
	mov	si,1234h
	mov	r8w,1234h
	mov	r15w,1234h
	mov	eax,12345678h
	mov	ebx,12345678h
	mov	esi,12345678h
	mov	r8d,12345678h
	mov	r14d,12345678h
	mov	rax,0123456789ABCDEFh
	mov	rcx,0123456789ABCDEFh
	mov	r8,0123456789ABCDEFh
	mov	r13,0123456789ABCDEFh

	mov	al,bl
	mov	cl,dl
	mov	ax,bx
	mov	cx,dx
	mov	eax,ebx
	mov	ecx,edx
	mov	rax,rbx
	mov	rcx,rdx

	mov	al,ah
	mov	sil,al
	mov	r8,r9

	mov	al,[rax]
	mov	ax,[rax]
	mov	eax,[rax]
	mov	rax,[rax]
	mov	rax,[rax]
	mov	rax,[rax+rbx]
	mov	rax,[rax+r8]
	mov	rax,[rax*8+r8]
	mov	rax,[rax*8+r8+4]
	mov	rax,[rax*8+r8+55555]
	mov	rax,[rsi+rdi]
	mov	rax,[rsi+rdi]
	mov	rax,[rsp+rax]
	mov	rax,[rax+rsp]

	mov	al,[r8]
	mov	ax,[r9]
	mov	eax,[r10]
	mov	rax,[r11]
	mov	rax,[r12]
	mov	rax,[r8+r9]
	mov	rax,[r8*4+r9]
	mov	rax,[r8*4+r9+44]
	mov	rax,[r8*4+r9+5555]
	mov	rax,[r11+r8]
	mov	rax,[r8+r11]
	mov	rax,[r12+r8]
	mov	rax,[r8+r12]
	mov	rax,[r8*8+r8+44]
	mov	rax,[r8*8+r9+44]
	mov	rax,[r8*8+r10+44]
	mov	rax,[r8*8+r11+44]
	mov	rax,[r8*8+r12+44]
	mov	rax,[r8*8+r13+44]
	mov	rax,[r8*8+r14+44]
	mov	rax,[r8*8+r15+44]
	mov	rax,[r13*8+r8+44]
	mov	rax,[r13*8+r9+44]
	mov	rax,[r13*8+r10+44]
	mov	rax,[r13*8+r11+44]
	mov	rax,[r13*8+r12+44]
	mov	rax,[r13*8+r13+44]
	mov	rax,[r13*8+r14+44]
	mov	rax,[r13*8+r15+44]

	add	al,[r8]
	add	ax,[r9]
	add	eax,[r10]
	add	rax,[r11]
	add	rax,[r12]
	add	rax,[r8+r9]
	add	rax,[r8*4+r9]
	add	rax,[r8*4+r9+44]
	add	rax,[r8*4+r9+5555]
	add	rax,[r11+r8]
	add	rax,[r8+r11]
	add	rax,[r12+r8]
	add	rax,[r8+r12]
	add	rax,[r8*8+r8+44]
	add	rax,[r8*8+r9+44]
	add	rax,[r8*8+r10+44]
	add	rax,[r8*8+r11+44]
	add	rax,[r8*8+r12+44]
	add	rax,[r8*8+r13+44]
	add	rax,[r8*8+r14+44]
	add	rax,[r8*8+r15+44]
	add	rax,[r13*8+r8+44]
	add	rax,[r13*8+r9+44]
	add	rax,[r13*8+r10+44]
	add	rax,[r13*8+r11+44]
	add	rax,[r13*8+r12+44]
	add	rax,[r13*8+r13+44]
	add	rax,[r13*8+r14+44]
	add	rax,[r13*8+r15+44]

	test	byte [esi],al
	test	byte [esi],ah
	test	byte [esi],r9b
	test	word [esi],bx
	test	word [esi],cx
	test	word [esi],r10w
	test	dword [esi],edx
	test	dword [esi],esi
	test	dword [esi],r11d
	test	qword [esi],rbx
	test	qword [esi],rcx
	test	qword [esi],r12

	test	byte [rsi],al
	test	byte [rsi],ah
	test	byte [rsi],r9b
	test	word [rsi],bx
	test	word [rsi],cx
	test	word [rsi],r10w
	test	dword [rsi],edx
	test	dword [rsi],esi
	test	dword [rsi],r11d
	test	qword [rsi],rbx
	test	qword [rsi],rcx
	test	qword [rsi],r12

	add	bl,[esi]
	add	bh,[edi]
	add	[ebx],dh
	add	[ecx],cl
	add	bx,[esi]
	add	cx,[edi]
	add	al,12h
	add	eax,012345678h
	add	eax,0FFEAAAAAh
	add	rax,012345678h
	add	ebx,ecx
	add	rbx,rsi
	add	rcx,r12
	add	r15,r13
	add	r14,rbx
	add	dh,al
	add	bh,bl
	add	sil,al
	add	dil,bl
	add	al,sil
	add	bl,dil
	add	sil,dil
	mov	rax,00123456789ABCDEFh
	mov	eax,012345678h
	adc	eax,012345678h
	adc	eax,0FFEAAAAAh
	adc	rax,012345678h
	sbb	eax,012345678h
	sbb	eax,0FFEAAAAAh
	sbb	rax,012345678h
	sub	eax,012345678h
	sub	eax,0FFEAAAAAh
	sub	rax,012345678h
	mov	rbx,088776655332211h
	mov	rsi,02483475827353h
	mov	r12,03278954623874623h
	mov	r15,023897412789614h
	add	r8b,r9b
	add	r8w,r9w
	add	r8d,r9d
	add	r8,r9

	add	bl,[rax]
	add	bl,[rbx]
	add	bl,[rcx]
	add	bl,[rdx]
	add	bl,[rsi]
	add	bl,[rdi]
	add	bl,[rbp]
	add	bl,[rsp]

	add	bl,[eax]
	add	bl,[ebx]
	add	bl,[ecx]
	add	bl,[edx]
	add	bl,[esi]
	add	bl,[edi]
	add	bl,[ebp]
	add	bl,[esp]

	add	bl,[rax+rbx]
	add	bl,[eax+ebx]
	add	bl,[rax*4+rbx]
	add	bl,[eax*4+ebx]
	add	bl,[rcx*8+rdx+54h]
	add	bl,[rdx*2+rsi+12345678h]
	add	bl,[abs 1]
	add	bl,[rel $+1+6]	; the instruction is 6 bytes long, we want RIP+1
	add	bl,[ecx*8+edx+54h]
	add	bl,[edx*2+esi+12345678h]

	add	bl,r9b

	add	ax,1235h
	add	ax,cx
	add	ax,r9w
	add	r10w,r15w
	add	r13w,[rel mark1]
	add	rbx,[rel mark1 + rax]	; NTS: Apparently this becomes [offset mark1 + rax]
	add	rbx,[rel mark1]
	add	rbx,[rel mark1]
	add	rbx,[rel mark1]
mark1:
	add	bl,[r8]
	add	bl,[r13]
	add	bl,[r15]
	add	bl,[r8+r9]
	add	bl,[rsi+r9]
	add	bl,[rdi*4+r14+222h]

	; group 0x00-0x3F test
	add	rsi,r15
	or	rsi,r15
	adc	rsi,r15
	sbb	rsi,r15
	and	rsi,r15
	sub	rsi,r15
	xor	rsi,r15
	cmp	rsi,r15
	add	rax,12345678h		; sadly, you can't do 64-bit immediates
	or	rax,12345678h		; sadly, you can't do 64-bit immediates
	adc	rax,12345678h		; sadly, you can't do 64-bit immediates
	sbb	rax,12345678h		; sadly, you can't do 64-bit immediates
	and	rax,12345678h		; sadly, you can't do 64-bit immediates
	sub	rax,12345678h		; sadly, you can't do 64-bit immediates
	xor	rax,12345678h		; sadly, you can't do 64-bit immediates
	cmp	rax,12345678h		; sadly, you can't do 64-bit immediates
	add	eax,12345678h
	or	eax,12345678h
	adc	eax,12345678h
	sbb	eax,12345678h
	and	eax,12345678h
	sub	eax,12345678h
	xor	eax,12345678h
	cmp	eax,12345678h
	add	ax,1234h
	or	ax,1234h
	adc	ax,1234h
	sbb	ax,1234h
	and	ax,1234h
	sub	ax,1234h
	xor	ax,1234h
	cmp	ax,1234h

; taken from test1.asm
	add	bl,[esp]
	add	bl,[a32 12345678h]
	add	bl,[a32 12345678h+edx+eax]
	add	bl,[ebp]
	add	bl,[ebp+42h]

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

	idiv	rax
	idiv	rbx
	idiv	rcx
	idiv	rdx
	idiv	rsi
	idiv	rdi
	idiv	word [rax]
	idiv	word [rbx]
	idiv	word [rcx]
	idiv	word [rdx]
	idiv	word [rsi]
	idiv	word [rdi]

	imul	rax
	imul	rbx
	imul	rcx
	imul	rdx
	imul	rsi
	imul	rdi
	imul	word [rax]
	imul	word [rbx]
	imul	word [rcx]
	imul	word [rdx]
	imul	word [rsi]
	imul	word [rdi]

	lea	bx,[esi+3]
	lea	cx,[ebx+edi+33h]
	test	ax,bx
	test	si,cx
	test	cl,bh
	test	[edi],bx
	test	[esi+ebx+43],cx
	cmp	bl,44h
	cmp	bx,2446h
	cmp	byte [ebx+3],35h
	cmp	word [ebp-3],2244h
	cmp	word [edi],62h
	mov	ah,dh
	mov	bl,[esi+ebx+3]
	mov	si,[edi+ebx+34h]
	inc	ax
	inc	bx
	inc	cx
	inc	dx
	inc	esi
	inc	eax
	inc	r8d
	inc	r9b
	inc	r10w
	inc	r11
	inc	rsi
;

	mov	cr0,rax
	mov	cr0,rbx
	mov	rax,cr0
	mov	cr2,rax
	mov	rax,cr2
	mov	cr6,rcx
	mov	cr7,rdx
	mov	cr8,rsi
	mov	cr9,rdi
	mov	rax,cr6
	mov	rbx,cr7
	mov	rcx,cr8
	mov	rdx,cr9
	mov	dr0,rax
	mov	rax,dr0
	mov	dr3,rax
	mov	rax,dr3
	mov	dr6,rcx
	mov	dr7,rdx
	mov	dr8,rsi
	mov	dr9,rdi
	mov	rax,dr6
	mov	rbx,dr7
	mov	rcx,dr8
	mov	rdx,dr9

	nop

	push	byte 0x12
	push	word 0x1234
	push	dword 0x12345678

;===================general================
	adc	al,al
	adc	al,0x10
	add	al,al
	add	al,0x10
	and	al,al
	and	al,0x10
	bsf	ax,ax
	bsr	ax,ax
	bswap	eax
	bt	ax,ax
	btc	ax,ax
	btr	ax,ax
	bts	ax,ax
	call	0x1234
	cbw
	cwde
	cwd
	cdq
	clc
	cld
	clflush	[rsi]
	cmc
	cmovz	ax,ax
	cmovnz	ax,ax
	cmp	al,al
	cmp	al,0x10
	cmpsb
	cmpsw
	cmpxchg	al,al
	cmpxchg	ax,ax
	cmpxchg8b [rsi]
	cpuid
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
	jz	short $
	jnz	short $
	jz	near $
	jnz	near $
	jecxz	$
	jrcxz	$
	jmp	short $
	jmp	near $
	jmp	[rsi]
	lahf
	lfs	ax,[rsi]
	lgs	ax,[rsi]
	lss	ax,[rsi]
	lea	ax,[rsi]
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
	movsx	bx,byte [rsi]
	movsx	ebx,byte [rsi]
	movsx	ebx,word [rsi]
	movzx	bx,byte [rsi]
	movzx	ebx,byte [rsi]
	movzx	ebx,word [rsi]
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
	pop	rax
	pop	si
	pop	rsi
	pop	fs
	popaq
	popcnt	ax,ax
	popcnt	rax,rax
	popf
	popfq
	prefetch [esi]
	prefetchw [esi]
	prefetchnta [esi]
	prefetcht0 [esi]
	prefetcht1 [esi]
	prefetcht2 [esi]
	push	ax
	push	rax
	push	fs
	pushaq
	pushfq
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
	db	0x0F,0x01,0xDD		; clgi
	cli
	clts
	hlt
	int	3
	invd
	invlpg	[rsi]
	invlpga	eax,ecx
	iret
	iretd
	lar	ax,ax
	lgdt	[rsi]
	lidt	[rsi]
	lldt	ax
	lmsw	ax
	lsl	ax,ax
	ltr	ax
	monitor
	mov	cr0,rax
	mov	rax,cr0
	mov	dr0,rax
	mov	rax,dr0
	mwait
	rdmsr
	rdpmc
	rdtsc
	rdtscp
	rsm
	sgdt	[rsi]
	sidt	[rsi]
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
	fadd	dword [rsi]
	fadd	qword [rsi]
	faddp	st3,st0
	faddp	st1,st0
	fiadd	word [rsi]
	fiadd	dword [rsi]
	fbld	[rsi]
	fbstp	[rsi]
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
	fcom	dword [rsi]
	fcom	qword [rsi]
	fcomp	st2
	fcomp	dword [rsi]
	fcomp	qword [rsi]
	fcompp
	fcomi	st2
	fcomip	st2
	fcos
	fdecstp
	fdiv	st0,st2
	fdiv	st2,st0
	fdiv	dword [rsi]
	fdiv	qword [rsi]
	fdivp	st2
	fidiv	word [rsi]
	fidiv	dword [rsi]
	fdivr	st0,st2
	fdivr	st2,st0
	fdivr	dword [rsi]
	fdivr	qword [rsi]
	fdivrp	st2
	fidivr	word [rsi]
	fidivr	dword [rsi]
	ffree	st2
	ficom	word [rsi]
	ficom	dword [rsi]
	ficomp	word [rsi]
	ficomp	dword [rsi]
	fild	word [rsi]
	fild	dword [rsi]
	fild	qword [rsi]
	finit
	fninit
	fist	word [rsi]
	fist	dword [rsi]
	fistp	word [rsi]
	fistp	dword [rsi]
	fistp	qword [rsi]
	fld	st2
	fld	dword [rsi]
	fld	qword [rsi]
	fld	tword [rsi]
	fld1
	fldcw	[rsi]
	fldenv	[rsi]
	fldl2e
	fldl2t
	fldlg2
	fldln2
	fldpi
	fldz
	fmul	st0,st2
	fmul	st2,st0
	fmul	dword [rsi]
	fmul	qword [rsi]
	fmulp	st2
	fimul	word [rsi]
	fimul	dword [rsi]
	fnop
	fpatan
	fprem
	fprem1
	fptan
	frndint
	frstor	[rsi]
	fsave	[rsi]
	fnsave	[rsi]
	fscale
	fsin
	fsincos
	fsqrt
	fst	st2
	fst	dword [rsi]
	fst	qword [rsi]
	fstp	st2
	fstp	dword [rsi]
	fstp	qword [rsi]
	fstp	tword [rsi]
	fstcw	[rsi]
	fnstcw	[rsi]
	fstenv	[rsi]
	fnstenv	[rsi]
	fstsw	ax
	fstsw	[rsi]
	fnstsw	ax
	fnstsw	[rsi]
	fsub	st0,st2
	fsub	st2,st0
	fsub	dword [rsi]
	fsub	qword [rsi]
	fsubp	st2
	fisub	word [rsi]
	fisub	dword [rsi]
	fsubr	st0,st2
	fsubr	st2,st0
	fsubr	dword [rsi]
	fsubr	qword [rsi]
	fsubrp	st2
	fisubr	word [rsi]
	fisubr	dword [rsi]
	ftst
	fucom	st2
	fucomp	st2
	fucompp
	fucomi	st2
	fucomip	st2
	fwait
	fxam
	fxch	st2
	fxrstor	[rsi]
	fxsave	[rsi]
	fxtract
	fyl2x
	fyl2xp1
;===================MMX=================
	cvtpd2pi	mm0,xmm1
	cvtpd2pi	mm0,[rsi]
	cvtpi2pd	xmm0,mm1
	cvtpi2pd	xmm0,[rsi]
	cvtpi2ps	xmm0,mm1
	cvtpi2ps	xmm0,[rsi]
	cvtps2pi	mm0,xmm1
	cvtps2pi	mm0,[rsi]
	cvttpd2pi	mm0,xmm1
	cvttpd2pi	mm0,[rsi]
	cvttps2pi	mm0,xmm1
	cvttps2pi	mm0,[rsi]
	emms
	femms
	fnsave		[rsi]
	fsave		[rsi]
	frstor		[rsi]
	fxrstor		[rsi]
	fxsave		[rsi]
	maskmovq	mm0,mm1
	maskmovq	mm1,mm0
	movdq2q		mm0,xmm1
	movntq		[rsi],mm1
	movd		mm0,eax
	movd		mm0,[rsi]
	movd		eax,mm1
	movd		[rsi],mm1
	movq		mm0,mm1
	movq		mm1,[rsi]
	movq		[rsi],mm0
	movq2dq		xmm1,mm0
	packssdw	mm0,mm1
	packssdw	mm0,[rsi]
	packsswb	mm0,mm1
	packsswb	mm0,[rsi]
	packuswb	mm0,mm1
	packuswb	mm0,[rsi]
	paddb		mm0,mm1
	paddb		mm0,[rsi]
	paddd		mm0,mm1
	paddd		mm0,[rsi]
	paddq		mm0,mm1
	paddq		mm0,[rsi]
	paddsb		mm0,mm1
	paddsb		mm0,[rsi]
	paddsw		mm0,mm1
	paddsw		mm0,[rsi]
	paddusb		mm0,mm1
	paddusb		mm0,[rsi]
	paddusw		mm0,mm1
	paddusw		mm0,[rsi]
	paddw		mm0,mm1
	paddw		mm0,[rsi]
	pand		mm0,mm1
	pand		mm0,[rsi]
	pandn		mm0,mm1
	pandn		mm0,[rsi]
	pavgb		mm0,mm1
	pavgb		mm0,[rsi]
	pavgusb		mm0,mm1
	pavgusb		mm0,[rsi]
	pavgw		mm0,mm1
	pavgw		mm0,[rsi]
	pcmpeqb		mm0,mm1
	pcmpeqb		mm0,[rsi]
	pcmpeqd		mm0,mm1
	pcmpeqd		mm0,[rsi]
	pcmpeqw		mm0,mm1
	pcmpeqw		mm0,[rsi]
	pcmpgtb		mm0,mm1
	pcmpgtb		mm0,[rsi]
	pcmpgtd		mm0,mm1
	pcmpgtd		mm0,[rsi]
	pcmpgtw		mm0,mm1
	pcmpgtw		mm0,[rsi]
	pextrw		eax,mm0,0
	pextrw		ebx,mm1,1
	pf2id		mm0,mm1
	pf2id		mm0,[rsi]
	pf2iw		mm0,mm1
	pf2iw		mm0,[rsi]
	pfacc		mm0,mm1
	pfacc		mm0,[rsi]
	pfadd		mm0,mm1
	pfadd		mm0,[rsi]
	pfcmpeq		mm0,mm1
	pfcmpeq		mm0,[rsi]
	pfcmpge		mm0,mm1
	pfcmpge		mm0,[rsi]
	pfcmpgt		mm0,mm1
	pfcmpgt		mm0,[rsi]
	pfmax		mm0,mm1
	pfmax		mm0,[rsi]
	pfmin		mm0,mm1
	pfmin		mm0,[rsi]
	pfmul		mm0,mm1
	pfmul		mm0,[rsi]
	pfnacc		mm0,mm1
	pfnacc		mm0,[rsi]
	pfpnacc		mm0,mm1
	pfpnacc		mm0,[rsi]
	pfrcp		mm0,mm1
	pfrcp		mm0,[rsi]
	pfrcpit1	mm0,mm1
	pfrcpit1	mm0,[rsi]
	pfrcpit2	mm0,mm1
	pfrcpit2	mm0,[rsi]
	pfrsqit1	mm0,mm1
	pfrsqit1	mm0,[rsi]
	pfrsqrt		mm0,mm1
	pfrsqrt		mm0,[rsi]
	pfsub		mm0,mm1
	pfsub		mm0,[rsi]
	pfsubr		mm0,mm1
	pfsubr		mm0,[rsi]
	pi2fd		mm0,mm1
	pi2fd		mm0,[rsi]
	pi2fw		mm0,mm1
	pi2fw		mm0,[rsi]
	pinsrw		mm0,eax,0
	pinsrw		mm1,ebx,1
	pmaddwd		mm0,mm1
	pmaddwd		mm0,[rsi]
	pmaxsw		mm0,mm1
	pmaxsw		mm0,[rsi]
	pmaxub		mm0,mm1
	pmaxub		mm0,[rsi]
	pminsw		mm0,mm1
	pminsw		mm0,[rsi]
	pminub		mm0,mm1
	pminub		mm0,[rsi]
	pmovmskb	eax,mm0
	pmulhrwa	mm0,mm1
	pmulhrwa	mm0,[rsi]
	pmulhuw		mm0,mm1
	pmulhuw		mm0,[rsi]
	pmulhw		mm0,mm1
	pmulhw		mm0,[rsi]
	pmullw		mm0,mm1
	pmullw		mm0,[rsi]
	pmuludq		mm0,mm1
	pmuludq		mm0,[rsi]
	por		mm0,mm1
	por		mm0,[rsi]
	psadbw		mm0,mm1
	psadbw		mm0,[rsi]
	pshufw		mm0,mm1,0
	pshufw		mm0,[rsi],0
	pshufw		mm0,mm1,2
	pshufw		mm0,[rsi],2
	pslld		mm0,mm1
	pslld		mm0,[rsi]
	pslld		mm0,1
	psllq		mm0,mm1
	psllq		mm0,[rsi]
	psllq		mm0,1
	psllw		mm0,mm1
	psllw		mm0,[rsi]
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
	psubb		mm0,[rsi]
	psubw		mm0,mm1
	psubw		mm0,[rsi]
	psubd		mm0,mm1
	psubd		mm0,[rsi]
	psubq		mm0,mm1
	psubq		mm0,[rsi]
	psubsb		mm0,mm1
	psubsb		mm0,[rsi]
	psubsw		mm0,mm1
	psubsw		mm0,[rsi]
	psubusb		mm0,mm1
	psubusb		mm0,[rsi]
	psubusw		mm0,mm1
	psubusw		mm0,[rsi]
	pswapd		mm0,mm1
	pswapd		mm0,[rsi]
	punpckhbw	mm0,mm1
	punpckhbw	mm0,[rsi]
	punpckhdq	mm0,mm1
	punpckhdq	mm0,[rsi]
	punpckhwd	mm0,mm1
	punpckhwd	mm0,[rsi]
	punpcklbw	mm0,mm1
	punpcklbw	mm0,[rsi]
	punpckldq	mm0,mm1
	punpckldq	mm0,[rsi]
	punpcklwd	mm0,mm1
	punpcklwd	mm0,[rsi]
	pxor		mm0,mm1
	pxor		mm0,[rsi]
;===================SSE=================
	addpd		xmm1,xmm2
	addpd		xmm1,[rsi]
	addps		xmm1,xmm2
	addps		xmm1,[rsi]
	addsd		xmm1,xmm2
	addsd		xmm1,[rsi]
	addss		xmm1,xmm2
	addss		xmm1,[rsi]
	addsubpd	xmm1,xmm2
	addsubpd	xmm1,[rsi]
	addsubps	xmm1,xmm2
	addsubps	xmm1,[rsi]
	andnpd		xmm1,xmm2
	andnpd		xmm1,[rsi]
	andnps		xmm1,xmm2
	andnps		xmm1,[rsi]
	andpd		xmm1,xmm2
	andpd		xmm1,[rsi]
	andps		xmm1,xmm2
	andps		xmm1,[rsi]
	cmppd		xmm1,xmm2,0
	cmppd		xmm1,[rsi],0
	cmpps		xmm1,xmm2,0
	cmpps		xmm1,[rsi],0
	cmpsd		xmm1,xmm2,0
	cmpsd		xmm1,[rsi],0
	cmpss		xmm1,xmm2,0
	cmpss		xmm1,[rsi],0
	comisd		xmm1,xmm2
	comisd		xmm1,[rsi]
	comiss		xmm1,xmm2
	comiss		xmm1,[rsi]
	cvtdq2pd	xmm1,xmm2
	cvtdq2pd	xmm1,[rsi]
	cvtdq2ps	xmm1,xmm2
	cvtdq2ps	xmm1,[rsi]
	cvtpd2dq	xmm1,xmm2
	cvtpd2dq	xmm1,[rsi]
	cvtpd2pi	mm1,xmm2
	cvtpd2pi	mm1,[rsi]
	cvtpd2ps	xmm1,xmm2
	cvtpd2ps	xmm1,[rsi]
	cvtpi2pd	xmm1,mm2
	cvtpi2pd	xmm1,[rsi]
	cvtpi2ps	xmm1,mm2
	cvtpi2ps	xmm1,[rsi]
	cvtps2dq	xmm1,xmm2
	cvtps2dq	xmm1,[rsi]
	cvtps2pd	xmm1,xmm2
	cvtps2pd	xmm1,[rsi]
	cvtps2pi	mm1,xmm2
	cvtps2pi	mm1,[rsi]
	cvtsd2si	eax,xmm1
	cvtsd2si	eax,[rsi]
	cvtsd2ss	xmm1,xmm2
	cvtsd2ss	xmm1,[rsi]
	cvtsi2sd	xmm1,eax
	cvtsi2sd	xmm1,[rsi]
	cvtsi2ss	xmm1,eax
	cvtsi2ss	xmm1,[rsi]
	cvtss2sd	xmm1,xmm2
	cvtss2sd	xmm1,[rsi]
	cvtss2si	eax,xmm2
	cvtss2si	eax,[rsi]
	cvttpd2dq	xmm1,xmm2
	cvttpd2dq	xmm1,[rsi]
	cvttpd2pi	mm1,xmm2
	cvttpd2pi	mm1,[rsi]
	cvttps2dq	xmm1,xmm2
	cvttps2dq	xmm1,[rsi]
	cvttps2pi	mm1,xmm2
	cvttps2pi	mm1,[rsi]
	cvttsd2si	eax,xmm1
	cvttsd2si	eax,[rsi]
	cvttss2si	eax,xmm1
	cvttss2si	eax,[rsi]
	divpd		xmm1,xmm2
	divpd		xmm1,[rsi]
	divps		xmm1,xmm2
	divps		xmm1,[rsi]
	divsd		xmm1,xmm2
	divsd		xmm1,[rsi]
	divss		xmm1,xmm2
	divss		xmm1,[rsi]
	extrq		xmm1,2,3
	extrq		xmm1,xmm2
	fxrstor		[rsi]
	fxsave		[rsi]
	haddpd		xmm1,xmm2
	haddpd		xmm1,[rsi]
	haddps		xmm1,xmm2
	haddps		xmm1,[rsi]
	hsubpd		xmm1,xmm2
	hsubpd		xmm1,[rsi]
	hsubps		xmm1,xmm2
	hsubps		xmm1,[rsi]
	insertq		xmm1,xmm2,3,4
	insertq		xmm1,xmm2
	lddqu		xmm1,[rsi]
	ldmxcsr		[rsi]
	maskmovdqu	xmm1,xmm2
	maxpd		xmm1,xmm2
	maxpd		xmm1,[rsi]
	maxps		xmm1,xmm2
	maxps		xmm1,[rsi]
	maxsd		xmm1,xmm2
	maxsd		xmm1,[rsi]
	maxss		xmm1,xmm2
	maxss		xmm1,[rsi]
	minpd		xmm1,xmm2
	minpd		xmm1,[rsi]
	minps		xmm1,xmm2
	minps		xmm1,[rsi]
	minsd		xmm1,xmm2
	minsd		xmm1,[rsi]
	minss		xmm1,xmm2
	minss		xmm1,[rsi]
	movapd		xmm1,xmm2
	movapd		xmm1,[rsi]
	movapd		[rsi],xmm1
	movaps		xmm1,xmm2
	movaps		xmm1,[rsi]
	movaps		[rsi],xmm1
	movd		xmm1,eax
	movd		xmm1,dword [rsi]
	movd		eax,xmm1
	movd		dword [rsi],xmm1
	movddup		xmm1,xmm2
	movddup		xmm1,[rsi]
	movdq2q		mm0,xmm1
	movdqa		xmm1,xmm2
	movdqa		xmm1,[rsi]
	movdqa		[rsi],xmm1
	movdqu		xmm1,xmm2
	movdqu		xmm1,[rsi]
	movdqu		[rsi],xmm1
	movhlps		xmm1,xmm2
	movhpd		xmm1,[rsi]
	movhpd		[rsi],xmm1
	movhps		xmm1,[rsi]
	movhps		[rsi],xmm1
	movlhps		xmm1,xmm2
	movlpd		xmm1,[rsi]
	movlpd		[rsi],xmm1
	movlps		xmm1,[rsi]
	movlps		[rsi],xmm1
	movmskpd	eax,xmm1
	movmskps	eax,xmm1
	movntdq		[rsi],xmm1
	movntpd		[rsi],xmm1
	movntps		[rsi],xmm1
	movntsd		[rsi],xmm1
	movntss		[rsi],xmm1
	movq		xmm1,xmm2
	movq		xmm1,[rsi]
	movq		[rsi],xmm1
	movq2dq		xmm1,mm2
	movsd		xmm1,xmm2
	movsd		xmm1,[rsi]
	movshdup	xmm1,xmm2
	movshdup	xmm1,[rsi]
	movsldup	xmm1,xmm2
	movsldup	xmm1,[rsi]
	movss		xmm1,xmm2
	movss		xmm1,[rsi]
	movss		[rsi],xmm1
	movupd		xmm1,xmm2
	movupd		xmm1,[rsi]
	movupd		[rsi],xmm1
	movups		xmm1,xmm2
	movups		xmm1,[rsi]
	movups		[rsi],xmm1
	mulpd		xmm1,xmm2
	mulpd		xmm1,[rsi]
	mulps		xmm1,xmm2
	mulps		xmm1,[rsi]
	mulsd		xmm1,xmm2
	mulsd		xmm1,[rsi]
	mulss		xmm1,xmm2
	mulss		xmm1,[rsi]
	orpd		xmm1,xmm2
	orpd		xmm1,[rsi]
	orps		xmm1,xmm2
	orps		xmm1,[rsi]
	packssdw	xmm1,xmm2
	packssdw	xmm1,[rsi]
	packsswb	xmm1,xmm2
	packsswb	xmm1,[rsi]
	packuswb	xmm1,xmm2
	packuswb	xmm1,[rsi]
	paddb		xmm1,xmm2
	paddb		xmm1,[rsi]
	paddd		xmm1,xmm2
	paddd		xmm1,[rsi]
	paddq		xmm1,xmm2
	paddq		xmm1,[rsi]
	paddsb		xmm1,xmm2
	paddsb		xmm1,[rsi]
	paddsw		xmm1,xmm2
	paddsw		xmm1,[rsi]
	paddusb		xmm1,xmm2
	paddusb		xmm1,[rsi]
	paddusw		xmm1,xmm2
	paddusw		xmm1,[rsi]
	paddw		xmm1,xmm2
	paddw		xmm1,[rsi]
	pand		xmm1,xmm2
	pand		xmm1,[rsi]
	pandn		xmm1,xmm2
	pandn		xmm1,[rsi]
	pavgb		xmm1,xmm2
	pavgb		xmm1,[rsi]
	pavgw		xmm1,xmm2
	pavgw		xmm1,[rsi]
	pcmpeqb		xmm1,xmm2
	pcmpeqb		xmm1,[rsi]
	pcmpeqd		xmm1,xmm2
	pcmpeqd		xmm1,[rsi]
	pcmpeqw		xmm1,xmm2
	pcmpeqw		xmm1,[rsi]
	pcmpgtb		xmm1,xmm2
	pcmpgtb		xmm1,[rsi]
	pcmpgtd		xmm1,xmm2
	pcmpgtd		xmm1,[rsi]
	pcmpgtw		xmm1,xmm2
	pcmpgtw		xmm1,[rsi]
	pextrw		eax,xmm1,2
	pinsrw		xmm1,eax,3
	pmaddwd		xmm1,xmm2
	pmaddwd		xmm1,[rsi]
	pmaxsw		xmm1,xmm2
	pmaxsw		xmm1,[rsi]
	pmaxub		xmm1,xmm2
	pmaxub		xmm1,[rsi]
	pminsw		xmm1,xmm2
	pminsw		xmm1,[rsi]
	pminub		xmm1,xmm2
	pminub		xmm1,[rsi]
	pmovmskb	eax,xmm1
	pmulhuw		xmm1,xmm2
	pmulhuw		xmm1,[rsi]
	pmulhw		xmm1,xmm2
	pmulhw		xmm1,[rsi]
	pmullw		xmm1,xmm2
	pmullw		xmm1,[rsi]
	pmuludq		xmm1,xmm2
	pmuludq		xmm1,[rsi]
	por		xmm1,xmm2
	por		xmm1,[rsi]
	psadbw		xmm1,xmm2
	psadbw		xmm1,[rsi]
	pshufd		xmm1,xmm2,3
	pshufd		xmm1,[rsi],3
	pshufhw		xmm1,xmm2,3
	pshufhw		xmm1,[rsi],3
	pshuflw		xmm1,xmm2,3
	pshuflw		xmm1,[rsi],3
	pslld		xmm1,xmm2
	pslld		xmm1,[rsi]
	pslld		xmm1,2
	pslldq		xmm1,2
	psllq		xmm1,xmm2
	psllq		xmm1,[rsi]
	psllq		xmm1,2
	psllw		xmm1,xmm2
	psllw		xmm1,[rsi]
	psllw		xmm1,2
	psrad		xmm1,xmm2
	psrad		xmm1,[rsi]
	psrad		xmm1,2
	psraw		xmm1,xmm2
	psraw		xmm1,[rsi]
	psraw		xmm1,2
	psrld		xmm1,xmm2
	psrld		xmm1,[rsi]
	psrld		xmm1,2
	psrldq		xmm1,2
	psrlq		xmm1,xmm2
	psrlq		xmm1,[rsi]
	psrlq		xmm1,2
	psrlw		xmm1,xmm2
	psrlw		xmm1,[rsi]
	psrlw		xmm1,2
	psubb		xmm1,xmm2
	psubb		xmm1,[rsi]
	psubd		xmm1,xmm2
	psubd		xmm1,[rsi]
	psubq		xmm1,xmm2
	psubq		xmm1,[rsi]
	psubsb		xmm1,xmm2
	psubsb		xmm1,[rsi]
	psubsw		xmm1,xmm2
	psubsw		xmm1,[rsi]
	psubusb		xmm1,xmm2
	psubusb		xmm1,[rsi]
	psubusw		xmm1,xmm2
	psubusw		xmm1,[rsi]
	psubw		xmm1,xmm2
	psubw		xmm1,[rsi]
	punpckhbw	xmm1,xmm2
	punpckhbw	xmm1,[rsi]
	punpckhdq	xmm1,xmm2
	punpckhdq	xmm1,[rsi]
	punpckhqdq	xmm1,xmm2
	punpckhqdq	xmm1,[rsi]
	punpckhwd	xmm1,xmm2
	punpckhwd	xmm1,[rsi]
	punpcklbw	xmm1,xmm2
	punpcklbw	xmm1,[rsi]
	punpckldq	xmm1,xmm2
	punpckldq	xmm1,[rsi]
	punpcklqdq	xmm1,xmm2
	punpcklqdq	xmm1,[rsi]
	punpcklwd	xmm1,xmm2
	punpcklwd	xmm1,[rsi]
	pxor		xmm1,xmm2
	pxor		xmm1,[rsi]
	rcpps		xmm1,xmm2
	rcpps		xmm1,[rsi]
	rcpss		xmm1,xmm2
	rcpss		xmm1,[rsi]
	rsqrtps		xmm1,xmm2
	rsqrtps		xmm1,[rsi]
	rsqrtss		xmm1,xmm2
	rsqrtss		xmm1,[rsi]
	shufpd		xmm1,xmm2,3
	shufpd		xmm1,[rsi],3
	shufps		xmm1,xmm2,3
	shufps		xmm1,[rsi],3
	sqrtpd		xmm1,xmm2
	sqrtpd		xmm1,[rsi]
	sqrtps		xmm1,xmm2
	sqrtps		xmm1,[rsi]
	sqrtsd		xmm1,xmm2
	sqrtsd		xmm1,[rsi]
	sqrtss		xmm1,xmm2
	sqrtss		xmm1,[rsi]
	stmxcsr		[rsi]
	subpd		xmm1,xmm2
	subpd		xmm1,[rsi]
	subps		xmm1,xmm2
	subps		xmm1,[rsi]
	subsd		xmm1,xmm2
	subsd		xmm1,[rsi]
	subss		xmm1,xmm2
	subss		xmm1,[rsi]
	ucomisd		xmm1,xmm2
	ucomisd		xmm1,[rsi]
	ucomiss		xmm1,xmm2
	ucomiss		xmm1,[rsi]
	unpckhpd	xmm1,xmm2
	unpckhpd	xmm1,[rsi]
	unpckhps	xmm1,xmm2
	unpckhps	xmm1,[rsi]
	unpcklpd	xmm1,xmm2
	unpcklpd	xmm1,[rsi]
	unpcklps	xmm1,xmm2
	unpcklps	xmm1,[rsi]
	xorpd		xmm1,xmm2
	xorpd		xmm1,[rsi]
	xorps		xmm1,xmm2
	xorps		xmm1,[rsi]
;===================VEX=================
	vfmaddpd	xmm1,xmm2,xmm3,xmm4
	vfmaddpd	xmm1,xmm2,[rsi],xmm4
	vfmaddpd	ymm1,ymm2,ymm3,ymm4
	vfmaddpd	ymm1,ymm2,[rsi],ymm4
	vfmaddps	xmm1,xmm2,xmm3,xmm4
	vfmaddps	xmm1,xmm2,[rsi],xmm4
	vfmaddps	ymm1,ymm2,ymm3,ymm4
	vfmaddps	ymm1,ymm2,[rsi],ymm4
	vfmaddsd	xmm1,xmm2,xmm3,xmm4
	vfmaddsd	xmm1,xmm2,[rsi],xmm4
	vfmaddss	xmm1,xmm2,xmm3,xmm4
	vfmaddss	xmm1,xmm2,[rsi],xmm4
	vfmaddsubpd	xmm1,xmm2,xmm3,xmm4
	vfmaddsubpd	xmm1,xmm2,[rsi],xmm4
	vfmaddsubpd	ymm1,ymm2,ymm3,ymm4
	vfmaddsubpd	ymm1,ymm2,[rsi],ymm4
	vfmaddsubps	xmm1,xmm2,xmm3,xmm4
	vfmaddsubps	xmm1,xmm2,[rsi],xmm4
	vfmaddsubps	ymm1,ymm2,ymm3,ymm4
	vfmaddsubps	ymm1,ymm2,[rsi],ymm4
	vfmsubaddpd	xmm1,xmm2,xmm3,xmm4
	vfmsubaddpd	xmm1,xmm2,[rsi],xmm4
	vfmsubaddpd	ymm1,ymm2,ymm3,ymm4
	vfmsubaddpd	ymm1,ymm2,[rsi],ymm4
	vfmsubaddps	xmm1,xmm2,xmm3,xmm4
	vfmsubaddps	xmm1,xmm2,[rsi],xmm4
	vfmsubaddps	ymm1,ymm2,ymm3,ymm4
	vfmsubaddps	ymm1,ymm2,[rsi],ymm4
	vfmsubpd	xmm1,xmm2,xmm3,xmm4
	vfmsubpd	xmm1,xmm2,[rsi],xmm4
	vfmsubpd	ymm1,ymm2,ymm3,ymm4
	vfmsubpd	ymm1,ymm2,[rsi],ymm4
	vfmsubps	xmm1,xmm2,xmm3,xmm4
	vfmsubps	xmm1,xmm2,[rsi],xmm4
	vfmsubps	ymm1,ymm2,ymm3,ymm4
	vfmsubps	ymm1,ymm2,[rsi],ymm4
	vfnmaddpd	xmm1,xmm2,xmm3,xmm4
	vfnmaddpd	xmm1,xmm2,[rsi],xmm4
	vfnmaddpd	ymm1,ymm2,ymm3,ymm4
	vfnmaddpd	ymm1,ymm2,[rsi],ymm4
	vfnmsubpd	xmm1,xmm2,xmm3,xmm4
	vfnmsubpd	xmm1,xmm2,[rsi],xmm4
	vfnmsubpd	ymm1,ymm2,ymm3,ymm4
	vfnmsubpd	ymm1,ymm2,[rsi],ymm4
	vfrczpd		xmm1,xmm2
	vfrczpd		xmm1,[rsi]
	vfrczpd		ymm1,ymm2
	vfrczpd		ymm1,[rsi]
	vfrczps		xmm1,xmm2
	vfrczps		xmm1,[rsi]
	vfrczps		ymm1,ymm2
	vfrczps		ymm1,[rsi]
	vfrczsd		xmm1,xmm2
	vfrczsd		xmm1,[rsi]
	vfrczss		xmm1,xmm2
	vfrczss		xmm1,[rsi]
	vpcmov		xmm1,xmm2,xmm3,xmm4
	vpcmov		xmm1,xmm2,[rsi],xmm4
	vpcmov		ymm1,ymm2,ymm3,ymm4
	vpcmov		ymm1,ymm2,[rsi],ymm4
	vpcomb		xmm1,xmm2,xmm3,4
	vpcomb		xmm1,xmm2,[rsi],4
	vpcomd		xmm1,xmm2,xmm3,4
	vpcomd		xmm1,xmm2,[rsi],4
	vpcomw		xmm1,xmm2,xmm3,4
	vpcomw		xmm1,xmm2,[rsi],4
	vpcomq		xmm1,xmm2,xmm3,4
	vpcomq		xmm1,xmm2,[rsi],4
	vpcomub		xmm1,xmm2,xmm3,4
	vpcomub		xmm1,xmm2,[rsi],4
	vpcomuw		xmm1,xmm2,xmm3,4
	vpcomuw		xmm1,xmm2,[rsi],4
	vpcomud		xmm1,xmm2,xmm3,4
	vpcomud		xmm1,xmm2,[rsi],4
	vpcomuq		xmm1,xmm2,xmm3,4
	vpcomuq		xmm1,xmm2,[rsi],4
	vphaddbw	xmm1,xmm2
	vphaddbw	xmm1,[rsi]
	vphaddbd	xmm1,xmm2
	vphaddbd	xmm1,[rsi]
	vphaddbq	xmm1,xmm2
	vphaddbq	xmm1,[rsi]
	vphadddq	xmm1,xmm2
	vphadddq	xmm1,[rsi]
	vphaddudq	xmm1,xmm2
	vphaddudq	xmm1,[rsi]
	vphadduwd	xmm1,xmm2
	vphadduwd	xmm1,[rsi]
	vphadduwq	xmm1,xmm2
	vphadduwq	xmm1,[rsi]
	vphaddwd	xmm1,xmm2
	vphaddwd	xmm1,[rsi]
	vphaddwq	xmm1,xmm2
	vphaddwq	xmm1,[rsi]
	vphsubbw	xmm1,xmm2
	vphsubbw	xmm1,[rsi]
	vphsubwd	xmm1,xmm2
	vphsubwd	xmm1,[rsi]
	vphsubdq	xmm1,xmm2
	vphsubdq	xmm1,[rsi]
	vpmacsdd	xmm1,xmm2,xmm3,xmm4
	vpmacsdd	xmm1,xmm2,[rsi],xmm4
;================================================================================
; TODO: Either AMD mis-documented these, or NASM has the opcodes backwards, argh!
;	vpmacsdql	xmm1,xmm2,xmm3,xmm4
;	vpmacsdql	xmm1,xmm2,[rsi],xmm4
;	vpmacsdqh	xmm1,xmm2,xmm3,xmm4
;	vpmacsdqh	xmm1,xmm2,[rsi],xmm4
;================================================================================
	vpmacssdd	xmm1,xmm2,xmm3,xmm4
	vpmacssdd	xmm1,xmm2,[rsi],xmm4
;================================================================================
; TODO: Either AMD mis-documented these, or NASM has the opcodes backwards, argh!
;	vpmacssdqh	xmm1,xmm2,xmm3,xmm4
;	vpmacssdqh	xmm1,xmm2,[rsi],xmm4
;	vpmacssdql	xmm1,xmm2,xmm3,xmm4
;	vpmacssdql	xmm1,xmm2,[rsi],xmm4
;================================================================================
	vpmacsswd	xmm1,xmm2,xmm3,xmm4
	vpmacsswd	xmm1,xmm2,[rsi],xmm4
	vpmacssww	xmm1,xmm2,xmm3,xmm4
	vpmacssww	xmm1,xmm2,[rsi],xmm4
	vpmacswd	xmm1,xmm2,xmm3,xmm4
	vpmacswd	xmm1,xmm2,[rsi],xmm4
	vpmacsww	xmm1,xmm2,xmm3,xmm4
	vpmacsww	xmm1,xmm2,[rsi],xmm4
	vpmadcsswd	xmm1,xmm2,xmm3,xmm4
	vpmadcsswd	xmm1,xmm2,[rsi],xmm4
	vpmadcswd	xmm1,xmm2,xmm3,xmm4
	vpmadcswd	xmm1,xmm2,[rsi],xmm4
	vpperm		xmm1,xmm2,xmm3,xmm4
	vpperm		xmm1,xmm2,[rsi],xmm4
	vpperm		xmm1,xmm2,xmm3,[rsi]
	vprotb		xmm1,xmm2,xmm3
	vprotb		xmm1,[rsi],xmm3
	vprotb		xmm1,xmm2,[rsi]
	vprotb		xmm1,xmm2,3
	vprotw		xmm1,xmm2,xmm3
	vprotw		xmm1,[rsi],xmm3
	vprotw		xmm1,xmm2,[rsi]
	vprotw		xmm1,xmm2,3
	vprotd		xmm1,xmm2,xmm3
	vprotd		xmm1,[rsi],xmm3
	vprotd		xmm1,xmm2,[rsi]
	vprotd		xmm1,xmm2,3
	vprotq		xmm1,xmm2,xmm3
	vprotq		xmm1,[rsi],xmm3
	vprotq		xmm1,xmm2,[rsi]
	vprotq		xmm1,xmm2,3
	vpshab		xmm1,xmm2,xmm3
	vpshab		xmm1,[rsi],xmm3
	vpshab		xmm1,xmm2,[rsi]
	vpshaw		xmm1,xmm2,xmm3
	vpshaw		xmm1,[rsi],xmm3
	vpshaw		xmm1,xmm2,[rsi]
	vpshad		xmm1,xmm2,xmm3
	vpshad		xmm1,[rsi],xmm3
	vpshad		xmm1,xmm2,[rsi]
	vpshaq		xmm1,xmm2,xmm3
	vpshaq		xmm1,[rsi],xmm3
	vpshaq		xmm1,xmm2,[rsi]
	vpshlb		xmm1,xmm2,xmm3
	vpshlb		xmm1,[rsi],xmm3
	vpshlb		xmm1,xmm2,[rsi]
	vpshlw		xmm1,xmm2,xmm3
	vpshlw		xmm1,[rsi],xmm3
	vpshlw		xmm1,xmm2,[rsi]
	vpshld		xmm1,xmm2,xmm3
	vpshld		xmm1,[rsi],xmm3
	vpshld		xmm1,xmm2,[rsi]
	vpshlq		xmm1,xmm2,xmm3
	vpshlq		xmm1,[rsi],xmm3
	vpshlq		xmm1,xmm2,[rsi]
;===================MORE=================
	blendpd		xmm1,xmm2,1
	blendps		xmm1,xmm2,1
	blendvpd	xmm1,xmm2,xmm0
	blendvps	xmm1,xmm2,xmm0
	crc32		eax,al
	crc32		eax,ax
	crc32		eax,eax
	crc32		eax,byte [rsi]
	crc32		eax,word [rsi]
	crc32		eax,dword [rsi]
	dppd		xmm1,xmm2,3
	dpps		xmm1,xmm2,3
	extractps	eax,xmm2,3
	insertps	xmm1,xmm2,3
	movntdqa	xmm1,[rsi]
	mpsadbw		xmm1,xmm2,3
	mpsadbw		xmm1,[rsi],3
	pabsb		mm1,mm2
	pabsb		xmm1,xmm2
	pabsw		mm1,mm2
	pabsw		xmm1,xmm2
	pabsd		mm1,mm2
	pabsd		xmm1,xmm2
	packusdw	xmm1,xmm2
	palignr		mm1,mm2,3
	palignr		mm1,[rsi],3
	palignr		xmm1,xmm2,3
	palignr		xmm1,[rsi],3
	pblendvb	xmm1,xmm2,xmm0
	pblendvb	xmm1,[rsi],xmm0
	pblendw		xmm1,xmm2,3
	pblendw		xmm1,[rsi],3
	pcmpestri	xmm1,xmm2,3
	pcmpestri	xmm1,[rsi],3
	pcmpestrm	xmm1,xmm2,3
	pcmpestrm	xmm1,[rsi],3
	pcmpistrm	xmm1,xmm2,3
	pcmpistrm	xmm1,[rsi],3
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
	swapgs
	xgetbv
	xlat
	xorpd		xmm1,xmm2
	xorps		xmm1,xmm2
	xrstor		[rsi]
	lfence
	xsave		[rsi]
	xsetbv
	invept		rax,[rsi]
	invvpid		rax,[rsi]
	vmcall
	vmclear		[rsi]
	vmlaunch
	vmresume
	vmptrld		[rsi]
	vmptrst		[rsi]
	vmread		rax,rax
	vmread		[rsi],rax
	vmwrite		rax,rax
	vmwrite		rax,[rsi]
	vmxoff
	vmxon		[rsi]
	getsec

