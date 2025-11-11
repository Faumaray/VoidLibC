export module rt.linux.sysnums_thread;

namespace rt::linux {
    export constexpr long __NR_clone           = 56;
    export constexpr long __NR_futex           = 202;
    export constexpr long __NR_set_tid_address = 218;
    export constexpr long __NR_tgkill          = 234;
    export constexpr long __NR_sched_yield     = 24;
}
