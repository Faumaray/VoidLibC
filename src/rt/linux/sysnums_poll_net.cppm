export module rt.linux.sysnums_poll_net;

namespace rt::linux {
    // fds & polling
    export constexpr long __NR_dup        = 32;
    export constexpr long __NR_dup3       = 292;
    export constexpr long __NR_pipe2      = 293;
    export constexpr long __NR_poll       = 7;
    export constexpr long __NR_ppoll      = 271;

    // epoll
    export constexpr long __NR_epoll_create1 = 291;
    export constexpr long __NR_epoll_ctl     = 233;
    export constexpr long __NR_epoll_pwait   = 281; // epoll_wait is 232

    // sockets
    export constexpr long __NR_socket    = 41;
    export constexpr long __NR_connect   = 42;
    export constexpr long __NR_accept4   = 288;
    export constexpr long __NR_bind      = 49;
    export constexpr long __NR_listen    = 50;
    export constexpr long __NR_setsockopt= 54;
    export constexpr long __NR_getsockopt= 55;
    export constexpr long __NR_sendto    = 44;
    export constexpr long __NR_recvfrom  = 45;
    export constexpr long __NR_shutdown  = 48;
}
