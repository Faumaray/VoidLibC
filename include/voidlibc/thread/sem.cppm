export module voidlibc.thread.sem;
export import voidlibc.core.types;

export extern "C" {
    struct sem_t { int val; int pad; };
    int sem_init(sem_t*, int pshared, unsigned int value) noexcept;
    int sem_destroy(sem_t*) noexcept;
    int sem_post(sem_t*) noexcept;
    int sem_wait(sem_t*) noexcept;
    int sem_trywait(sem_t*) noexcept;
    int sem_timedwait(sem_t*, const struct timespec* abstime) noexcept;
}
