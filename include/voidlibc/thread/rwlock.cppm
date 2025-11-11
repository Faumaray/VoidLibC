export module voidlibc.thread.rwlock;
export import voidlibc.core.types;

export extern "C" {
    struct pthread_rwlock_t {
        int state;    // >=0: reader count; -1: writer holds
        int wait_w;   // waiting writers
        int wait_r;   // waiting readers
        int pad;
    };
    using pthread_rwlockattr_t = void*; // ignored

    int pthread_rwlock_init(pthread_rwlock_t*, const pthread_rwlockattr_t*) noexcept;
    int pthread_rwlock_destroy(pthread_rwlock_t*) noexcept;
    int pthread_rwlock_rdlock(pthread_rwlock_t*) noexcept;
    int pthread_rwlock_tryrdlock(pthread_rwlock_t*) noexcept;
    int pthread_rwlock_wrlock(pthread_rwlock_t*) noexcept;
    int pthread_rwlock_trywrlock(pthread_rwlock_t*) noexcept;
    int pthread_rwlock_unlock(pthread_rwlock_t*) noexcept;
}
