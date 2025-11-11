    .section .text
    .globl _start
    .type _start, @function
_start:
    xor     %rbp, %rbp
    mov     %rsp, %rdi     # runtime_entry(void* raw_sp)
    call    runtime_entry
    # If it ever returns:
    mov     $60, %rax      # SYS_exit
    mov     $127, %rdi
    syscall
    hlt
