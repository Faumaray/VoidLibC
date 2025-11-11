.section .init_array,"aw"
.p2align 3
.quad __vl_so_init

.section .fini_array,"aw"
.p2align 3
.quad __vl_so_fini

.section .note.GNU-stack,"",@progbits
