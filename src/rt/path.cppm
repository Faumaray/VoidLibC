export module rt.path;

import rt.mem;
import rt.string;

namespace rt {
    // returns ptr to base name within given C-string (no allocation)
    export const char* basename(const char* path) noexcept {
        if (!path || !*path) return path ? path : "";
        const char* p = path;
        const char* base = path;
        for (; *p; ++p) if (*p == '/') base = p + 1;
        return *base ? base : base - 1; // if path ends with '/', keep last segment as ""
    }

    // writes dirname of path into out (buffer size out_cap). returns length written, or -1 if too small.
    export long dirname(const char* path, char* out, unsigned long out_cap) noexcept {
        if (!path || !*path) { if (out_cap) { out[0] = '.'; if (out_cap>1) out[1]=0; } return (out_cap>=2)?1:-1; }
        const char* end = path;
        for (; *end; ++end) {}
        // strip trailing slashes
        while (end > path && end[-1] == '/') --end;
        // find previous slash
        const char* slash = end;
        while (slash > path && slash[-1] != '/') --slash;
        unsigned long len = (slash == path) ? 1 : (unsigned long)(slash - path - (path[0]=='/'?0:0));
        if (slash == path) { // root or no slash
            if (out_cap < 2) return -1;
            out[0] = '/'; out[1] = 0; return 1;
        }
        if (len >= out_cap) return -1;
        for (unsigned long i = 0; i < len; ++i) out[i] = path[i];
        out[len] = 0;
        return static_cast<long>(len);
    }

    // join a + "/" + b into out; handles slashes simply
    export long join(const char* a, const char* b, char* out, unsigned long cap) noexcept {
        unsigned long la = 0; while (a && a[la]) ++la;
        unsigned long lb = 0; while (b && b[lb]) ++lb;
        bool need_sep = la && lb && a[la-1] != '/' && b[0] != '/';
        unsigned long need = la + (need_sep?1:0) + lb + 1;
        if (need > cap) return -1;
        for (unsigned long i = 0; i < la; ++i) out[i] = a[i];
        unsigned long k = la;
        if (need_sep) out[k++] = '/';
        for (unsigned long i = 0; i < lb; ++i) out[k++] = b[i];
        out[k] = 0;
        return static_cast<long>(k);
    }
}
