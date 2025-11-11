import voidlibc.c.io;

extern "C" int main(int, char**, char**){
    const char msg[] = "Hello from VoidLibC (M0)\n";
    write(1, msg, sizeof(msg)-1);
    return 0;
}
