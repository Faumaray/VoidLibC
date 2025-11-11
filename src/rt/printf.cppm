export module rt.printf;

import rt.format;
import rt.fs;

namespace rt::printf {
    // sink that writes to an fd using rt::write
    inline bool fd_sink(void* cookie, const char* s, unsigned long n) {
        int fd = (int)(long)cookie;
        long w = rt::write(fd, s, n);
        return (w == (long)n);
    }

    export long dprintf(int fd, const char* fmt, ...) {
        __builtin_va_list ap; __builtin_va_start(ap, fmt);
        long r = rt::format::vprint(&fd_sink, (void*)(long)fd, fmt, ap);
        __builtin_va_end(ap);
        return r;
    }
}
