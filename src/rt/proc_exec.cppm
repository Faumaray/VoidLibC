export module rt.proc_exec;

import rt.errno;
import rt.types;

#if defined(__linux__) && defined(__x86_64__)
  import rt.linux.sysnums_proc;
  import rt.linux.sys;
#endif

namespace rt::proc {
  // wait status helpers (classic layout)
  export inline bool WIFEXITED(int status)  { return ((status & 0x7f) == 0); }
  export inline rt_u8 WEXITSTATUS(int s)    { return (rt_u8)((s >> 8) & 0xff); }
  export inline bool WIFSIGNALED(int s)     { return (((s & 0x7f) != 0) && (((s & 0x7f)+1) >> 1) > 0); }
  export inline rt_u8 WTERMSIG(int s)       { return (rt_u8)(s & 0x7f); }
  export constexpr int WNOHANG = 1;


  export int fork() noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r):"a"(rt::linux::__NR_fork):"rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return (int)r; // 0 in child, pid in parent
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int vfork() noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r):"a"(rt::linux::__NR_vfork):"rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; } return (int)r;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  // argv/envp must be null-terminated vectors
  export int execve(const char* path, char* const argv[], char* const envp[]) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_execve), "D"(path), "S"(argv), "d"(envp)
      : "rcx","r11","memory");
    // on success, no return; on failure, -errno
    rt::set_errno((int)-r); return -1;
  #else
    rt::set_errno(38); return -1;
  #endif
  }

  export int wait4(int pid, int* status, int options) noexcept {
#if defined(__linux__) && defined(__x86_64__)
    long r = rt::linux::sys_wait4(pid, status, options, nullptr);
    if (r < 0) { rt::set_errno((int)-r); return -1; }
    return (int)r; // child pid (or 0 if WNOHANG and no state changes)
#else
    rt::set_errno(38); return -1;
#endif
  }

  export int waitpid(int pid, int* status, int options) noexcept {
    return wait4(pid, status, options);
  }
}
