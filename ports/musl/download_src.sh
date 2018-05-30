if [ ! -d "ports/musl/src" ]; then
	echo "    Downloading source for Musl..."
	curl https://www.musl-libc.org/releases/musl-1.1.19.tar.gz > musl-1.1.19.tar.gz
	gunzip musl-1.1.19.tar.gz
	tar -xf musl-1.1.19.tar
	cp -r musl-1.1.19/* ports/musl
	cp ports/musl/changes/arch_x86_64_pthread_arch.h		ports/musl/arch/x86_64/pthread_arch.h           		
	cp ports/musl/changes/arch_x86_64_syscall_arch.h                ports/musl/arch/x86_64/syscall_arch.h
	cp ports/musl/changes/src_env___libc_start_main.c               ports/musl/src/env/__libc_start_main.c
	cp ports/musl/changes/config.mak                                ports/musl/config.mak
	cp ports/musl/changes/dist_config.mak                           ports/musl/dist/config.mak
	cp ports/musl/changes/src_ldso_x86_64_tlsdesc.s                 ports/musl/src/ldso/x86_64/tlsdesc.s
	cp ports/musl/changes/src_process_x86_64_vfork.s                ports/musl/src/process/x86_64/vfork.s
	cp ports/musl/changes/src_signal_x86_64_restore.s               ports/musl/src/signal/x86_64/restore.s
	cp ports/musl/changes/src_thread_x86_64___set_thread_area.s     ports/musl/src/thread/x86_64/__set_thread_area.s
	cp ports/musl/changes/src_thread_x86_64___unmapself.s           ports/musl/src/thread/x86_64/__unmapself.s
	cp ports/musl/changes/src_thread_x86_64_clone.s                 ports/musl/src/thread/x86_64/clone.s
	cp ports/musl/changes/src_thread_x86_64_syscall_cp.s            ports/musl/src/thread/x86_64/syscall_cp.s
	rm -r musl-1.1.19 
	rm musl-1.1.19.tar
fi

