import voidlibc.c.io;
import voidlibc.c.fcntl;

extern "C" int main(int, char**, char**) {
    int fds[2];
    if (pipe(fds) != 0) {
        static const char er[] = "pipe fail\n";
        write(2, er, sizeof(er)-1);
        return 1;
    }
    static const char msg[] = "x";
    if (write(fds[1], msg, 1) != 1) {
        static const char er[] = "write fail\n";
        write(2, er, sizeof(er)-1);
        return 2;
    }
    char b[2] = {0,0};
    if (read(fds[0], b, 1) != 1 || b[0] != 'x') {
        static const char er[] = "read mismatch\n";
        write(2, er, sizeof(er)-1);
        return 3;
    }
    close(fds[0]); close(fds[1]);
    static const char ok[] = "pipe_echo ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
