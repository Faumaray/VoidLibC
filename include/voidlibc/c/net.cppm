export module voidlibc.c.net;
export import voidlibc.core.types;

export extern "C" {
    using socklen_t = unsigned int;

    // Minimal address types
    struct sockaddr { unsigned short sa_family; char sa_data[14]; };

    struct in_addr { unsigned int s_addr; };
    struct sockaddr_in {
        unsigned short sin_family;
        unsigned short sin_port;
        in_addr        sin_addr;
        unsigned char  sin_zero[8];
    };

    struct sockaddr_un {
        unsigned short sun_family;
        char           sun_path[108];
    };

    // Families / types / flags (Linux)
    inline constexpr int AF_UNIX = 1;
    inline constexpr int AF_INET = 2;

    inline constexpr int SOCK_STREAM   = 1;
    inline constexpr int SOCK_DGRAM    = 2;
    inline constexpr int SOCK_NONBLOCK = 0x800;    // == O_NONBLOCK
    inline constexpr int SOCK_CLOEXEC  = 0x80000;  // == O_CLOEXEC

    // Shutdown how
    inline constexpr int SHUT_RD = 0, SHUT_WR = 1, SHUT_RDWR = 2;

    // Levels / a couple options
    inline constexpr int SOL_SOCKET   = 1;
    inline constexpr int SO_REUSEADDR = 2;
    inline constexpr int SO_KEEPALIVE = 9;

    // API
    int  socket(int domain, int type, int protocol) noexcept;
    int  socketpair(int domain, int type, int protocol, int sv[2]) noexcept;
    int  bind(int fd, const struct sockaddr* addr, socklen_t len) noexcept;
    int  connect(int fd, const struct sockaddr* addr, socklen_t len) noexcept;
    int  listen(int fd, int backlog) noexcept;
    int  accept(int fd, struct sockaddr* addr, socklen_t* len) noexcept;

    long send(int fd, const void* buf, __SIZE_TYPE__ n, int flags) noexcept;
    long recv(int fd,       void* buf, __SIZE_TYPE__ n, int flags) noexcept;

    int  shutdown(int fd, int how) noexcept;
    int  setsockopt(int fd, int level, int opt, const void* val, socklen_t len) noexcept;
    int  getsockopt(int fd, int level, int opt,       void* val, socklen_t* len) noexcept;
}
