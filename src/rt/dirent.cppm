export module rt.dirent;

import rt.errno;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sysnums;
import rt.fs;     // for close/read/write if you want helpers; not required here
#endif

namespace rt {
    // struct linux_dirent64 (kernel ABI)
    export struct dirent64 {
        unsigned long d_ino;
        long          d_off;
        unsigned short d_reclen;
        unsigned char  d_type;
        char           d_name[]; // flexible
    };

    // Low-level: raw getdents64 into user buffer.
    export long getdents64(int fd, void* buf, unsigned long len) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        auto call = [](long n, long a, long b, long c){
            long r; asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b),"d"(c) : "rcx","r11","memory"); return r;
        };
        long r = call(rt::linux::__NR_getdents64, fd, (long)buf, (long)len);
        if (r < 0) { set_errno((int)-r); return -1; }
        return r;
#else
        set_errno(38); return -1;
#endif
    }
}
