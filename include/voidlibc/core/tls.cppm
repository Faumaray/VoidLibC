export module voidlibc.core.tls;

export extern "C" int* __vl_errno_location();
export extern "C" int* __errno_location();

export inline int& vl_errno() { return *__vl_errno_location(); }
export inline void* vl_tls_self() { return __builtin_thread_pointer(); }

// N.B. keep in sync with allocator (NCLASS=32) and keys (MAX_KEYS=128)
static inline constexpr int __vl_heap_classes = 32;
static inline constexpr int __vl_keys_slots   = 128;

// Internal TCB layout (visible only in this module; provide accessors)
struct __vl_TCB {
    void* self;          // must be first
    int   errno_val;
    int   pad0;
    void* heap[__vl_heap_classes];   // per-thread freelist heads
    void* keys[__vl_keys_slots];     // per-thread TLS key slots
};

export inline void** __vl_tls_heap_heads() {
    void* tp = __builtin_thread_pointer();
    if (!tp) return (void**)0;
    return ((__vl_TCB*)tp)->heap;
}

export inline void** __vl_tls_key_slots() {
    void* tp = __builtin_thread_pointer();
    if (!tp) return (void**)0;
    return ((__vl_TCB*)tp)->keys;
}

static int g_fallback_errno = 0;

extern "C" int* __vl_errno_location() {
    void* tp = __builtin_thread_pointer();
    if (!tp) return &g_fallback_errno;
    return &(((__vl_TCB*)tp)->errno_val);
}
extern "C" int* __errno_location() { return __vl_errno_location(); }
