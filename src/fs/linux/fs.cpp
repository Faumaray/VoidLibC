module voidlibc.c.fs;

import voidlibc.core.types;
import voidlibc.core.tls;     // vl_errno()
import voidlibc.sys.linux;    // scN, SYS_*

using namespace vl::sys::linux;

extern "C" int stat(const char* path, struct stat_t* st) noexcept {
    long r = sc2(SYS_stat, (long)path, (long)st);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" int fstat(int fd, struct stat_t* st) noexcept {
    long r = sc2(SYS_fstat, fd, (long)st);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" int mkdir(const char* path, unsigned mode) noexcept {
    long r = sc2(SYS_mkdir, (long)path, (long)mode);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" int rmdir(const char* path) noexcept {
    long r = sc1(SYS_rmdir, (long)path);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" int unlink(const char* path) noexcept {
    long r = sc1(SYS_unlink, (long)path);
    if (r < 0) { vl_errno() = (int)(-r); return -1; }
    return 0;
}

extern "C" char* getcwd(char* buf, __SIZE_TYPE__ size) noexcept {
    long r = sc2(SYS_getcwd, (long)buf, (long)size);
    if (r < 0) { vl_errno() = (int)(-r); return (char*)0; }
    // kernel wrote a NUL-terminated path; return buf
    return buf;
}
