export module voidlibc.c.signal;
export import voidlibc.core.types;

export extern "C" {

// --- basic signal numbers (subset) ---
inline constexpr int SIGINT   = 2;
inline constexpr int SIGQUIT  = 3;
inline constexpr int SIGILL   = 4;
inline constexpr int SIGABRT  = 6;
inline constexpr int SIGFPE   = 8;
inline constexpr int SIGKILL  = 9;
inline constexpr int SIGSEGV  = 11;
inline constexpr int SIGPIPE  = 13;
inline constexpr int SIGALRM  = 14;
inline constexpr int SIGTERM  = 15;
inline constexpr int SIGCHLD  = 17;
inline constexpr int SIGCONT  = 18;
inline constexpr int SIGSTOP  = 19;
inline constexpr int SIGTSTP  = 20;
inline constexpr int SIGTTIN  = 21;
inline constexpr int SIGTTOU  = 22;
inline constexpr int SIGBUS   = 7;
inline constexpr int SIGUSR1  = 10;
inline constexpr int SIGUSR2  = 12;

// handler type
using sighandler_t = void(*)(int);

// NOTE: C++ can’t constexpr-cast 1 → function pointer. Expose callable tokens.
inline sighandler_t SIG_DFL() noexcept { return (sighandler_t)0; }
inline sighandler_t SIG_IGN() noexcept { return (sighandler_t)(unsigned long)1; }

// --- sigset_t + helpers (x86-64: 1024 bits) ---
struct sigset_t { unsigned long __bits[16]; }; // 128 bytes

inline void sigemptyset(sigset_t* s) {
  for (int i = 0; i < 16; ++i) s->__bits[i] = 0UL;
}
inline void sigfillset(sigset_t* s) {
  for (int i = 0; i < 16; ++i) s->__bits[i] = ~0UL;
}
inline int sigaddset(sigset_t* s, int sig) {
  if (sig <= 0 || sig > 1024) return -1;
  int i = (sig - 1) / 64, b = (sig - 1) % 64;
  s->__bits[i] |= (1UL << b);
  return 0;
}
inline int sigdelset(sigset_t* s, int sig) {
  if (sig <= 0 || sig > 1024) return -1;
  int i = (sig - 1) / 64, b = (sig - 1) % 64;
  s->__bits[i] &= ~(1UL << b);
  return 0;
}
inline int sigismember(const sigset_t* s, int sig) {
  if (sig <= 0 || sig > 1024) return 0;
  int i = (sig - 1) / 64, b = (sig - 1) % 64;
  return (int)((s->__bits[i] >> b) & 1UL);
}

// --- sigaction flags (Linux subset) ---
inline constexpr unsigned SA_NOCLDSTOP = 0x00000001;
inline constexpr unsigned SA_NOCLDWAIT = 0x00000002;
inline constexpr unsigned SA_SIGINFO   = 0x00000004;
inline constexpr unsigned SA_RESTART   = 0x10000000;
inline constexpr unsigned SA_NODEFER   = 0x40000000;
inline constexpr unsigned SA_RESETHAND = 0x80000000;

struct sigaction_t {
  sighandler_t  sa_handler;   // or sa_sigaction when SA_SIGINFO (we keep simple handler)
  unsigned long sa_flags;
  void         (*sa_restorer)();
  sigset_t      sa_mask;
};

// how for sigprocmask
inline constexpr int SIG_BLOCK   = 0;
inline constexpr int SIG_UNBLOCK = 1;
inline constexpr int SIG_SETMASK = 2;

// Convenience helpers (so callers can write sigaction_ignore(&sa) if desired)
inline void sigaction_default(struct sigaction_t* sa) {
  sa->sa_handler  = SIG_DFL();
  sa->sa_flags   &= ~SA_SIGINFO;
  sa->sa_restorer = (void(*)())0;
}
inline void sigaction_ignore(struct sigaction_t* sa) {
  sa->sa_handler  = SIG_IGN();
  sa->sa_flags   &= ~SA_SIGINFO;
  sa->sa_restorer = (void(*)())0;
}

// API
int sigaction(int sig, const struct sigaction_t* act, struct sigaction_t* oact) noexcept;
int sigprocmask(int how, const sigset_t* set, sigset_t* oldset) noexcept;
int raise(int sig) noexcept;
unsigned alarm(unsigned seconds) noexcept;

} // extern "C"
