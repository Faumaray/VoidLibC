export module rt.pathops;

import rt.errno;
import rt.types;
import rt.constants;
import rt.cstring;

#if defined(__linux__) && defined(__x86_64__)
  import rt.linux.sysnums_proc;
#endif

namespace rt::pathops {
  // readlinkat(dfd, path, buf, bufsz) — returns length (no NUL), or -1 w/ errno
  export long readlinkat(int dfd, const char* path, char* buf, rt_usize bufsz) noexcept {
#if defined(__linux__) && defined(__x86_64__)
    long r;
    register long r10 __asm__("r10") = (long)bufsz;   // arg4
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(rt::linux::__NR_readlinkat),
                   "D"(dfd), "S"(path), "d"(buf),
                   "r"(r10)                              // bind r10 in THIS asm
                 : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; }
    return r;                                           // bytes (no NUL)
#else
    rt::set_errno(38); return -1;
#endif
  }


  // simple helpers (unlink/link/symlink/rename/utimens)
  export int unlinkat(int dfd, const char* path, int flags) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_unlinkat), "D"(dfd), "S"(path), "d"(flags)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }
  export int linkat(int odfd, const char* op, int ndfd, const char* np, int flags) noexcept {
#if defined(__linux__) && defined(__x86_64__)
    long r;
    register long r10 __asm__("r10") = (long)np;       // arg4
    register long r8  __asm__("r8")  = (long)flags;    // arg5
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(rt::linux::__NR_linkat),
                   "D"(odfd), "S"(op), "d"(ndfd),
                   "r"(r10), "r"(r8)
                 : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; }
    return 0;
#else
    rt::set_errno(38); return -1;
#endif
  }

  export int symlinkat(const char* target, int dfd, const char* linkpath) noexcept {
  #if defined(__linux__) && defined(__x86_64__)
    long r; asm volatile("syscall":"=a"(r)
      : "a"(rt::linux::__NR_symlinkat), "D"(target), "S"(dfd), "d"(linkpath)
      : "rcx","r11","memory");
    if (r<0){ rt::set_errno((int)-r); return -1; } return 0;
  #else
    rt::set_errno(38); return -1;
  #endif
  }
  export int renameat2(int odfd, const char* op, int ndfd, const char* np, unsigned flags) noexcept {
#if defined(__linux__) && defined(__x86_64__)
    long r;
    register long r10 __asm__("r10") = (long)np;       // arg4
    register long r8  __asm__("r8")  = (long)flags;    // arg5
    asm volatile("syscall"
                 : "=a"(r)
                 : "a"(rt::linux::__NR_renameat2),
                   "D"(odfd), "S"(op), "d"(ndfd),
                   "r"(r10), "r"(r8)
                 : "rcx","r11","memory");
    if (r < 0) { rt::set_errno((int)-r); return -1; }
    return 0;
#else
    rt::set_errno(38); return -1;
#endif
  }


  // realpath (no allocation): resolves symlinks up to max_depth, writes absolute path to out.
  // returns out on success or nullptr on error; sets errno.
  export char* realpath_at(int dfd, const char* path, char* out, rt_usize out_cap, int max_depth) noexcept {
    if (!path || !out || out_cap == 0) { rt::set_errno(22); return (char*)0; } // EINVAL

    // Scratch that lives for the whole call
    char rem[4096];
    char target[4096];

    // Seed output as absolute (normalize to absolute)
    out[0] = '/'; out[1] = 0;
    rt_usize out_len = 1;

    // Initialize remaining path with a collapsed copy of `path`
    {
      const char* s = path;
      // collapse leading '/'
      while (*s == '/') ++s;
      rt_usize k = 0;
      for (; *s && k < sizeof(rem) - 1; ++s) rem[k++] = *s;
      rem[k] = 0;
    }

    auto append_seg = [&](const char* seg, rt_usize seg_len) -> bool {
      if (out_len > 1 && out[out_len - 1] != '/') {
        if (out_len + 1 >= out_cap) { rt::set_errno(34); return false; } // ERANGE
        out[out_len++] = '/';
      }
      if (out_len + seg_len >= out_cap) { rt::set_errno(34); return false; }
      for (rt_usize i = 0; i < seg_len; ++i) out[out_len++] = seg[i];
      out[out_len] = 0;
      return true;
    };

    while (true) {
      // skip leading slashes in rem
      const char* p = rem;
      while (*p == '/') ++p;
      if (!*p) break;

      // capture next segment into a temp buffer BEFORE we mutate `rem`
      const char* seg_ptr = p;
      while (*p && *p != '/') ++p;
      rt_usize seg_len = (rt_usize)(p - seg_ptr);

      if (seg_len == 0) {
        // only slashes left
        // advance rem to end
        rem[0] = 0;
        break;
      }

      // Copy segment safely
      char segbuf[512];
      if (seg_len >= sizeof(segbuf)) { rt::set_errno(34); return (char*)0; } // ERANGE
      for (rt_usize i = 0; i < seg_len; ++i) segbuf[i] = seg_ptr[i];

      // Advance `p` over any subsequent slashes and compact into `rem`
      while (*p == '/') ++p;
      {
        rt_usize i = 0;
        for (; p[i] && i < sizeof(rem) - 1; ++i) rem[i] = p[i];
        rem[i] = 0;
      }

      // Handle "." and ".."
      if (seg_len == 1 && segbuf[0] == '.') continue;
      if (seg_len == 2 && segbuf[0] == '.' && segbuf[1] == '.') {
        if (out_len > 1) {
          if (out[out_len - 1] == '/') --out_len;
          while (out_len > 1 && out[out_len - 1] != '/') --out_len;
          out[out_len] = 0;
        }
        continue;
      }

      // Append this segment
      if (!append_seg(segbuf, seg_len)) return (char*)0;

      // Try resolving symlink at current `out`
      if (max_depth > 0) {
        long n;
        // readlinkat(dfd, out, target, sizeof(target)-1) with arg4 in r10
  #if defined(__linux__) && defined(__x86_64__)
        {
          register long r10 __asm__("r10") = (long)(sizeof(target) - 1);
          asm volatile("syscall"
            : "=a"(n)
            : "a"(rt::linux::__NR_readlinkat),
              "D"(rt::AT_FDCWD), "S"(out), "d"(target), "r"(r10)
            : "rcx","r11","memory");
        }
  #else
        n = -38; // ENOSYS
  #endif
        if (n < 0) {
          // not a symlink or error: ignore unless it's a hard error we care about
        } else if (n > 0) {
          target[n] = 0;
          // Absolute target resets out; relative pops the last segment
          if (target[0] == '/') {
            out_len = 1; out[0] = '/'; out[1] = 0;
          } else {
            if (out_len && out[out_len - 1] == '/') --out_len;
            while (out_len > 1 && out[out_len - 1] != '/') --out_len;
            out[out_len] = 0;
          }

          // Compose new remaining path: target + (existing rem if any)
          char combined[4096];
          rt_usize k = 0;
          // copy target
          for (; target[k] && k < sizeof(combined) - 1; ++k) combined[k] = target[k];
          // add '/' between target and rem if both non-empty
          if (combined[0] && rem[0] && k < sizeof(combined) - 1) combined[k++] = '/';
          for (rt_usize i = 0; rem[i] && k < sizeof(combined) - 1; ++i) combined[k++] = rem[i];
          combined[k] = 0;

          // overwrite rem with combined
          rt_usize i = 0;
          for (; combined[i] && i < sizeof(rem) - 1; ++i) rem[i] = combined[i];
          rem[i] = 0;

          --max_depth;
          continue; // process the new path
        }
      }
    }

    if (out_len > 1 && out[out_len - 1] == '/') { out[--out_len] = 0; }
    return out;
  }

  // convenience realpath relative to CWD
  export char* realpath(const char* path, char* out, rt_usize out_cap, int max_depth = 16) noexcept {
    return realpath_at(rt::AT_FDCWD, path, out, out_cap, max_depth);
  }
}
