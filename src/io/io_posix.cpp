module voidlibc.c.io;
import voidlibc.core.types;
import voidlibc.core.tls;
import voidlibc.sys.linux;

using namespace vl::sys::linux;

extern "C" long write(int fd, const void* buf, size_t n){
    long r = sc3(SYS_write, fd, (long)buf, n);
    if (r<0){ vl_errno() = -r; return -1; } return r;
}
extern "C" long read(int fd, void* buf, size_t n){
    long r = sc3(SYS_read, fd, (long)buf, n);
    if (r<0){ vl_errno() = -r; return -1; } return r;
}
extern "C" int close(int fd){
    long r = sc1(SYS_close, fd);
    if (r<0){ vl_errno() = -r; return -1; } return 0;
}
extern "C" int open(const char* path, int flags, unsigned mode){
    // x86-64 canonical path is openat(AT_FDCWD, ...)
    long r = sc4 /* via sc6 using r10 for the 4th arg */ (
      SYS_openat, -100 /*AT_FDCWD*/, (long)path, flags, mode
    );
    // Provide sc4 wrapper inline using sc6:
    return (int)((r<0)? (vl_errno() = -r, -1) : r);
}
extern "C" off_t lseek(int fd, off_t off, int whence){
    long r = sc3(SYS_lseek, fd, off, whence);
    if (r<0){ vl_errno() = -r; return (off_t)-1; } return (off_t)r;
}
