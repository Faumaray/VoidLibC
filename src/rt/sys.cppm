export module rt.sys;
import rt.abi; // keep only what you actually use

export namespace rt::sys {
    struct vtable {
        long  (*write)(int fd, const void* buf, __SIZE_TYPE__ len) noexcept;
        [[noreturn]] void (*exit)(int code) noexcept;
    };

    inline const vtable* g = nullptr;

    long write(int fd, const void* buf, __SIZE_TYPE__ len) noexcept {
        return g ? g->write(fd, buf, len) : 0L;
    }

    [[noreturn]] void exit(int code) noexcept {
        if (g) g->exit(code);
        __builtin_trap(); // no import of rt.panic here
    }

    void attach(const vtable* vt) noexcept { g = vt; }
}
