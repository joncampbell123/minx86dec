; x86-64 test assembly language
bits 64
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
	add	bl,[rel 1]
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

	lea	rsi,[r8*8+r9+22224444h]
	lea	r15,[r15*2+r14-7ffffffh]

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

	nop

