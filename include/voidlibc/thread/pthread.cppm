export module voidlibc.thread.pthread;
export import voidlibc.core.types;

export extern "C" {
    using pthread_start_routine = void*(*)(void*);
    using pthread_t = void*;           // opaque (points to our control block)
    using pthread_attr_t = void*;      // (unused for now)

    int pthread_create(pthread_t* t, const pthread_attr_t attr,
                       pthread_start_routine start, void* arg) noexcept;

    int pthread_join(pthread_t t, void** retval) noexcept;
}
