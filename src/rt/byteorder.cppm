export module rt.byteorder;

import rt.types;

namespace rt {
    export rt_u16 bswap16(rt_u16 x) noexcept {
        return (rt_u16)((x >> 8) | (x << 8));
    }
    export rt_u32 bswap32(rt_u32 x) noexcept {
        return (x >> 24) |
               ((x >> 8) & 0x0000FF00u) |
               ((x << 8) & 0x00FF0000u) |
               (x << 24);
    }
    export rt_u64 bswap64(rt_u64 x) noexcept {
        return ((x & 0x00000000000000FFull) << 56) |
               ((x & 0x000000000000FF00ull) << 40) |
               ((x & 0x0000000000FF0000ull) << 24) |
               ((x & 0x00000000FF000000ull) << 8)  |
               ((x & 0x000000FF00000000ull) >> 8)  |
               ((x & 0x0000FF0000000000ull) >> 24) |
               ((x & 0x00FF000000000000ull) >> 40) |
               ((x & 0xFF00000000000000ull) >> 56);
    }

    // assume little-endian host (x86-64); adjust if you add BE later
    export rt_u16 htons(rt_u16 x) noexcept { return bswap16(x); }
    export rt_u16 ntohs(rt_u16 x) noexcept { return bswap16(x); }
    export rt_u32 htonl(rt_u32 x) noexcept { return bswap32(x); }
    export rt_u32 ntohl(rt_u32 x) noexcept { return bswap32(x); }
    export rt_u64 htonll(rt_u64 x) noexcept { return bswap64(x); }
    export rt_u64 ntohll(rt_u64 x) noexcept { return bswap64(x); }
}
