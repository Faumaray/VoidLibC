export module rt.exit;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sys;
#endif

namespace rt {
    export [[noreturn]] void _exit(int code) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        linux::sys_exit(code);
#else
        __builtin_trap();
#endif
    }

    // Minimal abort: trap. (You can route to panic if desired.)
    export [[noreturn]] void abort() noexcept {
        __builtin_trap();
    }
}
