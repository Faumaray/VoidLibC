export module rt.cwd;

import rt.errno;
import rt.types;
import rt.cstring;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sysnums_proc;
#endif

namespace rt::cwd {
    // getcwd into caller buffer
    export char* getcwd(char* buf, rt_usize size) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r)
          : "a"(rt::linux::__NR_getcwd), "D"(buf), "S"(size)
          : "rcx","r11","memory");
        if (r < 0) { rt::set_errno((int)-r); return (char*)0; }
        return buf; // kernel NUL-terminates
#else
        rt::set_errno(38); return (char*)0;
#endif
    }

    export int chdir(const char* path) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r)
          : "a"(rt::linux::__NR_chdir), "D"(path)
          : "rcx","r11","memory");
        if (r < 0) { rt::set_errno((int)-r); return -1; } return 0;
#else
        rt::set_errno(38); return -1;
#endif
    }
}
