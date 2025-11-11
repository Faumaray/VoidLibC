export module voidlibc.core.tls_errno;

export extern "C" int* __vl_errno_location();
export inline int& vl_errno() { return *__vl_errno_location(); }

// glibc-compat accessor name for convenience
export extern "C" int* __errno_location();
