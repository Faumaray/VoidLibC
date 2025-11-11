export module rt.linux.sysnums_proc;

namespace rt::linux {
    // process
    export constexpr long __NR_fork    = 57;
    export constexpr long __NR_vfork   = 58;
    export constexpr long __NR_execve  = 59;
    export constexpr long __NR_wait4   = 61;

    // cwd / path
    export constexpr long __NR_getcwd     = 79;
    export constexpr long __NR_chdir      = 80;
    export constexpr long __NR_readlinkat = 267;

    // fs admin (subset)
    export constexpr long __NR_unlinkat = 263;
    export constexpr long __NR_linkat   = 265;
    export constexpr long __NR_symlinkat= 266;
    export constexpr long __NR_renameat2= 316;
    export constexpr long __NR_utimensat= 280;

    // signals
    export constexpr long __NR_rt_sigaction   = 13;
    export constexpr long __NR_rt_sigprocmask = 14;
    // tgkill already in sysnums_thread
}
