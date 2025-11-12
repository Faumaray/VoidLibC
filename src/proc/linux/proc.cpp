module voidlibc.c.proc;

import voidlibc.core.types;
import voidlibc.core.tls;   // vl_errno()
import voidlibc.sys.linux;

using namespace vl::sys::linux;

extern "C" int getpid() noexcept {
    long r = sc0(SYS_getpid);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}

extern "C" int getppid() noexcept {
    long r = sc0(SYS_getppid);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}

extern "C" int gettid() noexcept {
    long r = sc0(SYS_gettid);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;
}

extern "C" int kill(int pid, int sig) noexcept {
    long r = sc2(SYS_kill, pid, sig);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" int fork() noexcept {
    long r = sc0(SYS_fork);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r;               // 0 in child, >0 in parent
}

extern "C" int execve(const char* path, char* const argv[], char* const envp[]) noexcept {
    long r = sc3(SYS_execve, (long)path, (long)argv, (long)envp);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r; // no return on success
}

extern "C" int waitpid(int pid, int* wstatus, int options) noexcept {
    long r = sc4(SYS_wait4, pid, (long)wstatus, options, 0);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return (int)r; // returns child pid, or 0 with WNOHANG
}
