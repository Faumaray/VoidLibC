module voidlibc.thread.once;

import voidlibc.core.types;
import voidlibc.sys.linux;

using namespace vl::sys::linux;

static constexpr int FUTEX_WAIT = 0;
static constexpr int FUTEX_WAKE = 1;
static constexpr int FUTEX_PRIVATE_FLAG = 128;

extern "C" int pthread_once(pthread_once_t* c, void(*init)()) noexcept {
    int s = __atomic_load_n(&c->state, __ATOMIC_ACQUIRE);
    if (s == 2) return 0;

    int expected = 0;
    if (__atomic_compare_exchange_n(&c->state, &expected, 1, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
        // We are the initializer
        init();
        __atomic_store_n(&c->state, 2, __ATOMIC_RELEASE);
        (void)sc4(SYS_futex, (long)&c->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 0x7fffffff, 0);
        return 0;
    }

    // Someone else is initializing; wait until DONE
    for (;;) {
        s = __atomic_load_n(&c->state, __ATOMIC_ACQUIRE);
        if (s == 2) return 0;
        (void)sc4(SYS_futex, (long)&c->state, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, 1, 0);
    }
}
