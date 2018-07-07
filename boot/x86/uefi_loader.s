[bits 64]
[org 0x180000]
[section .text]

; 0x107000          Graphics info
; 0x107FE8	    RSDP address
; 0x107FF0	    Installation ID
; 0x140000-0x150000 Identity paging tables
; 0x160000-0x170000 Memory regions
; 0x180000-0x1C0000 Loader (this)
; 0x1C0000-0x1E0000 Kernel paging tables
; 0x1F0000-0x200000 Stack
; 0x200000-0x300000 Kernel

%define memory_map 0x160000
%define kernel_buffer 0x200000

start:
	; Setup the environment.
	lgdt	[gdt_data.gdt]
	mov	rax,0x140000
	mov	cr3,rax
	mov	rax,0x200000
	mov	rsp,rax
	mov	rax,0x50
	mov	ds,rax
	mov	es,rax
	mov	ss,rax

	; Find the program headers
	; RAX = ELF header, RBX = program headers
	mov	rax,kernel_buffer
	mov	rbx,[rax + 32]
	add	rbx,rax

	; ECX = entries, EDX = size of entry
	movzx	rcx,word [rax + 56]
	movzx	rdx,word [rax + 54]

	; Loop through each program header
	.loop_program_headers:
	push	rax
	push	rcx

	; Only deal with load segments
	mov	eax,[rbx]
	cmp	eax,1
	jne	.next_entry

	; Clear the memory
	mov	rcx,[rbx + 40]
	xor	rax,rax
	mov	rdi,[rbx + 16]
	rep	stosb

	; Copy the memory
	mov	rcx,[rbx + 32]
	mov	rsi,[rbx + 8]
	add	rsi,kernel_buffer
	mov	rdi,[rbx + 16]
	rep	movsb

	; Go to the next entry
	.next_entry:
	pop	rcx
	pop	rax

	add	rbx,rdx
	dec	rcx
	or	rcx,rcx
	jnz	.loop_program_headers

	jmp	run_kernel64

run_kernel64:
	; Get the start address of the kernel
	mov	rbx,kernel_buffer
	mov	rcx,[rbx + 24]

	; Map the first MB at 0xFFFFFF0000000000 --> 0xFFFFFF0000100000 
	mov	rax,[0xFFFFFFFFFFFFF000]
	mov	[0xFFFFFFFFFFFFFFF0],rax
	mov	rax,cr3
	mov	cr3,rax

	; Use the new linear address of the GDT
	mov	rax,0xFFFFFF0000000000
	add	qword [gdt_data.gdt2],rax
	lgdt	[gdt_data.gdt]

	call 	set_cs

	; Execute the kernel's _start function
	mov	rdi,0x100000
	jmp	rcx

set_cs:
	pop	rax
	push	0x48
	push	rax
	db	0x48, 0xCB

gdt_data:
	.null_entry:	dq 0
	.code_entry:	dd 0xFFFF	; 0x08
			db 0
			dw 0xCF9A
			db 0
	.data_entry:	dd 0xFFFF	; 0x10
			db 0
			dw 0xCF92
			db 0
	.code_entry_16:	dd 0xFFFF	; 0x18
			db 0
			dw 0x0F9A
			db 0
	.data_entry_16:	dd 0xFFFF	; 0x20
			db 0
			dw 0x0F92
			db 0
	.user_code:	dd 0xFFFF	; 0x2B
			db 0
			dw 0xCFFA
			db 0
	.user_data:	dd 0xFFFF	; 0x33
			db 0
			dw 0xCFF2
			db 0
	.tss:		dd 0x68		; 0x38
			db 0
			dw 0xE9
			db 0
			dq 0
	.code_entry64:	dd 0xFFFF	; 0x48
			db 0
			dw 0xAF9A
			db 0
	.data_entry64:	dd 0xFFFF	; 0x50
			db 0
			dw 0xAF92
			db 0
	.user_code64:	dd 0xFFFF	; 0x5B
			db 0
			dw 0xAFFA
			db 0
	.user_data64:	dd 0xFFFF	; 0x63
			db 0
			dw 0xAFF2
			db 0
	.user_code64c:	dd 0xFFFF	; 0x6B
			db 0
			dw 0xAFFA
			db 0
	.gdt:		dw (gdt_data.gdt - gdt_data - 1)
	.gdt2:		dq gdt_data
