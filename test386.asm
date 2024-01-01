; 386-specific
org 0

	cpu	all

_start:
	mov	tr0,eax
	mov	eax,tr0
	mov	tr1,eax
	mov	eax,tr1

	cpu	386

	ibts	ax,bx
	ibts	[si],bx
	ibts	eax,ebx
	ibts	[esi],ecx
	xbts	ax,bx
	xbts	bx,[si]
	xbts	eax,ebx
	xbts	ecx,[esi]
	smi
	umov	al,al
	umov	[si],al
	umov	ax,ax
	umov	[si],ax
	umov	eax,eax
	umov	[si],eax
	umov	al,[si]
	umov	ax,[si]
	umov	eax,[si]

; 486-specific
        nop
        nop
        nop
        nop
        nop
        nop

	cpu	all

; NTS: The 486a core is NOT expected to decode this, because 486 Step A-B0 used an alternate opcode
    cmpxchg	al,ah				; 0F B0 /r           CMPXCHG
    cmpxchg	[bx+si],cx			; 0F B0 /r           CMPXCHG
    cmpxchg     [ebx],esi			; 0F B0 /r           CMPXCHG

        nop
        nop
        nop
        nop
        nop
        nop

	cpu	486

; NTS: Nobody except the 486a core is expected to decode this, only 486 step A-B0 has this opcode
 cmpxchg486	al,ah				; 0F A6 /r           CMPXCHG [early 486s]
 cmpxchg486	[bx+si],cx			; 0F A6 /r           CMPXCHG [early 486s]
 cmpxchg486	[ebx],esi			; 0F A6 /r           CMPXCHG [early 486s]

	cpu	all

        nop
        nop
        nop
        nop
        nop
        nop

; 286-specific

        nop
        nop
         db	0xDB,0xE5			; undocumented FRSTPM (i287)
        nop
        nop

; x86-64

	cpu	all

; this is here for completeness, we're just abusing NASM's trust to get it in here so we can test decoding it.
; the instruction is not valid in 16- and 32-bit modes, so we have to tell NASM we're assembling 64-bit here.
bits 64
     swapgs					; 0F 01 /7
%ifdef B32
bits 32
%else
bits 16
%endif

; Itanium

	db	0x0F,0xB8,0x01,0x00		; JMPX rel16
	nop
	nop
	nop
	db	0x67,0x0F,0xB8,0x01,0x00,0x00,0x00 ; JMPX rel32
	nop
	nop
	db	0x0F,0x00,0x30			; JMPX /6
	nop
	nop

; other

	wait					; 9B
	nop

	fwait					; 9B
	nop

