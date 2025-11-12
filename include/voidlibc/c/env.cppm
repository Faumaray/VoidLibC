export module voidlibc.c.env;
export import voidlibc.core.types;
export import voidlibc.core.environ;

export extern "C" {
    char* getenv(const char* name) noexcept;
    int   setenv(const char* name, const char* value, int overwrite) noexcept;
    int   unsetenv(const char* name) noexcept;
    int   putenv(char* string) noexcept; // takes ownership of "name=val" C-string
}
