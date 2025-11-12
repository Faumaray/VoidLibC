import voidlibc.c.io;
import voidlibc.c.proc;
import voidlibc.c.exit;

extern "C" int main(int, char**, char**) {
    int pid = fork();
    if (pid < 0) {
        static const char er[] = "fork fail\n";
        write(2, er, sizeof(er)-1);
        return 1;
    }
    if (pid == 0) {
        static const char c[] = "C\n";
        write(1, c, sizeof(c)-1);
        _Exit(42);
    }
    int st = 0;
    int w = waitpid(pid, &st, 0);
    if (w != pid) {
        static const char er[] = "waitpid fail\n";
        write(2, er, sizeof(er)-1);
        return 2;
    }
    int code = (st >> 8) & 0xff; // WEXITSTATUS
    if (code != 42) {
        static const char er[] = "status mismatch\n";
        write(2, er, sizeof(er)-1);
        return 3;
    }
    static const char ok[] = "fork_wait ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
