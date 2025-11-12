module voidlibc.c.net;

import voidlibc.core.types;
import voidlibc.core.tls;     // vl_errno()
import voidlibc.sys.linux;

using namespace vl::sys::linux;

extern "C" int socket(int domain, int type, int protocol) noexcept {
  long r = sc3(SYS_socket, domain, type, protocol);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return (int)r;
}

extern "C" int socketpair(int domain, int type, int protocol, int sv[2]) noexcept {
  long r = sc4(SYS_socketpair, domain, type, protocol, (long)sv);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return 0;
}

extern "C" int bind(int fd, const struct sockaddr* addr, socklen_t len) noexcept {
  long r = sc3(SYS_bind, fd, (long)addr, (long)len);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return 0;
}

extern "C" int connect(int fd, const struct sockaddr* addr, socklen_t len) noexcept {
  long r = sc3(SYS_connect, fd, (long)addr, (long)len);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return 0;
}

extern "C" int listen(int fd, int backlog) noexcept {
  long r = sc2(SYS_listen, fd, backlog);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return 0;
}

extern "C" int accept(int fd, struct sockaddr* addr, socklen_t* len) noexcept {
  long r = sc4(SYS_accept4, fd, (long)addr, (long)len, 0);
  if (r < 0) {
    if (-r == 38 /*ENOSYS*/) {
      r = sc3(SYS_accept, fd, (long)addr, (long)len);
    }
  }
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return (int)r;
}

extern "C" long send(int fd, const void* buf, __SIZE_TYPE__ n, int flags) noexcept {
  // sendto(fd, buf, n, flags, NULL, 0)
  long r = sc6(SYS_sendto, fd, (long)buf, (long)n, flags, 0, 0);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return r;
}

extern "C" long recv(int fd, void* buf, __SIZE_TYPE__ n, int flags) noexcept {
  // recvfrom(fd, buf, n, flags, NULL, NULL)
  long r = sc6(SYS_recvfrom, fd, (long)buf, (long)n, flags, 0, 0);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return r;
}

extern "C" int shutdown(int fd, int how) noexcept {
  long r = sc2(SYS_shutdown, fd, how);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return 0;
}

extern "C" int setsockopt(int fd, int level, int opt, const void* val, socklen_t len) noexcept {
  long r = sc5(SYS_setsockopt, fd, level, opt, (long)val, (long)len);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return 0;
}

extern "C" int getsockopt(int fd, int level, int opt, void* val, socklen_t* len) noexcept {
  long r = sc5(SYS_getsockopt, fd, level, opt, (long)val, (long)len);
  if (r < 0) { vl_errno() = (int)(-r); return -1; }
  return 0;
}
