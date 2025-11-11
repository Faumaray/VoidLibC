export module rt.snprintf;

import rt.format;  // must provide: long vprint(bool(*sink)(void*, const char*, unsigned long),
//                                   void* cookie, const char* fmt, __builtin_va_list ap)

namespace rt::printf {

    export struct BufSink {
        char*          dst;
        unsigned long  cap;   // total capacity (n)
        unsigned long  len;   // bytes actually stored (<= cap ? cap-1 : cap-1)
    };

    // Always return true so rt.format::vprint can count the full "would-have-written" length.
    static bool sink_write(void* cookie, const char* s, unsigned long n) {
        auto* bs = (BufSink*)cookie;
        if (!bs || !bs->dst || bs->cap == 0) return true;   // counting only, no writes

        // space left for payload keeping room for the final NUL
        unsigned long room = (bs->len < bs->cap) ? (bs->cap - 1 - bs->len) : 0;
        unsigned long take = n < room ? n : room;
        for (unsigned long i = 0; i < take; ++i) bs->dst[bs->len++] = s[i];
        return true;
    }

    // Returns number of chars that would have been written (excluding NUL), even if truncated.
    export int vsnprintf(char* dst, unsigned long n, const char* fmt, __builtin_va_list ap) noexcept {
        BufSink bs{ dst, n, 0 };
        long total = rt::format::vprint(&sink_write, (void*)&bs, fmt, ap); // total "would-have-written"
        // NUL-terminate when possible per POSIX
        if (dst && n) {
            if (bs.len >= n) bs.len = n - 1;
            dst[bs.len] = '\0';
        }
        return (int)total;  // If you want EOVERFLOW handling for >INT_MAX, clamp here.
    }

    export int snprintf(char* dst, unsigned long n, const char* fmt, ...) noexcept {
        __builtin_va_list ap;
        __builtin_va_start(ap, fmt);
        int r = vsnprintf(dst, n, fmt, ap);
        __builtin_va_end(ap);
        return r;
    }
}
