export module voidlibc.c.time;
export import voidlibc.core.types;   // re-export types

export extern "C" {
    int      clock_gettime(int clk_id, timespec* ts);
    int      nanosleep(const timespec* req, timespec* rem);
    unsigned sleep(unsigned sec);
    int      gettimeofday(timeval* tv, void* tz);  // implemented via clock_gettime
    time_t   time(time_t* tloc);
}
