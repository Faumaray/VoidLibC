export module rt.cstring;

namespace rt::cstring {
    export unsigned long strlen(const char* s) noexcept {
        unsigned long n = 0; if (!s) return 0; while (s[n]) ++n; return n;
    }
    export int strcmp(const char* a, const char* b) noexcept {
        while (*a && (*a==*b)) { ++a; ++b; } return (unsigned char)*a - (unsigned char)*b;
    }
    export int strncmp(const char* a, const char* b, unsigned long n) noexcept {
        for (unsigned long i=0; i<n; ++i) { if (a[i]!=b[i] || !a[i] || !b[i]) return (unsigned char)a[i] - (unsigned char)b[i]; }
        return 0;
    }
    export char* strcpy(char* dst, const char* src) noexcept {
        char* d = dst; while ((*d++ = *src++)) {} return dst;
    }
    export char* strncpy(char* dst, const char* src, unsigned long n) noexcept {
        unsigned long i=0; for (; i<n && src[i]; ++i) dst[i]=src[i];
        for (; i<n; ++i) dst[i]=0; return dst;
    }
    export char* stpcpy(char* dst, const char* src) noexcept {
        while (*src) *dst++ = *src++; *dst = 0; return dst;
    }
    export unsigned long strnlen(const char* s, unsigned long max) noexcept {
        unsigned long n=0; while (n<max && s && s[n]) ++n; return n;
    }
}
