; x86-64 test assembly language
bits 64
	add	bl,[esi]
	add	bh,[edi]
	add	bx,[esi]
	add	cx,[edi]
	add	al,12h
	add	eax,012345678h
	add	eax,0FFEAAAAAh
	add	rax,012345678h
	mov	rax,00123456789ABCDEFh
	nop

