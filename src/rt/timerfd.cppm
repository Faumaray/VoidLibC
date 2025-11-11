export module rt.timerfd;

import rt.errno;
import rt.time;
#if defined(__linux__) && defined(__x86_64__)
import rt.linux.syscall;
#endif

namespace rt::timerfd {
    // structures matching kernel uapi
    export struct itimerspec    { rt::time::timespec it_interval; rt::time::timespec it_value; };

    // flags / clock ids
    export constexpr int CLOCK_REALTIME   = 0;
    export constexpr int CLOCK_MONOTONIC  = 1;
    export constexpr int TFD_CLOEXEC      = 0x80000;
    export constexpr int TFD_NONBLOCK     = 0x800;
    export constexpr int TFD_TIMER_ABSTIME= 1;

    // timerfd_create(clockid, flags)
    export int create(int clockid, int flags) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r = rt::linux::sc2(283, (long)clockid, (long)flags);
        if (r < 0) { rt::set_errno((int)-r); return -1; }
        return (int)r;
#else
        rt::set_errno(38); return -1;
#endif
    }

    // timerfd_settime(fd, flags, new_value*, old_value*)
    export int settime(int fd, int flags, const itimerspec* nv, itimerspec* ov) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r = rt::linux::sc4(286, (long)fd, (long)flags, (long)nv, (long)ov);
        if (r < 0) { rt::set_errno((int)-r); return -1; }
        return 0;
#else
        rt::set_errno(38); return -1;
#endif
    }

    // timerfd_gettime(fd, curr_value*)
    export int gettime(int fd, itimerspec* out) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r = rt::linux::sc2(287, (long)fd, (long)out);
        if (r < 0) { rt::set_errno((int)-r); return -1; }
        return 0;
#else
        rt::set_errno(38); return -1;
#endif
    }
}
