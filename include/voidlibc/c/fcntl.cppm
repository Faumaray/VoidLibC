export module voidlibc.c.fcntl;
export import voidlibc.core.types;

export extern "C" {

    // open flags (subset)
    inline constexpr int O_RDONLY    = 0x0000;
    inline constexpr int O_WRONLY    = 0x0001;
    inline constexpr int O_RDWR      = 0x0002;
    inline constexpr int O_NONBLOCK  = 0x0800;
    inline constexpr int O_CLOEXEC   = 0x80000;
    inline constexpr int O_CREAT     = 0x0040;
    inline constexpr int O_TRUNC     = 0x0200;
    inline constexpr int O_APPEND    = 0x0400;

    // fcntl cmds (subset)
    inline constexpr int F_DUPFD     = 0;
    inline constexpr int F_GETFD     = 1;
    inline constexpr int F_SETFD     = 2;
    inline constexpr int F_GETFL     = 3;
    inline constexpr int F_SETFL     = 4;

    // FD flags
    inline constexpr int FD_CLOEXEC  = 1;

    // API
    int  fcntl(int fd, int cmd, long arg) noexcept;
    int  dup(int fd) noexcept;
    int  dup2(int oldfd, int newfd) noexcept;
    int  dup3(int oldfd, int newfd, int flags) noexcept;
    int  pipe(int fds[2]) noexcept;
    int  pipe2(int fds[2], int flags) noexcept;

}
