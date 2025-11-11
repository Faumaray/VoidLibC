import voidlibc.c.io;
import voidlibc.thread.once;
import voidlibc.thread.pthread;
import voidlibc.thread.tlskeys;

static pthread_once_t g_once{};
static pthread_key_t  g_key;

static void make_key(){
    pthread_key_create(&g_key, /*dtor*/ [](void* /*p*/){
      // print 'x' when destructor runs
      const char x[] = "x";
      write(1, x, 1);
      // free not available here by contract; we just signal
    });
}

static void* worker(void*){
    pthread_once(&g_once, &make_key);
    // store a per-thread pointer-ish cookie
    pthread_setspecific(g_key, (void*)0x42);
    // check readback
    void* v = pthread_getspecific(g_key);
    if (v != (void*)0x42) return (void*)1;
    const char t[] = "t";
    write(1, t, 1);
    return (void*)0;
}

extern "C" int main(int, char**, char**){
    const char s[] = "once+keys\n";
    write(1, s, sizeof(s)-1);

    void* th[3];
    for (int i=0;i<3;++i) pthread_create(&th[i], nullptr, &worker, nullptr);
    for (int i=0;i<3;++i) pthread_join(th[i], nullptr);

    // main thread also uses the key; destructor should run at process exit
    pthread_once(&g_once, &make_key);
    pthread_setspecific(g_key, (void*)0x1234);

    const char nl[] = "\n";
    write(1, nl, 1);
    return 0;
}
