module voidlibc.thread.tlskeys;

import voidlibc.core.types;
import voidlibc.core.tls;
import voidlibc.sys.linux;

using namespace vl::sys::linux;

static constexpr int MAX_KEYS = 128;
// Global registry of destructors; 0=free, 1=used
static int               g_used[MAX_KEYS];
static void (*g_dtors[MAX_KEYS])(void*);

// A very small futex-based global lock (0=unlocked,1=locked)
static int g_lock = 0;
static inline void glock(){
  int exp=0;
  if (__atomic_compare_exchange_n(&g_lock, &exp, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) return;
  for(;;){
    int v = __atomic_load_n(&g_lock, __ATOMIC_RELAXED);
    if (v==0){
      int z=0;
      if (__atomic_compare_exchange_n(&g_lock, &z, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) return;
      continue;
    }
    (void)sc4(SYS_futex, (long)&g_lock, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, 1, 0);
  }
}
static inline void gunlock(){
  __atomic_store_n(&g_lock, 0, __ATOMIC_RELEASE);
  (void)sc4(SYS_futex, (long)&g_lock, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, 0);
}

extern "C" int pthread_key_create(pthread_key_t* key, void (*destructor)(void*)) noexcept {
  glock();
  for (int i=0;i<MAX_KEYS;++i){
    if (!__atomic_load_n(&g_used[i], __ATOMIC_RELAXED)) {
      __atomic_store_n(&g_used[i], 1, __ATOMIC_RELAXED);
      g_dtors[i] = destructor;
      *key = i;
      gunlock();
      return 0;
    }
  }
  gunlock();
  return 11; // EAGAIN (no more keys)
}

extern "C" int pthread_key_delete(pthread_key_t key) noexcept {
  if (key < 0 || key >= MAX_KEYS) return 22; // EINVAL
  glock();
  g_dtors[key] = nullptr;
  __atomic_store_n(&g_used[key], 0, __ATOMIC_RELAXED);
  gunlock();
  return 0;
}

extern "C" int pthread_setspecific(pthread_key_t key, const void* ptr) noexcept {
  if (key < 0 || key >= MAX_KEYS) return 22; // EINVAL
  void** slots = __vl_tls_key_slots();
  if (!slots) return 38; // ENOSYS (no TLS; shouldn't happen in our runtime)
  slots[key] = (void*)ptr;
  return 0;
}

extern "C" void* pthread_getspecific(pthread_key_t key) noexcept {
  if (key < 0 || key >= MAX_KEYS) return (void*)0;
  void** slots = __vl_tls_key_slots();
  if (!slots) return (void*)0;
  return slots[key];
}

// Run destructors per POSIX: iterate multiple times to handle re-set in dtor
extern "C" void __vl_tls_run_dtors() noexcept {
  void** slots = __vl_tls_key_slots();
  if (!slots) return;
  for (int pass=0; pass<4; ++pass) {
    int did = 0;
    glock(); // protect reading g_dtors while running
    for (int i=0;i<MAX_KEYS;++i){
      void* p = slots[i];
      if (!p) continue;
      void (*d)(void*) = g_dtors[i];
      if (!d) continue;
      slots[i] = (void*)0;
      did = 1;
      // Release global lock while running user code
      gunlock();
      d(p);
      glock();
    }
    gunlock();
    if (!did) break;
  }
}
