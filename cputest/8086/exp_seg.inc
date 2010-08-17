;==========================================================
; test8086_exp_seg
; 
; Test whether the CPU does explicit segment references
; correctly (segment override prefixes)
;
; ASSUME CALLER SETS CS == DS == ES == SS

;Start=====================================================
test8086_exp_seg:
	cli
	mov	[cs:s_sp],sp

;CS:=======================================================
test8086_exp_seg_cs:
	mov	si,s_test8086_exp_seg
	call	puts
	mov	si,s_test8086_exp_seg_cs
	call	puts

	mov	ax,cs			; ensure all other segment values are WRONG
	add	ax,0x1234
	mov	ds,ax
	add	ax,0x1234
	mov	es,ax
	add	ax,0x1234
	mov	ss,ax
	mov	si,s_testval
	mov	ax,[cs:si]
	mov	bx,0x1234
	mov	cx,cs
	mov	ds,cx
	mov	es,cx
	mov	ss,cx

	mov	si,s_pass
	cmp	ax,bx
	jz	.pass
	mov	si,s_fail
.pass:	call	puts
	mov	si,s_crlf
	call	puts

;DS:=======================================================
test8086_exp_seg_ds:
	mov	si,s_test8086_exp_seg
	call	puts
	mov	si,s_test8086_exp_seg_ds
	call	puts

	mov	ax,cs			; ensure all other segment values are WRONG
	add	ax,0x1234
	mov	es,ax
	add	ax,0x1234
	mov	ss,ax
	mov	si,s_testval
	mov	ax,[ds:si]
	mov	bx,0x1234
	mov	cx,cs
	mov	ds,cx
	mov	es,cx
	mov	ss,cx

	mov	si,s_pass
	cmp	ax,bx
	jz	.pass
	mov	si,s_fail
.pass:	call	puts
	mov	si,s_crlf
	call	puts

;ES:=======================================================
test8086_exp_seg_es:
	mov	si,s_test8086_exp_seg
	call	puts
	mov	si,s_test8086_exp_seg_es
	call	puts

	mov	ax,cs			; ensure all other segment values are WRONG
	mov	es,ax
	add	ax,0x1234
	mov	ds,ax
	add	ax,0x1234
	mov	ss,ax
	mov	si,s_testval
	mov	ax,[es:si]
	mov	bx,0x1234
	mov	cx,cs
	mov	ds,cx
	mov	es,cx
	mov	ss,cx

	mov	si,s_pass
	cmp	ax,bx
	jz	.pass
	mov	si,s_fail
.pass:	call	puts
	mov	si,s_crlf
	call	puts

;SS:=======================================================
test8086_exp_seg_ss:
	mov	si,s_test8086_exp_seg
	call	puts
	mov	si,s_test8086_exp_seg_ss
	call	puts

	mov	ax,cs			; ensure all other segment values are WRONG
	mov	ss,ax
	add	ax,0x1234
	mov	ds,ax
	add	ax,0x1234
	mov	es,ax
	mov	si,s_testval
	mov	ax,[ss:si]
	mov	bx,0x1234
	mov	cx,cs
	mov	ds,cx
	mov	es,cx
	mov	ss,cx

	mov	si,s_pass
	cmp	ax,bx
	jz	.pass
	mov	si,s_fail
.pass:	call	puts
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

s_test8086_exp_seg_cs:
	db		'CS override...........',0
s_test8086_exp_seg_ds:
	db		'DS override...........',0
s_test8086_exp_seg_es:
	db		'ES override...........',0
s_test8086_exp_seg_ss:
	db		'SS override...........',0
s_test8086_exp_seg:
	db		'8086 explicit segment: ',0

