import voidlibc.c.io;
import voidlibc.c.dirent;

extern "C" int main(int, char**, char**) {
    DIR* d = opendir(".");
    if (!d) {
        static const char er[] = "opendir failed\n";
        write(2, er, sizeof(er)-1);
        return 1;
    }
    int shown = 0;
    for (;;) {
        dirent* e = readdir(d);
        if (!e) break;
        // print name + '\n' (truncate names longer than 255; our struct ensures NUL)
        const char* s = e->d_name;
        const char* p = s;
        while (*p) ++p;
        write(1, s, (unsigned long)(p - s));
        static const char nl[] = "\n";
        write(1, nl, 1);
        if (++shown > 20) break; // keep output short
    }
    closedir(d);
    static const char ok[] = "ls_min ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
