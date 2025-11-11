import voidlibc.c.io;
import voidlibc.c.exit;

static void P(const char* s){ write(1, s, 1); }

static void f1(){ P("1"); }
static void f2(){ P("2"); }
static void f3(){ P("3"); }

extern "C" int main(int, char**, char**){
    static const char s[] = "exit_atexit\n";
    write(1, s, sizeof(s)-1);

    atexit(f1);
    atexit(f2);
    atexit(f3);   // LIFO => 3,2,1

    return 7;     // exit code should be 7 (we won’t print it)
}
