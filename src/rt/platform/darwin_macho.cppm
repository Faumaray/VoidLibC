export module rt.platform.darwin_macho;
import rt.panic;
import rt.sys;

export namespace rt::platform::darwin_macho {
    const rt::sys::vtable* vtab() noexcept {
        // TODO: implement macOS xnu syscall veneer in future
        rt::panic("rt: Darwin backend not implemented yet\n");
    }
}
