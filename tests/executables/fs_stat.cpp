import voidlibc.c.io;
import voidlibc.c.fs;

extern "C" int main(int, char**, char**) {
    stat_t st{};
    if (stat(".", &st) == 0) {
        static const char ok[] = "fs_stat ok\n";
        write(1, ok, sizeof(ok)-1);
        return 0;
    }
    static const char er[] = "fs_stat fail\n";
    write(2, er, sizeof(er)-1);
    return 1;
}
