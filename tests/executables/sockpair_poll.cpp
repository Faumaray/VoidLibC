import voidlibc.c.io;
import voidlibc.c.net;
import voidlibc.c.poll;

extern "C" int main(int, char**, char**) {
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) {
        static const char er[] = "socketpair failed\n";
        write(2, er, sizeof(er)-1);
        return 1;
    }

    static const char X[] = "X";
    if (send(sv[0], X, 1, 0) != 1) {
        static const char er[] = "send failed\n";
        write(2, er, sizeof(er)-1);
        return 2;
    }

    pollfd p{ sv[1], POLLIN, 0 };
    int pr = poll(&p, 1, 1000);
    if (pr <= 0 || !(p.revents & POLLIN)) {
        static const char er[] = "poll failed\n";
        write(2, er, sizeof(er)-1);
        return 3;
    }

    char b[4];
    long n = recv(sv[1], b, 1, 0);
    if (n != 1 || b[0] != 'X') {
        static const char er[] = "recv mismatch\n";
        write(2, er, sizeof(er)-1);
        return 4;
    }

    (void)close(sv[0]);
    (void)close(sv[1]);

    static const char ok[] = "sockpair_poll ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
