module voidlibc.thread.rwlock;

import voidlibc.core.types;
import voidlibc.sys.linux;

using namespace vl::sys::linux;

static constexpr int FUTEX_WAIT = 0;
static constexpr int FUTEX_WAKE = 1;
static constexpr int FUTEX_PRIVATE_FLAG = 128;

extern "C" int pthread_rwlock_init(pthread_rwlock_t* l, const pthread_rwlockattr_t*) noexcept {
  l->state = 0; l->wait_w = 0; l->wait_r = 0; l->pad = 0; return 0;
}
extern "C" int pthread_rwlock_destroy(pthread_rwlock_t*) noexcept { return 0; }

extern "C" int pthread_rwlock_tryrdlock(pthread_rwlock_t* l) noexcept {
  for (;;) {
    int s = __atomic_load_n(&l->state, __ATOMIC_RELAXED);
    if (s < 0) return 16; // EBUSY
    int ns = s + 1;
    if (__atomic_compare_exchange_n(&l->state, &s, ns, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
      return 0;
  }
}

extern "C" int pthread_rwlock_rdlock(pthread_rwlock_t* l) noexcept {
  for (;;) {
    int s = __atomic_load_n(&l->state, __ATOMIC_RELAXED);
    if (s >= 0) {
      int ns = s + 1;
      if (__atomic_compare_exchange_n(&l->state, &s, ns, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
        return 0;
      continue;
    }
    __atomic_fetch_add(&l->wait_r, 1, __ATOMIC_RELAXED);
    (void)sc4(SYS_futex, (long)&l->state, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, -1, 0);
    __atomic_fetch_sub(&l->wait_r, 1, __ATOMIC_RELAXED);
  }
}

extern "C" int pthread_rwlock_trywrlock(pthread_rwlock_t* l) noexcept {
  int zero = 0;
  if (__atomic_compare_exchange_n(&l->state, &zero, -1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
    return 0;
  return 16; // EBUSY
}

extern "C" int pthread_rwlock_wrlock(pthread_rwlock_t* l) noexcept {
  for (;;) {
    int s = __atomic_load_n(&l->state, __ATOMIC_RELAXED);
    if (s == 0) {
      int neg1 = -1;
      if (__atomic_compare_exchange_n(&l->state, &s, neg1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
        return 0;
      continue;
    }
    __atomic_fetch_add(&l->wait_w, 1, __ATOMIC_RELAXED);
    (void)sc4(SYS_futex, (long)&l->state, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, s, 0);
    __atomic_fetch_sub(&l->wait_w, 1, __ATOMIC_RELAXED);
  }
}

extern "C" int pthread_rwlock_unlock(pthread_rwlock_t* l) noexcept {
  int s = __atomic_load_n(&l->state, __ATOMIC_RELAXED);
  if (s == -1) {
    // release writer
    __atomic_store_n(&l->state, 0, __ATOMIC_RELEASE);
    if (__atomic_load_n(&l->wait_w, __ATOMIC_RELAXED) > 0) {
      // prefer writers to avoid starvation
      (void)sc4(SYS_futex, (long)&l->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, 0);
    } else if (__atomic_load_n(&l->wait_r, __ATOMIC_RELAXED) > 0) {
      (void)sc4(SYS_futex, (long)&l->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 0x7fffffff, 0);
    }
    return 0;
  }
  if (s > 0) {
    int ns = s - 1;
    __atomic_store_n(&l->state, ns, __ATOMIC_RELEASE);
    if (ns == 0) {
      // no readers left; wake one writer if any
      if (__atomic_load_n(&l->wait_w, __ATOMIC_RELAXED) > 0)
        (void)sc4(SYS_futex, (long)&l->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, 0);
      else if (__atomic_load_n(&l->wait_r, __ATOMIC_RELAXED) > 0)
        (void)sc4(SYS_futex, (long)&l->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 0x7fffffff, 0);
    }
    return 0;
  }
  return 22; // EINVAL
}
