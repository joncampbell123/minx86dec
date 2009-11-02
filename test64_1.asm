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
	nop

