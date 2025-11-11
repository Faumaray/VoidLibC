.section __TEXT,__text
.globl _start
.p2align 4
_start:
  # Stack on Mach-O: [argc][argv...,0][envp...,0][apple,...]
  popq  %rdi                  # argc
  mov   %rsp, %rsi            # argv
  lea   8(%rsi,%rdi,8), %rdx  # envp = argv + argc + 1
  and   $-16, %rsp
  call  _main                 # int main(int,char**,char**)
  mov   %eax, %edi
  call  _exit                 # noreturn

.subsections_via_symbols
