; 386-specific
org 0

_start:
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

