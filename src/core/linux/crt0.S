.section .text
.extern environ            # add near top
.globl _start
.type _start,@function
_start:
  xor   %rbp, %rbp

  # Parse argc/argv/envp from initial kernel stack
  mov   (%rsp), %rdi
  lea   8(%rsp), %rsi
  lea   16(%rsp,%rdi,8), %rdx
  mov   %rdx, environ(%rip)
  # Allocate our own 2 MiB stack
  mov   $9, %rax                  # SYS_mmap
  xor   %rdi, %rdi                # addr = NULL
  mov   $0x200000, %rsi           # len  = 2 MiB
  mov   $3, %rdx                  # PROT_READ|PROT_WRITE
  mov   $0x22, %r10               # MAP_PRIVATE|MAP_ANONYMOUS
  mov   $-1, %r8                  # fd = -1
  xor   %r9, %r9                  # off = 0
  syscall
  mov   $-4095, %r11
  cmp   %r11, %rax
  jae   1f                        # mmap failed → keep kernel stack
  lea   0x200000(%rax), %rsp      # switch to top of new stack
1:
  and   $-16, %rsp                # 16B align before calls

  # TLS TCB for main thread (1 page), self-pointer, ARCH_SET_FS
  mov   $9, %rax                  # SYS_mmap
  xor   %rdi, %rdi
  mov   $4096, %rsi               # len  = 1 page
  mov   $3, %rdx                  # PROT_READ|PROT_WRITE
  mov   $0x22, %r10               # MAP_PRIVATE|MAP_ANONYMOUS
  mov   $-1, %r8
  xor   %r9, %r9
  syscall
  mov   %rax, %r12                # r12 = tcb (or errno<0)
  mov   $-4095, %r11
  cmp   %r11, %r12
  jae   2f                        # mmap failed → no TLS (fallback errno)
  mov   %r12, (%r12)              # tcb->self = tcb
  mov   $158, %rax                # SYS_arch_prctl
  mov   $0x1002, %rdi             # ARCH_SET_FS
  mov   %r12, %rsi                # FS base = tcb
  syscall
2:
  # Probe a couple pages so stack VMA is faulted in
  sub   $8192, %rsp
  movb  $0, (%rsp)
  add   $8192, %rsp

  # Call main(int argc, char** argv, char** envp)
  call  main

  # Tail through libc exit() so atexit() + TLS dtors run
  mov   %eax, %edi                # status (int)  <<<< FIXED
  call  exit                      # noreturn

  ud2
.size _start, .-_start

# Non-exec stack note
.section .note.GNU-stack,"",@progbits
