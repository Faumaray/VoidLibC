module;
#if defined(__linux__)
#define VL_OS_LINUX 1
#elif defined(_WIN32) || defined(_WIN64)
#define VL_OS_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
#define VL_OS_DARWIN 1
#else
#define VL_OS_UNKNOWN 1
#endif
export module voidlibc.platform.os;

export enum class vl_os { linux_, windows_, darwin_, unknown_ };

export consteval vl_os vl_current_os() {
#if VL_OS_LINUX
    return vl_os::linux_;
#elif VL_OS_WINDOWS
    return vl_os::windows_;
#elif VL_OS_DARWIN
    return vl_os::darwin_;
#else
    return vl_os::unknown_;
#endif
}
