import voidlibc.c.memory;
import voidlibc.c.io;

int main(char**, char**){
    void* p = malloc(1000);                  // small
    if (!p) return 1;
    void* q = realloc(p, 8000);              // small→medium/large move
    if (!q) return 2;
    free(q);

    void* a = aligned_alloc(4096, 8192);     // large + header-just-before
    if (!a) return 3;
    free(a);

    static const char ok[] = "alloc ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
