module voidlibc.c.fcntl;

import voidlibc.core.types;
import voidlibc.core.tls;    // vl_errno()
import voidlibc.sys.linux;

using namespace vl::sys::linux;

extern "C" int fcntl(int fd, int cmd, long arg) noexcept {
    long r = sc3(72 /*SYS_fcntl*/, fd, cmd, arg);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}

extern "C" int dup(int fd) noexcept {
    long r = sc1(32 /*SYS_dup*/, fd);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}

extern "C" int dup2(int oldfd, int newfd) noexcept {
    long r = sc2(33 /*SYS_dup2*/, oldfd, newfd);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}

extern "C" int dup3(int oldfd, int newfd, int flags) noexcept {
    long r = sc3(292 /*SYS_dup3*/, oldfd, newfd, flags);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}

extern "C" int pipe(int fds[2]) noexcept {
    long r = sc1(22 /*SYS_pipe*/, (long)fds);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" int pipe2(int fds[2], int flags) noexcept {
    long r = sc2(293 /*SYS_pipe2*/, (long)fds, flags);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}
