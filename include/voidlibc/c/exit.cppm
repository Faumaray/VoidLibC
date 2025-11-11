export module voidlibc.c.exit;
export import voidlibc.core.types;

export extern "C" {
    int  atexit(void (*func)(void)) noexcept;
    [[noreturn]] void exit(int status) noexcept;
    [[noreturn]] void _Exit(int status) noexcept;
    [[noreturn]] void abort() noexcept;
}
