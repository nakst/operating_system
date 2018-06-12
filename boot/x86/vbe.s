vbe_init:
	mov	ax,vesa_info >> 4
	mov	es,ax
	xor	di,di

	; Get SVGA information.
	mov	ax,0x4F00
	int	0x10
	cmp	ax,0x4F
	jne 	vbe_bad

	; Load the list of available modes.
	add	di,0x200
	mov	eax,[es:14]
	cmp	eax,0
	je	.find_done
	mov	ax,[es:16]
	mov	fs,ax
	mov	si,[es:14]
	xor	cx,cx
	.find_loop:
	mov	ax,[fs:si]
	cmp	ax,0xFFFF
	je	.find_done
	mov	[es:di],ax
	add	di,2
	add	si,2
	jmp	.find_loop
	.find_done:

	; Add standard modes (if necessary).
	mov	word [es:di],0xFFFF
	cmp	di,0x200
	jne	.added_modes
	mov	word [es:di + 0],257
	mov	word [es:di + 2],259
	mov	word [es:di + 4],261
	mov	word [es:di + 6],263
	mov	word [es:di + 8],273
	mov	word [es:di + 10],276
	mov	word [es:di + 12],279
	mov	word [es:di + 14],282
	mov	word [es:di + 16],274
	mov	word [es:di + 18],277
	mov	word [es:di + 20],280
	mov	word [es:di + 22],283
	mov	word [es:di + 24],0xFFFF
	.added_modes:

	; Check which of these modes can be used.
	mov	si,0x200
	mov	di,0x200
	.check_loop:
	mov	cx,[es:si]
	mov	[es:di],cx
	cmp	cx,0xFFFF
	je	.check_done
	push	di
	push	si
	mov	ax,0x4F01
	xor	di,di
	or	cx,(1 << 14)
	int	0x10
	pop	si
	pop	di
	add	si,2
	cmp	ax,0x4F			; Interrupt failed.
	jne	.check_loop
	cmp	byte [es:0x19],24	; We only support 24-bit modes currently.
	jne	.check_loop
	cmp	word [es:0x14],480	; We support a minimum vertical resolution of 480 pixels.
	jl	.check_loop
	add	di,2
	jmp	.check_loop
	.check_done:

	; Print a list of the available modes.
	mov	si,vbe_s_select_video_mode
	call	vbe_print_string
	mov	bx,0x200
	mov	cx,1
	.print_loop:
	mov	dx,[es:bx]
	cmp	dx,0xFFFF
	je	.print_done
	xor	di,di
	push	cx
	mov	ax,0x4F01
	mov	cx,dx
	or	cx,(1 << 14)
	int	0x10
	pop	cx
	mov	si,vbe_s_left_bracket
	call	vbe_print_string
	mov	ax,cx
	call	vbe_print_decimal
	mov	si,vbe_s_right_bracket
	call	vbe_print_string
	mov	ax,[es:0x12]
	call	vbe_print_decimal
	mov	si,vbe_s_by
	call	vbe_print_string
	mov	ax,[es:0x14]
	call	vbe_print_decimal
	call	vbe_print_newline
	inc	cx
	add	bx,2
	jmp	.print_loop
	.print_done:
	
	; Let the user select a mode.
	mov	dx,cx
	dec	dx
	xor	cx,cx
	.select_loop:
	cmp	cx,dx
	jb	.c1
	mov	cx,0
	.c1:
	call	vbe_set_highlighted_line
	xor	ax,ax
	int	0x16
	shr	ax,8
	cmp	ax,72
	jne	.k11
	dec	cx
	.k11:
	cmp	ax,80
	jne	.k12
	inc	cx
	.k12:
	cmp	ax,28
	jne	.select_loop

	; Set the graphics mode.
	mov	di,cx
	shl	di,1
	add	di,0x200
	mov	bx,[es:di]
	or	bx,(1 << 14)
	mov	cx,bx
	mov	ax,0x4F02
	int	0x10
	cmp	ax,0x4F
	jne	vbe_failed
	mov	ax,0x4F01
	xor	di,di
	int	0x10
	ret

vbe_bad:
	mov	word [es:di + 0x12],0
	ret

vbe_failed:
	mov	si,vbe_s_failed
	call	vbe_print_string
	jmp	vbe_init.select_loop

vbe_print_newline:
	pusha
	mov	ah,0xE
	mov	al,13
	int	0x10
	mov	ah,0xE
	mov	al,10
	int	0x10
	popa
	ret

vbe_print_space:
	pusha
	mov	ah,0xE
	mov	al,' '
	int	0x10
	popa
	ret

vbe_print_string: ; Input - SI.
	pusha
	.loop:
	lodsb
	or	al,al
	jz	.done
	mov	ah,0xE
	int	0x10
	jmp	.loop
	.done:
	popa
	ret

vbe_print_decimal: ; Input - AX.
	pusha
	mov	bx,.buffer
	mov	cx,10
	.next:
	xor	dx,dx
	div	cx
	add	dx,'0'
	mov	[bx],dl
	inc	bx
	cmp	ax,0
	jne	.next
	.loop:
	dec	bx
	mov	al,[bx]
	mov	ah,0xE
	int	0x10
	cmp	bx,.buffer
	jne	.loop
	popa
	ret
	.buffer: db 0, 0, 0, 0, 0

vbe_set_highlighted_line: ; Input - CX
	pusha
	mov	ax,0xB800
	mov	fs,ax
	mov	di,1
	mov	dx,(80 * 25)
	.clear_loop:
	mov	byte [fs:di],0x07
	add	di,2
	dec	dx
	cmp	dx,0
	jne	.clear_loop
	mov	ax,cx
	add	ax,2
	mov	cx,160
	mul	cx
	mov	dx,80
	mov	di,ax
	inc	di
	.highlight_loop:
	mov	byte [fs:di],0x70
	add	di,2
	dec	dx
	cmp	dx,0
	jne	.highlight_loop
	popa
	ret

vbe_s_select_video_mode: db 'Select a video mode: [use up/down then press enter]',13,10,0
vbe_s_left_bracket: db '(',0
vbe_s_right_bracket: db ') ',0
vbe_s_by: db 'x',0
vbe_s_failed: db 'This graphics mode could not be selected. Please try a different one.',13,10,0
