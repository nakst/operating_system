.global __vfork
.weak vfork
.type __vfork,@function
.type vfork,@function
__vfork:
vfork:
	pop %rdx
	mov $42,%rdi
	mov $1,%rsi
	syscall#nakst - We don't support forking.
	push %rdx
	mov %rax,%rdi
	jmp __syscall_ret
