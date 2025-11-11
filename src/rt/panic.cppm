export module rt.panic;
import rt.sys;
import rt.string;

export namespace rt {
    [[noreturn]] void panic(const char* msg) noexcept {
        sys::write(2, msg, string::strlen(msg));
        sys::exit(1);
        __builtin_unreachable();
    }

    [[noreturn]] void unreachable() noexcept {
        panic("rt: unreachable reached\n");
    }
}
