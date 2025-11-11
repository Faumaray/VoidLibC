export module rt.process;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sysnums;
#endif

namespace rt {
    export int getpid() noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall" : "=a"(r) : "a"(rt::linux::__NR_getpid) : "rcx","r11","memory");
        return static_cast<int>(r);
#else
        return 0;
#endif
    }

    export unsigned getuid() noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall" : "=a"(r) : "a"(rt::linux::__NR_getuid) : "rcx","r11","memory");
        return static_cast<unsigned>(r);
#else
        return 0;
#endif
    }

    export unsigned geteuid() noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall" : "=a"(r) : "a"(rt::linux::__NR_geteuid) : "rcx","r11","memory");
        return static_cast<unsigned>(r);
#else
        return 0;
#endif
    }
}
