module voidlibc.thread.mutex;
import voidlibc.core.types;
import voidlibc.core.tls;
import voidlibc.sys.linux;

using namespace vl::sys::linux;

static constexpr int FUTEX_WAIT = 0;
static constexpr int FUTEX_WAKE = 1;
static constexpr int FUTEX_PRIVATE_FLAG = 128;
static constexpr int EBUSY = 16;

extern "C" int pthread_mutex_init(pthread_mutex_t* m, const pthread_mutexattr_t*) noexcept {
    m->state = 0; m->pad = 0; return 0;
}
extern "C" int pthread_mutex_destroy(pthread_mutex_t*) noexcept { return 0; }

extern "C" int pthread_mutex_trylock(pthread_mutex_t* m) noexcept {
    int expected = 0;
    if (__atomic_compare_exchange_n(&m->state, &expected, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
        return 0;
    return EBUSY;
}

extern "C" int pthread_mutex_lock(pthread_mutex_t* m) noexcept {
    // fast path
    int expected = 0;
    if (__atomic_compare_exchange_n(&m->state, &expected, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
        return 0;

    // contended path: mark as 2 (locked+waiters), sleep until becomes 0
    for (;;) {
        int s = __atomic_load_n(&m->state, __ATOMIC_RELAXED);
        if (s == 0) {
            if (__atomic_compare_exchange_n(&m->state, &s, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
                return 0;
            continue;
        }
        if (s != 2) {
            int two = 2;
            __atomic_compare_exchange_n(&m->state, &s, two, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
        }
        (void)sc4(SYS_futex, (long)&m->state, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, 2, 0);
    }
}

extern "C" int pthread_mutex_unlock(pthread_mutex_t* m) noexcept {
    int s = __atomic_load_n(&m->state, __ATOMIC_RELAXED);
    if (s == 1) {
        // no waiters
        __atomic_store_n(&m->state, 0, __ATOMIC_RELEASE);
        return 0;
    }
    // wake one waiter
    __atomic_store_n(&m->state, 0, __ATOMIC_RELEASE);
    (void)sc4(SYS_futex, (long)&m->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, 0);
    return 0;
}
