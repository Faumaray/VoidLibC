export module voidlibc.core.intrinsics;

export inline void compiler_fence()  { __asm__ __volatile__("" ::: "memory"); }
export inline unsigned short bswap16(unsigned short x){ return __builtin_bswap16(x); }
export inline unsigned int   bswap32(unsigned int x){ return __builtin_bswap32(x); }
export inline unsigned long long bswap64(unsigned long long x){ return __builtin_bswap64(x); }
