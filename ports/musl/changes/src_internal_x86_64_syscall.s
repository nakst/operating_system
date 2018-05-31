.global __syscall
.hidden __syscall
.type __syscall,@function
__syscall:
	mov $OSMakeLinuxSystemCall,%rax
	call *%rax                 
	ret
