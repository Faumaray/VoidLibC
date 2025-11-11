export module voidlibc.core.types;
// Basic ABI types without <cstdint>
export using size_t   = __SIZE_TYPE__;
export using ssize_t  = __INTPTR_TYPE__;
export using intptr_t = __INTPTR_TYPE__;
export using uintptr_t= __UINTPTR_TYPE__;
export using int8_t   = __INT8_TYPE__;
export using uint8_t  = __UINT8_TYPE__;
export using int16_t  = __INT16_TYPE__;
export using uint16_t = __UINT16_TYPE__;
export using int32_t  = __INT32_TYPE__;
export using uint32_t = __UINT32_TYPE__;
export using int64_t  = __INT64_TYPE__;
export using uint64_t = __UINT64_TYPE__;

export using time_t   = long;
export using suseconds_t = long;
export using off_t    = long;
export using mode_t   = unsigned;

// Timespec / Timeval
export struct timespec { long tv_sec; long tv_nsec; };
export struct timeval  { long tv_sec; long tv_usec; };
