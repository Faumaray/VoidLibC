export module rt.thread;

import rt.errno;
import rt.types;
import rt.time;

#if defined(__linux__) && defined(__x86_64__)
  import rt.linux.sysnums_thread;
#endif

namespace rt::thread {
  // futex operations (subset)
  export constexpr int FUTEX_WAIT = 0;
  export constexpr int FUTEX_WAKE = 1;
  export constexpr int FUTEX_PRIVATE_FLAG = 128;

  // low-level futex
  export long futex(rt_i32* uaddr, int op, rt_i32 val, const rt::time::timespec* timeout,
                    rt_i32* uaddr2, rt_i32 val3) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r;
    register long r10 __asm__("r10") = (long)timeout;
    register long r8  __asm__("r8")  = (long)uaddr2;
    register long r9  __asm__("r9")  = (long)val3;
    asm volatile("syscall"
      : "=a"(r)
      : "a"(rt::linux::__NR_futex), "D"(uaddr), "S"(op), "d"(val),
        "r"(r10), "r"(r8), "r"(r9)
      : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return r;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  // sched_yield
  export int sched_yield() noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r):"a"(rt::linux::__NR_sched_yield):"rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  // minimal spin+futex mutex
  export struct mutex {
    // 0 = unlocked, 1 = locked
    rt_i32 state{0};
  };

  export void mutex_lock(mutex* m) noexcept {
    // simple TAS spin, then futex wait
    while (__atomic_exchange_n(&m->state, 1, __ATOMIC_ACQUIRE) == 1) {
      // if already locked, futex WAIT until it changes
      rt_i32 expected = 1;
      futex(&m->state, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, expected, nullptr, nullptr, 0);
    }
  }

  export void mutex_unlock(mutex* m) noexcept {
    __atomic_store_n(&m->state, 0, __ATOMIC_RELEASE);
    // wake one waiter
    futex(&m->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0);
  }
}
