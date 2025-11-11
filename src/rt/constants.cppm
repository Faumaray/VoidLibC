export module rt.constants;

namespace rt {
    // AT_FDCWD
    export constexpr int AT_FDCWD = -100;

    // open flags
    export constexpr int O_RDONLY   = 0x0000;
    export constexpr int O_WRONLY   = 0x0001;
    export constexpr int O_RDWR     = 0x0002;
    export constexpr int O_CREAT    = 0x0040;
    export constexpr int O_EXCL     = 0x0080;
    export constexpr int O_TRUNC    = 0x0200;
    export constexpr int O_APPEND   = 0x0400;
    export constexpr int O_NONBLOCK = 0x0800;
    export constexpr int O_CLOEXEC  = 0x80000;

    // lseek whence
    export constexpr int SEEK_SET = 0;
    export constexpr int SEEK_CUR = 1;
    export constexpr int SEEK_END = 2;

    // mmap prot/flags
    export constexpr int PROT_NONE  = 0x0;
    export constexpr int PROT_READ  = 0x1;
    export constexpr int PROT_WRITE = 0x2;
    export constexpr int PROT_EXEC  = 0x4;

    export constexpr int MAP_SHARED    = 0x01;
    export constexpr int MAP_PRIVATE   = 0x02;
    export constexpr int MAP_FIXED     = 0x10;
    export constexpr int MAP_ANONYMOUS = 0x20;

    // dirent d_type
    export constexpr unsigned char DT_UNKNOWN = 0;
    export constexpr unsigned char DT_FIFO    = 1;
    export constexpr unsigned char DT_CHR     = 2;
    export constexpr unsigned char DT_DIR     = 4;
    export constexpr unsigned char DT_BLK     = 6;
    export constexpr unsigned char DT_REG     = 8;
    export constexpr unsigned char DT_LNK     = 10;
    export constexpr unsigned char DT_SOCK    = 12;
}
