export module voidlibc.sys.linux;

/* x86-64 Linux syscall shim & constants
 * - returns: rax (negative values are -errno)
 * - calling conv: rax=n, rdi=a1, rsi=a2, rdx=a3, r10=a4, r8=a5, r9=a6
 * - clobbers: rcx, r11, memory
 */
export namespace vl::sys::linux {

  /* ----- syscall numbers (subset we use) ----- */
  inline constexpr long
    SYS_read          =   0,
    SYS_write         =   1,
    SYS_open          =   2,
    SYS_close         =   3,
    SYS_stat          =   4,
    SYS_fstat         =   5,
    SYS_poll         = 7,
    SYS_lseek         =   8,
    SYS_mmap          =   9,
    SYS_mprotect      =  10,
    SYS_munmap        =  11,
    SYS_rt_sigaction  =  13,
    SYS_rt_sigprocmask=  14,
    SYS_sched_yield   =  24,
    SYS_nanosleep     =  35,
    SYS_alarm        = 37,
    SYS_getpid       = 39,
    SYS_socket       = 41,
     SYS_connect      = 42,
     SYS_accept       = 43,
     SYS_sendto       = 44,
     SYS_recvfrom     = 45,
     SYS_sendmsg      = 46,
     SYS_recvmsg      = 47,
     SYS_shutdown     = 48,
     SYS_bind         = 49,
     SYS_listen       = 50,
     SYS_getsockname  = 51,
     SYS_getpeername  = 52,
     SYS_socketpair   = 53,
     SYS_setsockopt   = 54,
     SYS_getsockopt   = 55,
    SYS_clone         =  56,
    SYS_fork         = 57,
    SYS_execve       = 59,
    SYS_exit          =  60,
    SYS_wait4        = 61,
    SYS_kill          = 62,
    SYS_getcwd        =  79,
    SYS_mkdir         =  83,
    SYS_rmdir         =  84,
    SYS_unlink        =  87,
    SYS_getppid      = 110,
    SYS_arch_prctl    = 158,
    SYS_gettid       = 186,
    SYS_futex         = 202,
    SYS_getdents64    = 217,
    SYS_set_tid_address = 218,
    SYS_clock_gettime = 228,
    SYS_exit_group    = 231,
    SYS_openat        = 257,
    SYS_ppoll        = 271,
    SYS_accept4      = 288;

  /* ----- common flags (subset) ----- */
  // mmap/mprotect
  inline constexpr int
    PROT_NONE   = 0x0,
    PROT_READ   = 0x1,
    PROT_WRITE  = 0x2,
    PROT_EXEC   = 0x4;

  inline constexpr int
    MAP_SHARED      = 0x01,
    MAP_PRIVATE     = 0x02,
    MAP_FIXED       = 0x10,
    MAP_ANONYMOUS   = 0x20; // aka MAP_ANON

  // futex ops
  inline constexpr int
    FUTEX_WAIT           = 0,
    FUTEX_WAKE           = 1,
    FUTEX_PRIVATE_FLAG   = 128,
    FUTEX_WAIT_PRIVATE   = (FUTEX_WAIT | FUTEX_PRIVATE_FLAG),
    FUTEX_WAKE_PRIVATE   = (FUTEX_WAKE | FUTEX_PRIVATE_FLAG);

  // clone flags (subset we use)
  inline constexpr unsigned long
    CLONE_VM             = 0x00000100,
    CLONE_FS             = 0x00000200,
    CLONE_FILES          = 0x00000400,
    CLONE_SIGHAND        = 0x00000800,
    CLONE_PARENT         = 0x00008000,
    CLONE_THREAD         = 0x00010000,
    CLONE_SYSVSEM        = 0x00040000,
    CLONE_SETTLS         = 0x00080000,
    CLONE_PARENT_SETTID  = 0x00100000,
    CLONE_CHILD_CLEARTID = 0x00200000,
    CLONE_CHILD_SETTID   = 0x01000000;

  // arch_prctl codes
  inline constexpr long
    ARCH_SET_FS = 0x1002,
    ARCH_GET_FS = 0x1003;

  /* ----- raw syscall wrappers sc0..sc6 (x86-64) ----- */
  inline long sc0(long n) {
    long r;
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(n)
                 : "rcx","r11","memory");
    return r;
  }

  inline long sc1(long n, long a1) {
    long r;
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(n), "D"(a1)
                 : "rcx","r11","memory");
    return r;
  }

  inline long sc2(long n, long a1, long a2) {
    long r;
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(n), "D"(a1), "S"(a2)
                 : "rcx","r11","memory");
    return r;
  }

  inline long sc3(long n, long a1, long a2, long a3) {
    long r;
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(n), "D"(a1), "S"(a2), "d"(a3)
                 : "rcx","r11","memory");
    return r;
  }

  inline long sc4(long n, long a1, long a2, long a3, long a4) {
    long r;
    register long r10 asm("r10") = a4;
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10)
                 : "rcx","r11","memory");
    return r;
  }

  inline long sc5(long n, long a1, long a2, long a3, long a4, long a5) {
    long r;
    register long r10 asm("r10") = a4;
    register long r8  asm("r8")  = a5;
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8)
                 : "rcx","r11","memory");
    return r;
  }

  inline long sc6(long n, long a1, long a2, long a3, long a4, long a5, long a6) {
    long r;
    register long r10 asm("r10") = a4;
    register long r8  asm("r8")  = a5;
    register long r9  asm("r9")  = a6;
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8), "r"(r9)
                 : "rcx","r11","memory");
    return r;
  }

} // namespace vl::sys::linux
