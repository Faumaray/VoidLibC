export module rt.stat;

import rt.errno;

#if defined(__linux__) && defined(__x86_64__)
  import rt.linux.sys;
  import rt.linux.sysnums;
#endif
extern long rt_linux_do_syscall2(long, long, long); // local alias trick avoided; use wrapper below

namespace rt {
  // very small stat layout (64-bit); extend as needed
  export struct stat64 {
    unsigned long st_dev;
    unsigned long st_ino;
    unsigned long st_nlink;
    unsigned int  st_mode;
    unsigned int  st_uid;
    unsigned int  st_gid;
    unsigned int  __pad0;
    unsigned long st_rdev;
    long          st_size;
    long          st_blksize;
    long          st_blocks;
    long          st_atime; long st_atime_nsec;
    long          st_mtime; long st_mtime_nsec;
    long          st_ctime; long st_ctime_nsec;
    long          __reserved[3];
  };

  // fstat via legacy __NR_fstat (works on x86_64)
  export long fstat(int fd, stat64* st) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    // reuse linux raw syscall helpers from rt.linux.sys
    // We can't reach the internal helpers, so expose via thin inline in this TU:
    auto call = [](long n, long a, long b) {
      long r; asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b) : "rcx","r11","memory"); return r;
    };
    long r = call(rt::linux::__NR_fstat, fd, (long)st);
    if (r < 0) { set_errno((int)-r); return -1; }
    return r;
  #else
    set_errno(38); return -1;
  #endif
  }

  // newfstatat / fstatat(2)
  export long newfstatat(int dfd, const char* path, stat64* st, int flags) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    auto call = [](long n, long a, long b, long c, long d){
      long r; register long r10 __asm__("r10") = d;
      asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b),"d"(c),"r"(r10) : "rcx","r11","memory"); return r;
    };
    long r = call(rt::linux::__NR_newfstatat, dfd, (long)path, (long)st, flags);
    if (r < 0) { set_errno((int)-r); return -1; }
    return r;
  #else
    set_errno(38); return -1;
  #endif
  }
}
