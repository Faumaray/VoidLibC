module voidlibc.core.tls_errno;

// Single-thread bootstrap: no TLS yet.
static int g_vl_errno = 0;

extern "C" int* __vl_errno_location() { return &g_vl_errno; }
// glibc-compat accessor
extern "C" int* __errno_location()    { return &g_vl_errno; }
