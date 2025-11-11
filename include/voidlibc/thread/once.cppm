export module voidlibc.thread.once;
export import voidlibc.core.types;

export extern "C" {
    struct pthread_once_t { int state; int pad; }; // 0=NEEDS, 1=DOING, 2=DONE
    using  pthread_once_routine = void(*)();

    int pthread_once(pthread_once_t* ctrl, pthread_once_routine init) noexcept;
}
