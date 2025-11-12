module voidlibc.c.exit;

import voidlibc.core.types;
import voidlibc.sys.linux;
import voidlibc.core.tls;
import voidlibc.thread.mutex;   // our futex-based mutex
import voidlibc.c.io;           // write()

using namespace vl::sys::linux;


// ---- simple atexit registry
static pthread_mutex_t g_atexit_lock{0,0};
static void (*g_atexit_funcs[128])(void);
static int g_atexit_n = 0;

// Called by exit(); safe to call multiple times (we guard with count)
static inline void run_atexit() {
    // LIFO
    for (;;) {
        pthread_mutex_lock(&g_atexit_lock);
        int i = g_atexit_n;
        if (i <= 0) { pthread_mutex_unlock(&g_atexit_lock); break; }
        void (*f)(void) = g_atexit_funcs[i-1];
        g_atexit_n = i-1;
        pthread_mutex_unlock(&g_atexit_lock);
        if (f) f();
    }
}

extern "C" int atexit(void (*func)(void)) noexcept {
    pthread_mutex_lock(&g_atexit_lock);
    int ok = 0;
    if (g_atexit_n < (int)(sizeof(g_atexit_funcs)/sizeof(g_atexit_funcs[0]))) {
        g_atexit_funcs[g_atexit_n++] = func;
        ok = 1;
    }
    pthread_mutex_unlock(&g_atexit_lock);
    return ok ? 0 : 12; // ENOMEM if table full
}

// Provided by TLS-keys module
extern "C" void __vl_tls_run_dtors() noexcept;

extern "C" [[noreturn]] void exit(int status) noexcept {
    run_atexit();
    __vl_tls_run_dtors();
    // terminate all threads in the process
    sc1(SYS_exit_group, status);
    __builtin_unreachable();
}

extern "C" [[noreturn]] void _Exit(int status) noexcept {
    sc1(SYS_exit_group, status);
    __builtin_unreachable();
}

extern "C" [[noreturn]] void abort() noexcept {
    // Minimal abort: write message, then exit with 134
    static const char msg[] = "Aborted\n";
    write(2, msg, sizeof(msg)-1);
    sc1(SYS_exit_group, 134);
    __builtin_unreachable();
}
