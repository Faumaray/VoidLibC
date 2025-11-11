export module rt.types;

// exact-width integers using compiler-provided types (no libc/stdlib)
namespace rt::types {
    using u8  = __UINT8_TYPE__;
    using u16 = __UINT16_TYPE__;
    using u32 = __UINT32_TYPE__;
    using u64 = __UINT64_TYPE__;

    using i8  = __INT8_TYPE__;
    using i16 = __INT16_TYPE__;
    using i32 = __INT32_TYPE__;
    using i64 = __INT64_TYPE__;

    using usize = __SIZE_TYPE__;
    using isize = __PTRDIFF_TYPE__;

    using uptr = __UINTPTR_TYPE__;
    using iptr = __INTPTR_TYPE__;
}

export using rt_u8  = rt::types::u8;
export using rt_u16 = rt::types::u16;
export using rt_u32 = rt::types::u32;
export using rt_u64 = rt::types::u64;

export using rt_i8  = rt::types::i8;
export using rt_i16 = rt::types::i16;
export using rt_i32 = rt::types::i32;
export using rt_i64 = rt::types::i64;

export using rt_usize = rt::types::usize;
export using rt_isize = rt::types::isize;

export using rt_uptr = rt::types::uptr;
export using rt_iptr = rt::types::iptr;
