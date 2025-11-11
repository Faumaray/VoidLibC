export module rt.string;

export namespace rt::string {
    __SIZE_TYPE__ strlen(const char* s) noexcept {
        __SIZE_TYPE__ n = 0;
        while (s[n]) ++n;
        return n;
    }

    int strcmp(const char* a, const char* b) noexcept {
        for (; *a && (*a == *b); ++a, ++b) {}
        return static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
    }
}
