	nop
.global __restore_rt
.type __restore_rt,@function
__restore_rt:
	mov $15, %rax
	nop #nakst - we don't support signals
.size __restore_rt,.-__restore_rt
