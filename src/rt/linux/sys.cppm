export module rt.linux.sys;
import rt.linux.sysnums;
import rt.linux.sysnums_proc;
import rt.errno;

extern "C" long rt_linux_sys_wait4(int, int*, int, void*);

namespace rt::linux {
  // Raw syscall helpers (0..6 args). Negative return -> -errno.
  // Callers translate to errno via to_errno() below.
  inline long do_syscall0(long n) {
    long r; asm volatile("syscall" : "=a"(r) : "a"(n) : "rcx","r11","memory"); return r;
  }
  inline long do_syscall1(long n, long a) {
    long r; asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a) : "rcx","r11","memory"); return r;
  }
  inline long do_syscall2(long n, long a, long b) {
    long r; asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b) : "rcx","r11","memory"); return r;
  }
  inline long do_syscall3(long n, long a, long b, long c) {
    long r; asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b),"d"(c) : "rcx","r11","memory"); return r;
  }
  inline long do_syscall4(long n, long a, long b, long c, long d) {
    long r; register long r10 __asm__("r10") = d;
    asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b),"d"(c),"r"(r10) : "rcx","r11","memory"); return r;
  }
  inline long do_syscall5(long n, long a, long b, long c, long d, long e) {
    long r; register long r10 __asm__("r10") = d; register long r8 __asm__("r8") = e;
    asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b),"d"(c),"r"(r10),"r"(r8) : "rcx","r11","memory"); return r;
  }
  inline long do_syscall6(long n, long a, long b, long c, long d, long e, long f) {
    long r; register long r10 __asm__("r10") = d; register long r8 __asm__("r8") = e; register long r9 __asm__("r9") = f;
    asm volatile("syscall" : "=a"(r) : "a"(n),"D"(a),"S"(b),"d"(c),"r"(r10),"r"(r8),"r"(r9) : "rcx","r11","memory"); return r;
  }

  inline long to_errno(long r) {
    if (r < 0) { rt::set_errno((int)-r); return -1; }
    return r;
  }

  // ---- exported minimal API (Linux-only) ----
  export long sys_write(int fd, const void* buf, unsigned long len) noexcept {
    return to_errno(do_syscall3(__NR_write, fd, (long)buf, (long)len));
  }
  export [[noreturn]] void sys_exit(int code) noexcept {
    (void)do_syscall1(__NR_exit, code);
    __builtin_unreachable();
  }
  export long sys_read(int fd, void* buf, unsigned long len) noexcept {
    return to_errno(do_syscall3(__NR_read, fd, (long)buf, (long)len));
  }
  export long sys_close(int fd) noexcept {
    return to_errno(do_syscall1(__NR_close, fd));
  }
  // openat flags are passed through as-is
  export long sys_openat(int dfd, const char* path, long flags, long mode) noexcept {
    return to_errno(do_syscall4(__NR_openat, dfd, (long)path, flags, mode));
  }
  export long sys_lseek(int fd, long off, int whence) noexcept {
    return to_errno(do_syscall3(__NR_lseek, fd, off, whence));
  }
  export long sys_mmap(void* addr, unsigned long len, long prot, long flags, int fd, long off) noexcept {
    return to_errno(do_syscall6(__NR_mmap, (long)addr, (long)len, prot, flags, fd, off));
  }
  export long sys_munmap(void* addr, unsigned long len) noexcept {
    return to_errno(do_syscall2(__NR_munmap, (long)addr, (long)len));
  }
  export long sys_mprotect(void* addr, unsigned long len, long prot) noexcept {
    return to_errno(do_syscall3(__NR_mprotect, (long)addr, (long)len, prot));
  }
  export long sys_nanosleep(const void* req_ts, void* rem_ts) noexcept {
    return to_errno(do_syscall2(__NR_nanosleep, (long)req_ts, (long)rem_ts));
  }
  export long sys_clock_gettime(int clk_id, void* ts) noexcept {
    return to_errno(do_syscall2(__NR_clock_gettime, clk_id, (long)ts));
  }

  export long sys_wait4(int pid, int* status, int options, void* rusage) noexcept {
#if defined(__linux__) && defined(__x86_64__)
    long ret;
    register long a4 __asm__("r10") = (long)rusage;  // pin 4th arg
    asm volatile("mov $61, %%rax \n\tsyscall"
                 : "=a"(ret)
                 : "D"(pid), "S"(status), "d"(options), "r"(a4)
                 : "rcx","r11","memory");
    return ret;  // <0 → -errno
#else
    return -38;  // ENOSYS
#endif
  }
}
