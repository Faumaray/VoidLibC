.section .text
.globl __vl_rt_sigreturn
.type  __vl_rt_sigreturn,@function
__vl_rt_sigreturn:
  mov   $15, %rax        # SYS_rt_sigreturn on x86-64
  syscall
  ud2                    # should not return
.size __vl_rt_sigreturn, .-__vl_rt_sigreturn

.section .note.GNU-stack,"",@progbits
