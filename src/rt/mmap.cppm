export module rt.mmap;
import rt.errno;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sys;
#endif

namespace rt {
    // Common PROT/FLAGS are passed through verbatim for Linux; define your own in a header later.
    export void* mmap(void* addr, unsigned long len, long prot, long flags, int fd, long off) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r = linux::sys_mmap(addr, len, prot, flags, fd, off);
        return (r == -1) ? (void*)0 : (void*)r;
#else
        set_errno(38); return (void*)0;
#endif
    }

    export long munmap(void* addr, unsigned long len) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_munmap(addr, len);
#else
        set_errno(38); return -1;
#endif
    }

    export long mprotect(void* addr, unsigned long len, long prot) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_mprotect(addr, len, prot);
#else
        set_errno(38); return -1;
#endif
    }
}
