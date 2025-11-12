export module voidlibc.c.fs;
export import voidlibc.core.types;

export extern "C" {

    // Linux x86-64 compatible stat layout (glibc-like)
    struct stat_t {
        unsigned long long st_dev;
        unsigned long long st_ino;
        unsigned long long st_nlink;
        unsigned int       st_mode;
        unsigned int       st_uid;
        unsigned int       st_gid;
        unsigned int       __pad0;
        unsigned long long st_rdev;
        long long          st_size;
        long               st_blksize;
        long long          st_blocks;
        timespec           st_atim;
        timespec           st_mtim;
        timespec           st_ctim;
        long               __glibc_reserved[3];
    };

    // POSIX-ish surface (freestanding signatures we’ll support on Linux first)
    int   stat  (const char* path, struct stat_t* st) noexcept;
    int   fstat (int fd, struct stat_t* st) noexcept;

    int   mkdir (const char* path, unsigned mode) noexcept;
    int   rmdir (const char* path) noexcept;
    int   unlink(const char* path) noexcept;

    char* getcwd(char* buf, __SIZE_TYPE__ size) noexcept;

} // extern "C"
