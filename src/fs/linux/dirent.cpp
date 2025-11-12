module voidlibc.c.dirent;          // implementation unit of this module
import voidlibc.core.types;
import voidlibc.core.tls;          // vl_errno()
import voidlibc.sys.linux;         // scN, SYS_*
import voidlibc.c.memory;          // malloc/free

using namespace vl::sys::linux;

// Minimal flags we need (avoid pulling any headers)
static constexpr int O_RDONLY    = 0;
static constexpr int O_DIRECTORY = 0200000;

// Kernel getdents64 record
struct linux_dirent64 {
  unsigned long long d_ino;
  long long          d_off;
  unsigned short     d_reclen;
  unsigned char      d_type;
  char               d_name[];   // NUL-terminated
};

// DIR backing store


extern "C" DIR* opendir(const char* path) noexcept {
  long fd = sc3(SYS_open, (long)path, O_RDONLY | O_DIRECTORY, 0);
  if (fd < 0) { vl_errno() = (int)(-fd); return (DIR*)0; }

  DIR* d = (DIR*)malloc(sizeof(DIR));
  if (!d) { (void)sc1(SYS_close, (int)fd); vl_errno() = 12; return (DIR*)0; } // ENOMEM
  d->fd = (int)fd;
  d->sz = 0;
  d->pos = 0;
  d->err = 0;
  return d;
}

extern "C" dirent* readdir(DIR* d) noexcept {
  if (!d) { vl_errno() = 22; return (dirent*)0; } // EINVAL

  for (;;) {
    if (d->pos >= d->sz) {
      long r = sc3(SYS_getdents64, d->fd, (long)d->buf, (long)sizeof(d->buf));
      if (r < 0) { vl_errno() = (int)(-r); return (dirent*)0; }
      if (r == 0) return (dirent*)0; // EOF
      d->sz = (int)r;
      d->pos = 0;
    }

    char* p = d->buf + d->pos;
    linux_dirent64* k = (linux_dirent64*)p;
    int reclen = (int)k->d_reclen;
    if (reclen <= 0 || d->pos + reclen > d->sz) { vl_errno() = 5; return (dirent*)0; } // EIO

    d->out.d_ino    = k->d_ino;
    d->out.d_off    = k->d_off;
    d->out.d_reclen = (unsigned short)reclen;
    d->out.d_type   = k->d_type;

    int i = 0;
    while (i < 255 && k->d_name[i] != '\0') { d->out.d_name[i] = k->d_name[i]; ++i; }
    d->out.d_name[i] = '\0';

    d->pos += reclen;
    return &d->out;
  }
}

extern "C" int closedir(DIR* d) noexcept {
  if (!d) { vl_errno() = 22; return -1; } // EINVAL
  (void)sc1(SYS_close, d->fd);
  free(d);
  return 0;
}
