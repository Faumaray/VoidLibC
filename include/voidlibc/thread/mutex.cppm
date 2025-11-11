export module voidlibc.thread.mutex;
export import voidlibc.core.types;

export extern "C" {
    struct pthread_mutex_t { int state; int pad; };
    using  pthread_mutexattr_t = void*; // ignored for now

    int pthread_mutex_init(pthread_mutex_t* m, const pthread_mutexattr_t* attr) noexcept;
    int pthread_mutex_destroy(pthread_mutex_t* m) noexcept;
    int pthread_mutex_lock(pthread_mutex_t* m) noexcept;
    int pthread_mutex_trylock(pthread_mutex_t* m) noexcept;
    int pthread_mutex_unlock(pthread_mutex_t* m) noexcept;
}
