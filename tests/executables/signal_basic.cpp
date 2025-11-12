import voidlibc.c.io;
import voidlibc.c.signal;
import voidlibc.c.proc;

static void handler(int) {
    static const char h[] = "H\n";
    write(1, h, sizeof(h)-1);  // async-signal-safe
}

extern "C" int main(int, char**, char**) {
    sigaction_t sa{};
    sa.sa_handler = handler;
    sa.sa_flags   = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, nullptr) != 0) {
        static const char er[] = "sigaction fail\n";
        write(2, er, sizeof(er)-1);
        return 1;
    }

    // deliver via kill(self)
    if (kill(getpid(), SIGUSR1) != 0) {
        static const char er[] = "kill fail\n";
        write(2, er, sizeof(er)-1);
        return 2;
    }

    static const char ok[] = "signal_basic ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
