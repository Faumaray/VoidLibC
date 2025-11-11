export module rt.net;

import rt.errno;
import rt.types;

#if defined(__linux__) && defined(__x86_64__)
  import rt.linux.sysnums_poll_net;
#endif

namespace rt::net {
  // families, types, protocols (subset)
  export constexpr int AF_INET       = 2;
  export constexpr int SOCK_STREAM   = 1;
  export constexpr int SOCK_DGRAM    = 2;
  export constexpr int SOCK_NONBLOCK = 0x800;
  export constexpr int SOCK_CLOEXEC  = 0x80000;
  export constexpr int IPPROTO_TCP   = 6;
  export constexpr int IPPROTO_UDP   = 17;

  // shutdown how
  export constexpr int SHUT_RD   = 0;
  export constexpr int SHUT_WR   = 1;
  export constexpr int SHUT_RDWR = 2;

  export struct in_addr { rt_u32 s_addr; }; // network byte order
  export struct sockaddr {
    rt_u16        sa_family;
    char          sa_data[14];
  };
  export struct sockaddr_in {
    rt_u16  sin_family;
    rt_u16  sin_port; // network byte order
    in_addr sin_addr;
    rt_u8   sin_zero[8]{};
  };

  export int socket(int domain, int type, int protocol) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_socket), "D"(domain), "S"(type), "d"(protocol)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return (int)r;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int connect(int fd, const sockaddr* addr, unsigned int len) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_connect), "D"(fd), "S"(addr), "d"(len)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int bind(int fd, const sockaddr* addr, unsigned int len) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_bind), "D"(fd), "S"(addr), "d"(len)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int listen(int fd, int backlog) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_listen), "D"(fd), "S"(backlog)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int accept4(int fd, sockaddr* addr, unsigned int* len, int flags) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; register long r10 __asm__("r10") = flags;
    asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_accept4), "D"(fd), "S"(addr), "d"(len), "r"(r10)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return (int)r;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export long sendto(int fd, const void* buf, unsigned long len, int flags,
                   const sockaddr* to, unsigned int tolen) noexcept {
#if defined(__linux__) && defined(__x86_64__)
    long r;
    register long r10 __asm__("r10") = (long)flags;   // 4th
    register long r8  __asm__("r8")  = (long)to;      // 5th
    register long r9  __asm__("r9")  = (long)tolen;   // 6th
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(rt::linux::__NR_sendto),
                   "D"(fd), "S"(buf), "d"(len),
                   "r"(r10), "r"(r8), "r"(r9)
                 : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return r;
#else
    rt::set_errno(38); return -1;
#endif
  }

  export long recvfrom(int fd, void* buf, unsigned long len, int flags,
                       sockaddr* from, unsigned int* fromlen) noexcept {
#if defined(__linux__) && defined(__x86_64__)
    long r;
    register long r10 __asm__("r10") = (long)flags;     // 4th
    register long r8  __asm__("r8")  = (long)from;      // 5th
    register long r9  __asm__("r9")  = (long)fromlen;   // 6th
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(rt::linux::__NR_recvfrom),
                   "D"(fd), "S"(buf), "d"(len),
                   "r"(r10), "r"(r8), "r"(r9)
                 : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return r;
#else
    rt::set_errno(38); return -1;
#endif
  }


  export int shutdown(int fd, int how) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_shutdown), "D"(fd), "S"(how)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }
}
