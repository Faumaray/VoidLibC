export module rt.entry;
import rt.abi;
import rt.sys;
import rt.panic;
import rt.platform.linux_x86_64;
import rt.platform.windows_pe;
import rt.platform.darwin_macho;
import rt.env;

extern "C" int main(int, char**, char**); // demo main for bring-up

namespace {
    using namespace rt;

    abi::args parse_args(void* raw_sp) noexcept {
        auto* sp = reinterpret_cast<__UINTPTR_TYPE__*>(raw_sp);
        int argc = static_cast<int>(*sp);
        char** argv = reinterpret_cast<char**>(sp + 1);
        char** envp = argv + argc + 1;
        return {argc, argv, envp};
    }

    // Extremely simple runtime target probe. For now, assume Linux if
    // ELF syscall succeeds; extend with richer PE/Mach-O detection when polyglot.
    void attach_backend() {
#if defined(__x86_64__)
        sys::attach(rt::platform::linux_x86_64::vtab());
#else
        rt::panic("rt: unsupported arch for now (need backend)\n");
#endif
    }
}

export extern "C" [[noreturn]] void runtime_entry(void* raw_sp) {
    attach_backend();
    auto a = parse_args(raw_sp);
    rt::_env_set(a.argc, a.argv, a.envp);
    int rc = main(a.argc, a.argv, a.envp);
    rt::sys::exit(rc);
}
