export module rt.alloc;

import rt.mem;
import rt.panic;

namespace rt::alloc {
    namespace {
        alignas(16) static unsigned char arena[1 << 20]; // 1 MiB demo
        static __SIZE_TYPE__ off = 0;
    }

    export void* malloc(__SIZE_TYPE__ n) noexcept {
        if (n == 0) n = 1;
        const __SIZE_TYPE__ aligned = (n + 15) & ~static_cast<__SIZE_TYPE__>(15);
        if (off + aligned > sizeof(arena)) return nullptr;
        void* p = arena + off;
        off += aligned;
        return p;
    }

    export void free(void*) noexcept {
        // bump-alloc demo: no-op
    }

    // Internal helpers used by the global operators (defined in alloc_ops.cpp)
    export void* operator_new(__SIZE_TYPE__ n) noexcept {
        if (void* p = malloc(n)) return p;
        rt::panic("rt.alloc: out of memory\n");
    }

    export void operator_delete(void* p) noexcept {
        free(p);
    }
}
