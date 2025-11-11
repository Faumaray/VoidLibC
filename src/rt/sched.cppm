export module rt.sched;

#if defined(__linux__) && defined(__x86_64__)
import rt.thread; // provides sched_yield()
#endif

namespace rt::sched {
    export int yield() noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return rt::thread::sched_yield();
#else
        return 0;
#endif
    }
}
