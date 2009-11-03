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

	nop

