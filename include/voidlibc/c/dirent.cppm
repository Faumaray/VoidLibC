export module voidlibc.c.dirent;
export import voidlibc.core.types;

export extern "C" {

    struct dirent {
        unsigned long long d_ino;
        long long          d_off;
        unsigned short     d_reclen;
        unsigned char      d_type;
        char               d_name[256]; // NUL-terminated (truncated if longer)
    };

    struct DIR {
        int    fd;
        int    sz;
        int    pos;
        int    err;
        alignas(8) char buf[4096];
        dirent out;                    // reusable result
    };

    DIR*           opendir (const char* path) noexcept;
    struct dirent* readdir (DIR* d) noexcept;
    int            closedir(DIR* d) noexcept;

} // extern "C"
