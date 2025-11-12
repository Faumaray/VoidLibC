module voidlibc.c.env;

import voidlibc.core.types;
import voidlibc.core.environ;
import voidlibc.core.tls;
import voidlibc.c.memory;   // malloc, free, realloc
import voidlibc.c.string;   // strlen, memcpy, strcmp
import voidlibc.sys.linux;  // SYS_mprotect (optional later)

using namespace vl::sys::linux;

static inline __SIZE_TYPE__ cstr_len(const char* s) {
  const char* p = s; while (*p) ++p; return (__SIZE_TYPE__)(p - s);
}
static inline int cstr_cmp(const char* a, const char* b) {
  while (*a && (*a == *b)) { ++a; ++b; }
  return (int)((unsigned char)*a - (unsigned char)*b);
}
static inline int name_eq(const char* ent, const char* name, __SIZE_TYPE__ nlen) {
  for (__SIZE_TYPE__ i=0;i<nlen;++i) if (ent[i]!=name[i]) return 0;
  return ent[nlen]=='=';
}

extern "C" char* getenv(const char* name) noexcept {
  if (!name || !*name) return (char*)0;
  __SIZE_TYPE__ nlen = cstr_len(name);
  char** env = environ;
  if (!env) return (char*)0;
  for (; *env; ++env) {
    const char* e = *env;
    if (name_eq(e, name, nlen)) return (char*)(e + nlen + 1);
  }
  return (char*)0;
}

static int ensure_env_modifiable() {
  // If env was provided by kernel/loader, we just reuse it and expand by copying on first write.
  // Detect a sentinel we set after first write by checking the slot right before environ
  // (simple approach: always clone once on first mutation).
  static int cloned = 0;
  if (cloned) return 0;

  // Count vars
  __SIZE_TYPE__ cnt = 0;
  if (environ) { while (environ[cnt]) ++cnt; }
  // allocate new vector: +2 spare
  char** nv = (char**)malloc((cnt + 2) * sizeof(char*));
  if (!nv) { vl_errno() = 12; return -1; }
  for (__SIZE_TYPE__ i=0;i<cnt;++i) nv[i] = environ[i];
  nv[cnt] = (char*)0; nv[cnt+1] = (char*)0;
  environ = nv;
  cloned = 1;
  return 0;
}

extern "C" int setenv(const char* name, const char* value, int overwrite) noexcept {
  if (!name || !*name || !value) { vl_errno() = 22; return -1; }
  if (ensure_env_modifiable() != 0) return -1;

  __SIZE_TYPE__ nlen = cstr_len(name);
  __SIZE_TYPE__ vlen = cstr_len(value);

  // search existing
  __SIZE_TYPE__ i = 0;
  for (; environ[i]; ++i) {
    const char* e = environ[i];
    if (name_eq(e, name, nlen)) {
      if (!overwrite) return 0;
      // replace in place with fresh "name=value"
      __SIZE_TYPE__ tot = nlen + 1 + vlen + 1;
      char* nv = (char*)malloc(tot);
      if (!nv) { vl_errno() = 12; return -1; }
      // copy name, '=', value, '\0'
      for (__SIZE_TYPE__ k=0;k<nlen;++k) nv[k]=name[k];
      nv[nlen]='=';
      for (__SIZE_TYPE__ k=0;k<vlen;++k) nv[nlen+1+k]=value[k];
      nv[tot-1]='\0';
      environ[i] = nv;
      return 0;
    }
  }

  // append new
  __SIZE_TYPE__ cnt = i;
  // maybe need grow by 2 (new + null)
  char** old = environ;
  // try to find spare null slot
  if (old[cnt] != (char*)0 || old[cnt+1] != (char*)0) {
    // grow
    char** nw = (char**)realloc(old, (cnt + 2 + 8) * sizeof(char*)); // small slack
    if (!nw) { vl_errno() = 12; return -1; }
    environ = nw;
    // ensure nulls
    for (__SIZE_TYPE__ z = cnt; z < cnt + 2 + 8; ++z) environ[z] = (char*)0;
  }

  __SIZE_TYPE__ tot = nlen + 1 + vlen + 1;
  char* nv = (char*)malloc(tot);
  if (!nv) { vl_errno() = 12; return -1; }
  for (__SIZE_TYPE__ k=0;k<nlen;++k) nv[k]=name[k];
  nv[nlen]='=';
  for (__SIZE_TYPE__ k=0;k<vlen;++k) nv[nlen+1+k]=value[k];
  nv[tot-1]='\0';

  environ[cnt]   = nv;
  environ[cnt+1] = (char*)0;
  return 0;
}

extern "C" int unsetenv(const char* name) noexcept {
  if (!name || !*name) { vl_errno() = 22; return -1; }
  if (ensure_env_modifiable() != 0) return -1;

  __SIZE_TYPE__ nlen = cstr_len(name);
  __SIZE_TYPE__ i=0, w=0;
  for (; environ[i]; ++i) {
    const char* e = environ[i];
    if (!name_eq(e, name, nlen)) environ[w++] = environ[i];
  }
  environ[w] = (char*)0;
  return 0;
}

extern "C" int putenv(char* string) noexcept {
  if (!string) { vl_errno() = 22; return -1; }
  if (ensure_env_modifiable() != 0) return -1;

  // find '='
  char* p = string; while (*p && *p!='=') ++p;
  if (*p != '=') { vl_errno() = 22; return -1; } // need name=value

  // compute name length
  __SIZE_TYPE__ nlen = (__SIZE_TYPE__)(p - string);

  // replace if exists
  __SIZE_TYPE__ i=0;
  for (; environ[i]; ++i) {
    const char* e = environ[i];
    // match name=
    __SIZE_TYPE__ k=0;
    while (k<nlen && e[k]==string[k]) ++k;
    if (k==nlen && e[nlen]=='=') { environ[i] = string; return 0; }
  }

  // append
  environ[i] = string;
  environ[i+1] = (char*)0;
  return 0;
}
