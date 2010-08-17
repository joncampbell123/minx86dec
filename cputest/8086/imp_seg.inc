;==========================================================
; test8086_imp_seg
; 
; Test whether the CPU does "implied" segment references
; correctly (DS for memory references, but SS for [BP] or
; [SP] etc)
;
; ASSUME CALLER SETS CS == DS == ES == SS

;Start=====================================================
test8086_imp_seg:
	cli
	mov	[cs:s_sp],sp

; If I use [si], [di], etc. does it default to the data seg?
; This is kind of superfluous since if that were not the case
; this program and DOS would have crashed a bit far back, but
; I still want to make sure:
def_ds:	push	cs
	pop	ds
	mov	si,s_test8086_imp_seg
	call	puts
	mov	si,s_test8086_imp_seg_def_ds
	call	puts
	mov	si,s_testval
	mov	ax,[si]			; implied DS, right?
	mov	bx,[cs:si]		; CS = DS, for check
	mov	si,s_pass
	cmp	ax,bx
	jz	.pass
	mov	si,s_fail
.pass:	call	puts
	mov	si,s_crlf
	call	puts

; If I use [bp], [bp+si], or [bp+di], then the CPU will def
; to using SS as base, right? (early emulators I wrote missed
; this fact apparently, allowing me to run some DOS games
; but nothing beyond that).
;
; This is the reason interrupts were cleared above: we're
; going to mess with the stack pointer and register!
def_ss:	push	cs
	pop	ds
	mov	si,s_test8086_imp_seg
	call	puts
	mov	si,s_test8086_imp_seg_stack_bp
	call	puts
; advance SS forward by 100h (10h pages)
	mov	ax,ss
	add	ax,10h
	mov	ss,ax
	mov	bp,s_testval-0x100
; OK, test
	mov	ax,[cs:s_testval]	; ref
	mov	bx,[bp]			; should ref as if SS:[BP] right?
	cmp	ax,bx
	jnz	.fail

	sub	bp,0x10
	mov	si,0x10
	mov	bx,[bp+si]
	cmp	ax,bx
	jnz	.fail

	sub	bp,0x10
	mov	di,0x20
	mov	bx,[bp+di]
	cmp	ax,bx
	jnz	.fail

	add	di,bp			; CHECK: if we just use [di], ref DS
	mov	bx,[di]			; we WON'T get the value, right?
	cmp	ax,bx
	jz	.fail

	mov	si,s_pass
	jmp	.pass
.fail:	mov	si,s_fail	
.pass:	cli
	mov	ax,cs
	mov	ds,ax
	mov	ss,ax
	mov	sp,[cs:s_sp]

	call	puts
	mov	si,s_crlf
	call	puts

;Finish====================================================
	cli
	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	sp,[cs:s_sp]
	sti
	ret

s_test8086_imp_seg_def_ds:
	db		'defaults to DS.........',0
s_test8086_imp_seg_stack_bp:
	db		'[BP] implies SS........',0
s_test8086_imp_seg:
	db		'8086 implied segment: ',0
s_testval:
	dw		0x1234
s_sp:
	dw		0

