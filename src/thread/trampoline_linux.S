    .section .text
    .globl vl_thread_trampoline
    .type  vl_thread_trampoline,@function
# On entry:
#   RDI = ThreadBlock*
#   RSP = child stack (aligned by parent)
vl_thread_trampoline:
    xor   %rbp, %rbp
    mov   %rdi, %r13            # save ThreadBlock* across syscalls
    and   $-16, %rsp            # keep stack aligned (pre-call %rsp % 16 == 0)

    # --- TLS for child: map 1 page, self-pointer at [tcb], ARCH_SET_FS
    mov   $9, %rax              # SYS_mmap
    xor   %rdi, %rdi            # addr = NULL
    mov   $4096, %rsi           # len
    mov   $3, %rdx              # PROT_READ|PROT_WRITE
    mov   $0x22, %r10           # MAP_PRIVATE|MAP_ANONYMOUS
    mov   $-1, %r8              # fd = -1
    xor   %r9, %r9              # off = 0
    syscall

    mov   $-4095, %r11
    cmp   %r11, %rax
    jae   1f                    # mmap failed → run without TLS

    mov   %rax, %r12            # r12 = tcb
    mov   %r12, (%r12)          # tcb->self = tcb
    mov   $158, %rax            # SYS_arch_prctl
    mov   $0x1002, %rdi         # ARCH_SET_FS
    mov   %r12, %rsi            # FS base = tcb
    syscall

1:
    # Restore ThreadBlock* and run C entry
    mov   %r13, %rdi
    call  vl_thread_entry       # long vl_thread_entry(ThreadBlock*)

    # Run per-thread TLS destructors before exiting the thread
    call  __vl_tls_run_dtors    # <-- correct symbol name (plural)

    # exit this thread
    xor   %rdi, %rdi            # status = 0
    mov   $60, %rax             # SYS_exit
    syscall
    ud2
    .size vl_thread_trampoline, .-vl_thread_trampoline

    # Mark non-executable stack to silence ld warning
    .section .note.GNU-stack,"",@progbits
