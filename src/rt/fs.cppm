export module rt.fs;
import rt.errno;
import rt.panic;

#if defined(__linux__) && defined(__x86_64__)
import rt.linux.sys;
#endif

namespace rt {
    // openat() so we can later support dirfds and AT_* flags
    export long openat(int dfd, const char* path, long flags, long mode) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_openat(dfd, path, flags, mode);
#else
        set_errno(38 /*ENOSYS*/); return -1;
#endif
    }

    export long close(int fd) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_close(fd);
#else
        set_errno(38); return -1;
#endif
    }

    export long read(int fd, void* buf, unsigned long len) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_read(fd, buf, len);
#else
        set_errno(38); return -1;
#endif
    }

    export long write(int fd, const void* buf, unsigned long len) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_write(fd, buf, len);
#else
        set_errno(38); return -1;
#endif
    }

    export long lseek(int fd, long off, int whence) noexcept {
#if defined(__linux__) && defined(__x86_64__)
        return linux::sys_lseek(fd, off, whence);
#else
        set_errno(38); return -1;
#endif
    }
}
