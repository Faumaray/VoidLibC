export module rt.mem;

export namespace rt::mem {
    void* memcpy(void* dst, const void* src, __SIZE_TYPE__ n) noexcept {
        auto* d = static_cast<unsigned char*>(dst);
        auto* s = static_cast<const unsigned char*>(src);
        for (__SIZE_TYPE__ i = 0; i < n; ++i) d[i] = s[i];
        return dst;
    }

    void* memmove(void* dst, const void* src, __SIZE_TYPE__ n) noexcept {
        auto* d = static_cast<unsigned char*>(dst);
        auto* s = static_cast<const unsigned char*>(src);
        if (d < s) {
            for (__SIZE_TYPE__ i = 0; i < n; ++i) d[i] = s[i];
        } else if (d > s) {
            for (__SIZE_TYPE__ i = n; i > 0; --i) d[i-1] = s[i-1];
        }
        return dst;
    }

    void* memset(void* dst, int c, __SIZE_TYPE__ n) noexcept {
        auto* d = static_cast<unsigned char*>(dst);
        for (__SIZE_TYPE__ i = 0; i < n; ++i) d[i] = static_cast<unsigned char>(c);
        return dst;
    }
}
