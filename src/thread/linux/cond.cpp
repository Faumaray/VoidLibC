module voidlibc.thread.cond;

import voidlibc.core.types;
import voidlibc.core.tls;
import voidlibc.sys.linux;
import voidlibc.c.time;      // clock_gettime

using namespace vl::sys::linux;

extern "C" int pthread_cond_init(pthread_cond_t* c, const pthread_condattr_t*) noexcept {
  c->seq = 0; c->waiters = 0; c->pad[0]=c->pad[1]=0; return 0;
}
extern "C" int pthread_cond_destroy(pthread_cond_t*) noexcept { return 0; }

extern "C" int pthread_cond_signal(pthread_cond_t* c) noexcept {
  int w = __atomic_load_n(&c->waiters, __ATOMIC_RELAXED);
  if (w > 0) {
    __atomic_fetch_add(&c->seq, 1, __ATOMIC_RELEASE);
    (void)sc4(SYS_futex, (long)&c->seq, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, 0);
  }
  return 0;
}

extern "C" int pthread_cond_broadcast(pthread_cond_t* c) noexcept {
  int w = __atomic_load_n(&c->waiters, __ATOMIC_RELAXED);
  if (w > 0) {
    __atomic_fetch_add(&c->seq, 1, __ATOMIC_RELEASE);
    (void)sc4(SYS_futex, (long)&c->seq, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, w, 0);
  }
  return 0;
}

extern "C" int pthread_cond_wait(pthread_cond_t* c, pthread_mutex_t* m) noexcept {
  int s = __atomic_load_n(&c->seq, __ATOMIC_ACQUIRE);
  __atomic_fetch_add(&c->waiters, 1, __ATOMIC_ACQ_REL);

  // unlock mutex before sleeping
  pthread_mutex_unlock(m);

  // sleep until seq changes
  for (;;) {
    long r = sc4(SYS_futex, (long)&c->seq, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, s, 0);
    (void)r;
    int s2 = __atomic_load_n(&c->seq, __ATOMIC_ACQUIRE);
    if (s2 != s) break; // woken or spuriously awakened but seq changed
    // if spuriously woken without seq change, loop and sleep again
  }

  __atomic_fetch_sub(&c->waiters, 1, __ATOMIC_ACQ_REL);
  // relock
  pthread_mutex_lock(m);
  return 0;
}

extern "C" int pthread_cond_timedwait(pthread_cond_t* c, pthread_mutex_t* m, const struct timespec* abstime) noexcept {
  // Convert absolute CLOCK_REALTIME to relative timeout for futex
  timespec now{};
  if (clock_gettime(0 /*CLOCK_REALTIME*/, &now) < 0) return -1;
  long sec = abstime->tv_sec - now.tv_sec;
  long nsec = abstime->tv_nsec - now.tv_nsec;
  if (nsec < 0) { nsec += 1000000000L; sec -= 1; }
  if (sec < 0) { // already timed out
    return 110;  // ETIMEDOUT
  }
  timespec rel{ sec, nsec };

  int s = __atomic_load_n(&c->seq, __ATOMIC_ACQUIRE);
  __atomic_fetch_add(&c->waiters, 1, __ATOMIC_ACQ_REL);

  pthread_mutex_unlock(m);

  for (;;) {
    long r = sc4(SYS_futex, (long)&c->seq, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, s, (long)&rel);
    if (r == -110) { // -ETIMEDOUT
      __atomic_fetch_sub(&c->waiters, 1, __ATOMIC_ACQ_REL);
      pthread_mutex_lock(m);
      return 110; // ETIMEDOUT
    }
    int s2 = __atomic_load_n(&c->seq, __ATOMIC_ACQUIRE);
    if (s2 != s) break;
  }

  __atomic_fetch_sub(&c->waiters, 1, __ATOMIC_ACQ_REL);
  pthread_mutex_lock(m);
  return 0;
}
