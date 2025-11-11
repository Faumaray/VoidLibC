module voidlibc.c.time;
import voidlibc.core.types;
import voidlibc.core.tls;
import voidlibc.sys.linux;

using namespace vl::sys::linux;

extern "C" int clock_gettime(int clk_id, timespec* ts){
    long r = sc2(SYS_clock_gettime, clk_id, (long)ts);
    if (r<0){ vl_errno() = -r; return -1; } return 0;
}
extern "C" int nanosleep(const timespec* req, timespec* rem){
    long r = sc2(SYS_nanosleep, (long)req, (long)rem);
    if (r<0){ vl_errno() = -r; return -1; } return 0;
}
extern "C" unsigned sleep(unsigned sec){
    timespec req{ (long)sec, 0 }, rem{ 0, 0 };
    while (nanosleep(&req, &rem) < 0){
        if (vl_errno() != 4 /*EINTR*/) break;
        req = rem;
    }
    return 0;
}
extern "C" int gettimeofday(timeval* tv, void* /*tz*/){
    // Implement via CLOCK_REALTIME for simplicity and 64-bit safety
    timespec ts{};
    if (clock_gettime(0 /*CLOCK_REALTIME*/, &ts) < 0) return -1;
    tv->tv_sec  = ts.tv_sec;
    tv->tv_usec = (ts.tv_nsec / 1000);
    return 0;
}
extern "C" time_t time(time_t* tloc){
    timespec ts{};
    if (clock_gettime(0 /*CLOCK_REALTIME*/, &ts) < 0) return (time_t)-1;
    if (tloc) *tloc = ts.tv_sec;
    return ts.tv_sec;
}
