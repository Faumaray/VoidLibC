export module rt.poll;

import rt.errno;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sysnums_poll_net;
#endif

namespace rt::polling {
    export struct pollfd { int fd; short events; short revents; };
    export constexpr short POLLIN  = 0x001;
    export constexpr short POLLOUT = 0x004;
    export constexpr short POLLERR = 0x008;
    export constexpr short POLLHUP = 0x010;
    export constexpr short POLLNVAL= 0x020;

    // poll(fds, nfds, timeout_ms)
    export int poll(pollfd* fds, unsigned long nfds, int timeout_ms) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r; asm volatile("syscall":"=a"(r)
          : "a"(rt::linux::__NR_poll), "D"(fds), "S"(nfds), "d"(timeout_ms)
          : "rcx","r11","memory");
        if (r<0){ rt::set_errno((int)-r); return -1; } return (int)r;
#else
        rt::set_errno(38); return -1;
#endif
    }

    // ppoll(fds, nfds, ts, sigmask)
    export struct timespec { long tv_sec; long tv_nsec; };
    export int ppoll(pollfd* fds, unsigned long nfds,
                 const timespec* ts, const void* sigmask,
                 unsigned long sigsetsize) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r;
        register long r10 __asm__("r10") = (long)sigmask;    // 4th
        register long r8  __asm__("r8")  = (long)sigsetsize; // 5th
        register long r9  __asm__("r9")  = 0L;               // 6th (unused)
        asm volatile("syscall"
                     : "=a"(r)
                     : "a"(rt::linux::__NR_ppoll),
                       "D"(fds), "S"(nfds), "d"(ts),
                       "r"(r10), "r"(r8), "r"(r9)
                     : "rcx","r11","memory");
        if (r < 0) { rt::set_errno((int)-r); return -1; } return (int)r;
#else
        rt::set_errno(38); return -1;
#endif
    }

}
