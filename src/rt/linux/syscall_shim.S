    .text

    # long __rt_linux_syscall0(long nr)
    .globl __rt_linux_syscall0
    .type  __rt_linux_syscall0, @function
__rt_linux_syscall0:
    mov %rdi, %rax      # nr
    syscall
    ret

    # long __rt_linux_syscall1(long nr, long a1)
    .globl __rt_linux_syscall1
    .type  __rt_linux_syscall1, @function
__rt_linux_syscall1:
    mov %rdi, %rax      # nr
    mov %rsi, %rdi      # a1
    syscall
    ret

    # long __rt_linux_syscall2(long nr, long a1, long a2)
    .globl __rt_linux_syscall2
    .type  __rt_linux_syscall2, @function
__rt_linux_syscall2:
    mov %rdi, %rax
    mov %rsi, %rdi
    mov %rdx, %rsi
    syscall
    ret

    # long __rt_linux_syscall3(long nr, long a1, long a2, long a3)
    .globl __rt_linux_syscall3
    .type  __rt_linux_syscall3, @function
__rt_linux_syscall3:
    mov %rdi, %rax
    mov %rsi, %rdi
    mov %rdx, %rsi
    mov %rcx, %rdx      # SysV a3 is rcx in our C ABI position
    syscall
    ret

    # long __rt_linux_syscall4(long nr, long a1, long a2, long a3, long a4)
    .globl __rt_linux_syscall4
    .type  __rt_linux_syscall4, @function
__rt_linux_syscall4:
    mov %rdi, %rax
    mov %rsi, %rdi
    mov %rdx, %rsi
    mov %rcx, %rdx
    mov %r8,  %r10      # kernel needs arg4 in r10
    syscall
    ret

    # long __rt_linux_syscall5(long nr, long a1, long a2, long a3, long a4, long a5)
    .globl __rt_linux_syscall5
    .type  __rt_linux_syscall5, @function
__rt_linux_syscall5:
    mov %rdi, %rax
    mov %rsi, %rdi
    mov %rdx, %rsi
    mov %rcx, %rdx
    mov %r8,  %r10
    # a5 already in r9? No: SysV puts it in r8; kernel wants r8. Keep r8.
    mov %r9,  %r8
    syscall
    ret

    # long __rt_linux_syscall6(long nr, long a1, long a2, long a3, long a4, long a5, long a6)
    .globl __rt_linux_syscall6
    .type  __rt_linux_syscall6, @function
__rt_linux_syscall6:
    mov %rdi, %rax
    mov %rsi, %rdi
    mov %rdx, %rsi
    mov %rcx, %rdx
    mov %r8,  %r10
    mov %r9,  %r8
    mov 8(%rsp), %r9    # a6 is on stack at call-site per SysV; load to r9
    syscall
    ret
