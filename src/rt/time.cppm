export module rt.time;
import rt.errno;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sys;
import rt.linux.syscall;
#endif

namespace rt::time {
    // simple timespec (avoid libc headers)
    export struct timespec { long tv_sec; long tv_nsec; };

    export constexpr int CLOCK_REALTIME  = 0;
    export constexpr int CLOCK_MONOTONIC = 1;

    export int nanosleep(const timespec* req, timespec* rem) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r = rt::linux::sc2(35, (long)req, (long)rem); // __NR_nanosleep
        if (r < 0) { rt::set_errno((int)-r); return -1; }
        return 0;
#else
        rt::set_errno(38); return -1;
#endif
    }

    export int clock_nanosleep(int clk, int flags, const timespec* req, timespec* rem) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        long r = rt::linux::sc4(230, (long)clk, (long)flags, (long)req, (long)rem); // __NR_clock_nanosleep
        if (r < 0) { rt::set_errno((int)-r); return -1; }
        return 0;
#else
        rt::set_errno(38); return -1;
#endif
    }

    // CLOCK_REALTIME=0, CLOCK_MONOTONIC=1 (Linux)
    export long clock_gettime(int clk_id, timespec* ts) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_clock_gettime(clk_id, ts);
#else
        set_errno(38); return -1;
#endif
    }
    export unsigned sleep(unsigned seconds) noexcept {
        timespec req{ (long)seconds, 0 }, rem{0,0};
        int r = nanosleep(&req, &rem);
        if (r == 0) return 0;
        // if interrupted, return unslept seconds
        return (unsigned)rem.tv_sec;
    }

    export int usleep(unsigned usec) noexcept {
        timespec req{ (long)(usec / 1000000u), (long)((usec % 1000000u) * 1000u) };
        return nanosleep(&req, (timespec*)0);
    }
}
