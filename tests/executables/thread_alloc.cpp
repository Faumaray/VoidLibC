import voidlibc.c.io;
import voidlibc.thread.pthread;
import voidlibc.thread.mutex;
import voidlibc.thread.cond;
import voidlibc.c.time;
import voidlibc.c.memory;

struct Shared {
    pthread_mutex_t m;
    pthread_cond_t  c;
    int start;
    int done;
} g;

static void put(const char* s){ write(1, s, 1); }

static void* worker(void*){
    // wait for start
    pthread_mutex_lock(&g.m);
    while(!g.start) pthread_cond_wait(&g.c, &g.m);
    pthread_mutex_unlock(&g.m);

    // alloc/free churn
    for (int i=0;i<2000;++i){
        void* p = malloc(256 + (i&63));
        void* q = malloc(8192);
        p = realloc(p, 1024);
        free(p); free(q);
    }

    pthread_mutex_lock(&g.m);
    g.done++;
    pthread_mutex_unlock(&g.m);
    put("d"); // mark done
    return 0;
}

extern "C" int main(int, char**, char**){
    pthread_mutex_init(&g.m, nullptr);
    pthread_cond_init(&g.c, nullptr);
    g.start = 0; g.done = 0;

    const int N=4;
    void* th[N];
    for (int i=0;i<N;++i) {
        if (pthread_create(&th[i], nullptr, &worker, nullptr) != 0) return 1;
    }

    pthread_mutex_lock(&g.m);
    g.start = 1;
    pthread_cond_broadcast(&g.c);
    pthread_mutex_unlock(&g.m);

    for (int i=0;i<N;++i) pthread_join(th[i], nullptr);

    static const char ok[] = "\nthread_alloc ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
