export module rt.signals;

import rt.errno;
import rt.types;

#if defined(__linux__) && defined(__x86_64__)
  import rt.linux.sysnums_proc;
  import rt.linux.sysnums_thread; // tgkill
#endif

namespace rt::sig {
  // Kernel-sized mask for x86-64: 64 signals → 64-bit mask (8 bytes)
  export struct sigset_t { rt_u64 mask; };

  // how
  export constexpr int SIG_BLOCK   = 0;
  export constexpr int SIG_UNBLOCK = 1;
  export constexpr int SIG_SETMASK = 2;

  // common signals (subset)
  export constexpr int SIGPIPE = 13;
  export constexpr int SIGTERM = 15;
  export constexpr int SIGINT  = 2;
  export constexpr int SIGKILL = 9;
  export constexpr int SIGHUP  = 1;
  export constexpr int SIGCHLD = 17;

  export using sighandler_t = void(*)(int);

  // Kernel rt_sigaction layout on x86-64
  export struct k_sigaction {
    sighandler_t   sa_handler;
    rt_u64         sa_flags;
    void         (*sa_restorer)(void); // may be null; SA_RESTORER not required here
    sigset_t       sa_mask;
  };

  export int sigaction(int signum, const k_sigaction* act, k_sigaction* old) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r;
    // size of *kernel* sigset_t = 8 bytes on x86-64
    register long r10 __asm__("r10") = (long)sizeof(sigset_t);
    asm volatile("syscall"
      : "=a"(r)
      : "a"(rt::linux::__NR_rt_sigaction),
        "D"(signum), "S"(act), "d"(old), "r"(r10)
      : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int sigprocmask(int how, const sigset_t* set, sigset_t* old) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r;
    register long r10 __asm__("r10") = (long)sizeof(sigset_t); // 8 bytes
    asm volatile("syscall"
      : "=a"(r)
      : "a"(rt::linux::__NR_rt_sigprocmask),
        "D"(how), "S"(set), "d"(old), "r"(r10)
      : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int tgkill(int tgid, int tid, int sig) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r;
    asm volatile("syscall"
      : "=a"(r)
      : "a"(rt::linux::__NR_tgkill),
        "D"(tgid), "S"(tid), "d"(sig)
      : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }
}
