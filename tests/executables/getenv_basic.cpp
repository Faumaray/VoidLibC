import voidlibc.c.io;
import voidlibc.c.env;

extern "C" int main(int, char**, char**) {
    const char* p = getenv("PATH");
    if (p) {
        static const char ok[] = "getenv ok\n";
        write(1, ok, sizeof(ok)-1);
        return 0;
    }
    static const char er[] = "getenv fail\n";
    write(2, er, sizeof(er)-1);
    return 1;
}
