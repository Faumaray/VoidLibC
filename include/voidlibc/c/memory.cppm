export module voidlibc.c.memory;
import voidlibc.core.types;

export extern "C" {
    void* malloc(size_t n) noexcept;
    void  free(void* p) noexcept;
    void* realloc(void* p, size_t n) noexcept;
    void* aligned_alloc(size_t align, size_t n) noexcept;
    int   posix_memalign(void** out, size_t align, size_t n) noexcept;
}
