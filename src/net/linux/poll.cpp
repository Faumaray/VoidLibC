module voidlibc.c.poll;

import voidlibc.core.types;
import voidlibc.core.tls;     // vl_errno()
import voidlibc.sys.linux;

using namespace vl::sys::linux;

extern "C" int poll(struct pollfd* fds, unsigned long nfds, int timeout_ms) noexcept {
    long r = sc3(SYS_poll, (long)fds, (long)nfds, (long)timeout_ms);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}
