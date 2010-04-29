; x86-64 test assembly language
bits 64
	a32	movsb

	movsb
	movsw
	movsd
	movsq

	cmpsb
	cmpsw
	cmpsd
	cmpsq

	xchg	bl,dl
	xchg	[esi],cl
	xchg	[edi],si

	mov	al,12h
	mov	bh,34h
	mov	dh,34h
	mov	sil,23h
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
	add	r13w,[rel 0x100]
	add	rbx,[rel 0x100000]
	add	rbx,[rel 0x80000000]
	add	rbx,[rel 0x90000000]	; OOPS! NASM will happily encode this, even though it now makes a negative number!
	add	rbx,[rel 0x100000000]	; NASM will happily let this overflow, giving a relative addr of 0
	lea	rsi,[rel 0]
	lea	esi,[rel 0]
	lea	si,[rel 0]
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
	mov	ax,es
	mov	ds,ax
	mov	bx,cs
	mov	cs,bx
	mov	cx,ds
	mov	dx,ss
	mov	[edi],es
	mov	[esi],ds
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

