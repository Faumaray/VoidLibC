export module voidlibc.thread.tlskeys;
export import voidlibc.core.types;

export extern "C" {
    using pthread_key_t = int;

    int pthread_key_create(pthread_key_t* key, void (*destructor)(void*)) noexcept;
    int pthread_key_delete(pthread_key_t key) noexcept;

    int   pthread_setspecific(pthread_key_t key, const void* ptr) noexcept;
    void* pthread_getspecific(pthread_key_t key) noexcept;

    // Called by runtime on thread exit / process exit
    void __vl_tls_run_dtors() noexcept;
}
