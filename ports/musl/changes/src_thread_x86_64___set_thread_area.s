/* Copyright 2011-2012 Nicholas J. Kain, licensed under standard MIT license */
.text
.global __set_thread_area
.type __set_thread_area,@function

__set_thread_area:
#nakst - redirect to OSMakeLinuxSystemCall
	mov %rdi,%rsi           /* shift for syscall */
	movl $0x1002,%edx       /* SET_FS register */
	movl $158,%edi          /* set fs segment to */
	mov $OSMakeLinuxSystemCall,%rax
	call *%rax                 /* arch_prctl(SET_FS, arg)*/
	ret
