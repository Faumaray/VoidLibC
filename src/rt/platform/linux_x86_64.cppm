export module rt.platform.linux_x86_64;
import rt.sys;

namespace {
    inline long sys_write(long fd, long buf, long len) {
        long ret;
        asm volatile(
          "syscall"
          : "=a"(ret)
          : "a"(1), "D"(fd), "S"(buf), "d"(len)
          : "rcx", "r11", "memory"
        );
        return ret;
    }
    [[noreturn]] inline void sys_exit(long code) {
        asm volatile(
          "syscall"
          :
          : "a"(60), "D"(code)
          : "rcx", "r11", "memory"
        );
        __builtin_unreachable();
    }

    constexpr rt::sys::vtable LINUX_VT = {
        /* write */ [](int fd, const void* buf, __SIZE_TYPE__ len) noexcept -> long {
            return sys_write(fd, (long)buf, (long)len);
        },
        /* exit  */ [](int code) noexcept -> void {
            sys_exit(code);
        }
    };
}

export namespace rt::platform::linux_x86_64 {
    const rt::sys::vtable* vtab() noexcept { return &LINUX_VT; }
}
