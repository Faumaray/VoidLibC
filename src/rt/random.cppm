export module rt.random;

import rt.errno;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sysnums;
#endif

namespace rt {
    // flags: 0 / GRND_NONBLOCK(0x0001) / GRND_RANDOM(0x0002)
    export long getrandom(void* buf, unsigned long len, unsigned int flags) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        auto call = [](long n, long a, long b, long c){
            long r; asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b),"d"(c) : "rcx","r11","memory"); return r;
        };
        long r = call(rt::linux::__NR_getrandom, (long)buf, (long)len, flags);
        if (r < 0) { set_errno((int)-r); return -1; }
        return r;
#else
        set_errno(38); return -1;
#endif
    }
}
