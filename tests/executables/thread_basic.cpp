import voidlibc.c.io;
import voidlibc.thread.pthread;

static void itoa_u(unsigned long x, char* out) {
    char buf[32]; int i=0;
    if (x==0){ out[0]='0'; out[1]=0; return; }
    while(x){ buf[i++] = char('0' + (x%10)); x/=10; }
    for(int j=0;j<i;++j) {
        out[j]=buf[i-1-j];
        out[i]=0;
    }
}

static void* worker(void* p){
    const char* s = (const char*)p;
    write(1, s, 2); // "T\n"
    return (void*)42;
}

extern "C" int main(int, char**, char**){
    static const char hi[] = "main\n";
    write(1, hi, sizeof(hi)-1);

    pthread_t th;
    static const char tn[] = "T\n";
    if (pthread_create(&th, nullptr, &worker, (void*)tn) != 0) return 1;

    void* rv = nullptr;
    if (pthread_join(th, &rv) != 0) return 2;

    char msg[32] = "rv=";
    itoa_u((unsigned long)rv, msg+3);
    static const char nl[] = "\n";
    write(1, msg, 3); write(1, msg+3, 2); write(1, nl, 1);
    return 0;
}
