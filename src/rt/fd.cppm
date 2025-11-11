export module rt.fd;

import rt.errno;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sysnums_poll_net;
#endif

namespace rt {
    export int dup(int oldfd) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r):"a"(rt::linux::__NR_dup),"D"(oldfd):"rcx","r11","memory");
        if (r<0){ set_errno((int)-r); return -1; } return (int)r;
#else
        set_errno(38); return -1;
#endif
    }

    export int dup3(int oldfd, int newfd, int flags) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r):"a"(rt::linux::__NR_dup3),"D"(oldfd),"S"(newfd),"d"(flags):"rcx","r11","memory");
        if (r<0){ set_errno((int)-r); return -1; } return (int)r;
#else
        set_errno(38); return -1;
#endif
    }

    // pipe2: fds[0]=read, fds[1]=write
    export int pipe2(int fds[2], int flags) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r):"a"(rt::linux::__NR_pipe2),"D"(fds),"S"(flags):"rcx","r11","memory");
        if (r<0){ set_errno((int)-r); return -1; } return 0;
#else
        set_errno(38); return -1;
#endif
    }
}
