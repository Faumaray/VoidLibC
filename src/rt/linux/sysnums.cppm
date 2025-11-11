export module rt.linux.sysnums;

namespace rt::linux {
    // minimal set; extend as needed
    export constexpr long __NR_read      = 0;
    export constexpr long __NR_write     = 1;
    export constexpr long __NR_openat    = 257;
    export constexpr long __NR_close     = 3;
    export constexpr long __NR_fstat     = 5;   // newfstatat is 262 if you need it
    export constexpr long __NR_lseek     = 8;
    export constexpr long __NR_mmap      = 9;
    export constexpr long __NR_mprotect  = 10;
    export constexpr long __NR_munmap    = 11;
    export constexpr long __NR_nanosleep = 35;
    export constexpr long __NR_clock_gettime = 228;
    export constexpr long __NR_exit      = 60;
    export constexpr long __NR_newfstatat = 262;
    export constexpr long __NR_getdents64 = 217;
    export constexpr long __NR_getrandom  = 318;
    export constexpr long __NR_getpid     = 39;
    export constexpr long __NR_getuid     = 102;
    export constexpr long __NR_geteuid    = 107;
}
