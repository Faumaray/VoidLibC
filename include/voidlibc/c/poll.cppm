export module voidlibc.c.poll;
export import voidlibc.core.types;

export extern "C" {
    struct pollfd {
        int   fd;
        short events;
        short revents;
    };

    // Events (Linux)
    inline constexpr short POLLIN   = 0x0001;
    inline constexpr short POLLPRI  = 0x0002;
    inline constexpr short POLLOUT  = 0x0004;
    inline constexpr short POLLERR  = 0x0008;
    inline constexpr short POLLHUP  = 0x0010;
    inline constexpr short POLLNVAL = 0x0020;

    int poll(struct pollfd* fds, unsigned long nfds, int timeout_ms) noexcept;
}
