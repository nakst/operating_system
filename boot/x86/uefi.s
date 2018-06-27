[bits 64]

[global GetCR3]
GetCR3:
	mov	rax,cr3
	and	rax,~0xFFF
	ret

[global LoadGDT]
LoadGDT:
	cli
	lgdt	[rdi]
	mov	ax,0x50
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	mov	ss,ax
	pop	rax
	push	0x48
	push	rax
	db	0x48
	db	0xCB
