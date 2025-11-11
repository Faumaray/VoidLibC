export module rt.errno;

namespace rt {
    // POSIX-like errno for freestanding land.
    // Thread-local by default; adjust if you build threads.
    static int errno_value = 0;

    export int* errno_ptr() noexcept { return &errno_value; }
    export void set_errno(int e) noexcept { errno_value = e; }
    export int  get_errno() noexcept { return errno_value; }
    export extern "C" int* __errno_location() noexcept { return &errno_value; }
}
