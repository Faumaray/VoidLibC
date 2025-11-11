export module rt.platform.windows_pe;
import rt.panic;
import rt.sys;

export namespace rt::platform::windows_pe {
    const rt::sys::vtable* vtab() noexcept {
        // TODO: implement using ntdll Zw/Nt calls in future
        rt::panic("rt: Windows backend not implemented yet\n");
    }
}
