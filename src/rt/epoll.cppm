export module rt.epoll;

import rt.errno;
import rt.types;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sysnums_poll_net;
#endif

namespace rt::epoll {
    // event flags
    export constexpr rt_u32 EPOLLIN     = 0x001;
    export constexpr rt_u32 EPOLLOUT    = 0x004;
    export constexpr rt_u32 EPOLLERR    = 0x008;
    export constexpr rt_u32 EPOLLHUP    = 0x010;
    export constexpr rt_u32 EPOLLRDHUP  = 0x2000;

    // ctl ops
    export constexpr int EPOLL_CTL_ADD = 1;
    export constexpr int EPOLL_CTL_DEL = 2;
    export constexpr int EPOLL_CTL_MOD = 3;

    export struct epoll_event {
        rt_u32 events;
        rt_u64 data; // user data
    };

    export int create1(int flags) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r):"a"(rt::linux::__NR_epoll_create1),"D"(flags):"rcx","r11","memory");
        if (r<0){ rt::set_errno((int)-r); return -1; } return (int)r;
#else
        rt::set_errno(38); return -1;
#endif
    }

    export int ctl(int epfd, int op, int fd, epoll_event* ev) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r)
          : "a"(rt::linux::__NR_epoll_ctl), "D"(epfd), "S"(op), "d"(fd), "r"(ev)
          : "rcx","r11","memory");
        if (r<0){ rt::set_errno((int)-r); return -1; } return 0;
#else
        rt::set_errno(38); return -1;
#endif
    }

    export int wait(int epfd, epoll_event* evs, int maxevents, int timeout_ms) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r;
        register long r10 __asm__("r10") = (long)timeout_ms; // 4th
        register long r8  __asm__("r8")  = 0L;               // 5th (sigmask)
        register long r9  __asm__("r9")  = 0L;               // 6th (sigsetsize)
        asm volatile("syscall"
                     : "=a"(r)
                     : "a"(rt::linux::__NR_epoll_pwait),
                       "D"(epfd), "S"(evs), "d"(maxevents),
                       "r"(r10), "r"(r8), "r"(r9)
                     : "rcx","r11","memory");
        if (r < 0) { rt::set_errno((int)-r); return -1; } return (int)r;
#else
        rt::set_errno(38); return -1;
#endif
    }
}
