export module voidlibc.c.io;
export import voidlibc.core.types;   // re-export types to importers

// Minimal O_* and SEEK_*
export inline constexpr int O_RDONLY = 0;
export inline constexpr int O_WRONLY = 1;
export inline constexpr int O_RDWR   = 2;
export inline constexpr int O_CREAT  = 64;
export inline constexpr int O_TRUNC  = 512;
export inline constexpr int O_APPEND = 1024;

export inline constexpr int SEEK_SET = 0;
export inline constexpr int SEEK_CUR = 1;
export inline constexpr int SEEK_END = 2;

export extern "C" {
    long write(int fd, const void* buf, size_t n);
    long read(int fd, void* buf, size_t n);
    int  close(int fd);
    int  open(const char* path, int flags, unsigned mode);
    off_t lseek(int fd, off_t off, int whence);
}
