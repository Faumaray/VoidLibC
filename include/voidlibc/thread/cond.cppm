export module voidlibc.thread.cond;
export import voidlibc.core.types;
export import voidlibc.thread.mutex;

export extern "C" {
    struct pthread_cond_t { int seq; int waiters; int pad[2]; };
    using  pthread_condattr_t = void*; // ignored

    int pthread_cond_init(pthread_cond_t* c, const pthread_condattr_t* attr) noexcept;
    int pthread_cond_destroy(pthread_cond_t* c) noexcept;
    int pthread_cond_signal(pthread_cond_t* c) noexcept;
    int pthread_cond_broadcast(pthread_cond_t* c) noexcept;
    int pthread_cond_wait(pthread_cond_t* c, pthread_mutex_t* m) noexcept;
    int pthread_cond_timedwait(pthread_cond_t* c, pthread_mutex_t* m, const struct timespec* abstime) noexcept;
}
