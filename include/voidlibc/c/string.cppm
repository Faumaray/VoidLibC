export module voidlibc.c.string;
import voidlibc.core.types;

export extern "C" {
    void* memcpy(void* dst, const void* src, size_t n);
    void* memmove(void* dst, const void* src, size_t n);
    void* memset(void* dst, int c, size_t n);
    int   strcmp(const char* a, const char* b);
    size_t strlen(const char* s);
}
