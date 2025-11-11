export module rt.linux.syscall;

// Decls for the .S shims
extern "C" long __rt_linux_syscall0(long);
extern "C" long __rt_linux_syscall1(long,long);
extern "C" long __rt_linux_syscall2(long,long,long);
extern "C" long __rt_linux_syscall3(long,long,long,long);
extern "C" long __rt_linux_syscall4(long,long,long,long,long);
extern "C" long __rt_linux_syscall5(long,long,long,long,long,long);
extern "C" long __rt_linux_syscall6(long,long,long,long,long,long,long);

namespace rt::linux {
    export inline long sc0(long nr)                                      { return __rt_linux_syscall0(nr); }
    export inline long sc1(long nr, long a1)                              { return __rt_linux_syscall1(nr,a1); }
    export inline long sc2(long nr, long a1, long a2)                     { return __rt_linux_syscall2(nr,a1,a2); }
    export inline long sc3(long nr, long a1, long a2, long a3)            { return __rt_linux_syscall3(nr,a1,a2,a3); }
    export inline long sc4(long nr, long a1, long a2, long a3, long a4)   { return __rt_linux_syscall4(nr,a1,a2,a3,a4); }
    export inline long sc5(long nr, long a1, long a2, long a3, long a4, long a5) {
        return __rt_linux_syscall5(nr,a1,a2,a3,a4,a5);
    }
    export inline long sc6(long nr, long a1, long a2, long a3, long a4, long a5, long a6) {
        return __rt_linux_syscall6(nr,a1,a2,a3,a4,a5,a6);
    }
}
