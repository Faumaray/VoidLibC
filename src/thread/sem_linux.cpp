module voidlibc.thread.sem;

import voidlibc.core.types;
import voidlibc.sys.linux;
import voidlibc.c.time;

using namespace vl::sys::linux;

static constexpr int FUTEX_WAIT = 0;
static constexpr int FUTEX_WAKE = 1;
static constexpr int FUTEX_PRIVATE_FLAG = 128;

extern "C" int sem_init(sem_t* s, int, unsigned int value) noexcept {
  s->val = (int)value; s->pad = 0; return 0;
}
extern "C" int sem_destroy(sem_t*) noexcept { return 0; }

extern "C" int sem_post(sem_t* s) noexcept {
  int old = __atomic_fetch_add(&s->val, 1, __ATOMIC_RELEASE);
  if (old < 0) { // had waiters
    (void)sc4(SYS_futex, (long)&s->val, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, 0);
  }
  return 0;
}

extern "C" int sem_trywait(sem_t* s) noexcept {
  for (;;) {
    int v = __atomic_load_n(&s->val, __ATOMIC_ACQUIRE);
    if (v <= 0) return 11; // EAGAIN
    int nv = v - 1;
    if (__atomic_compare_exchange_n(&s->val, &v, nv, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED))
      return 0;
  }
}

extern "C" int sem_wait(sem_t* s) noexcept {
  for (;;) {
    // fast path
    int v = __atomic_load_n(&s->val, __ATOMIC_ACQUIRE);
    while (v > 0) {
      int nv = v - 1;
      if (__atomic_compare_exchange_n(&s->val, &v, nv, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED))
        return 0;
    }
    // contended: set negative to mark waiters and sleep
    int neg = -1;
    if (__atomic_compare_exchange_n(&s->val, &v, neg, true, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
      (void)sc4(SYS_futex, (long)&s->val, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, neg, 0);
    } else {
      // someone changed it; loop
    }
  }
}

extern "C" int sem_timedwait(sem_t* s, const struct timespec* abstime) noexcept {
  // convert absolute CLOCK_REALTIME to relative
  timespec now{};
  if (clock_gettime(0, &now) < 0) return -1;
  long sec = abstime->tv_sec - now.tv_sec;
  long nsec = abstime->tv_nsec - now.tv_nsec;
  if (nsec < 0) { nsec += 1000000000L; sec -= 1; }
  if (sec < 0) return 110; // ETIMEDOUT
  timespec rel{ sec, nsec };

  for (;;) {
    int v = __atomic_load_n(&s->val, __ATOMIC_ACQUIRE);
    while (v > 0) {
      int nv = v - 1;
      if (__atomic_compare_exchange_n(&s->val, &v, nv, false, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED))
        return 0;
    }
    int neg = -1;
    if (__atomic_compare_exchange_n(&s->val, &v, neg, true, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
      long r = sc4(SYS_futex, (long)&s->val, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, neg, (long)&rel);
      if (r == -110) return 110; // ETIMEDOUT
    }
  }
}
