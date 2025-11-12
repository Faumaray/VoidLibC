module voidlibc.c.signal;

import voidlibc.core.types;
import voidlibc.core.tls;     // vl_errno()
import voidlibc.sys.linux;

using namespace vl::sys::linux;

// Kernel-compatible layout for rt_sigaction
struct k_sigaction {
    void (*handler)(int);
    unsigned long flags;
    void (*restorer)();
    sigset_t mask;              // userland 1024-bit, kernel copies only 'sigsetsize' bytes
};

// Provided by rt_sigreturn_linux.S
extern "C" void __vl_rt_sigreturn();

// SA_RESTORER bit on x86-64
static inline constexpr unsigned long SA_RESTORER = 0x04000000UL;
// x86-64 expects sigsetsize = sizeof(unsigned long) == 8
static inline constexpr long K_SIGSET_SIZE = (long)sizeof(unsigned long);

extern "C" int sigaction(int sig, const struct sigaction_t* act, struct sigaction_t* oact) noexcept {
    k_sigaction kin{}, kout{};
    k_sigaction* in = nullptr;
    if (act) {
        kin.handler  = act->sa_handler;
        kin.flags    = (act->sa_flags | SA_RESTORER);
        kin.restorer = __vl_rt_sigreturn;
        kin.mask     = act->sa_mask;
        in = &kin;
    }
    long r = sc4(SYS_rt_sigaction, sig, (long)in, (long)(oact ? &kout : 0), K_SIGSET_SIZE);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    if (oact) {
        oact->sa_handler  = kout.handler;
        oact->sa_flags    = kout.flags;
        oact->sa_restorer = kout.restorer;
        oact->sa_mask     = kout.mask;
    }
    return 0;
}

extern "C" int sigprocmask(int how, const sigset_t* set, sigset_t* oldset) noexcept {
    long r = sc4(SYS_rt_sigprocmask, how, (long)set, (long)oldset, K_SIGSET_SIZE);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" int raise(int sig) noexcept {
    long pid = sc0(SYS_getpid);
    if (pid < 0) { vl_errno() = (int)(-pid); return -1; }
    long r = sc2(SYS_kill, pid, sig);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" unsigned alarm(unsigned seconds) noexcept {
    long r = sc1(SYS_alarm, (long)seconds);
    if (r < 0) { vl_errno() = (int)(-r); return 0; }
    return (unsigned)r;
}
