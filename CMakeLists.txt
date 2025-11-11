cmake_minimum_required(VERSION 3.29)  # 3.29+ has solid C++ modules support
project(VoidLibC LANGUAGES C CXX ASM)

# =========================
# Global compile / link opts
# =========================
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

add_compile_options(
        -ffreestanding -fno-exceptions -fno-rtti -fvisibility=hidden
        -fno-stack-protector -fno-asynchronous-unwind-tables
        -mno-red-zone
        -Wall -Wextra -Werror
)
add_link_options(-nostdlib -Wl,--gc-sections -no-pie)

# =========================
# Source discovery
# =========================
file(GLOB_RECURSE RT_MODULES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/include/**/*.cppm"
        "${CMAKE_SOURCE_DIR}/src/**/*.cppm"
)

file(GLOB_RECURSE RT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/include/**/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
)

# --- Filter platform-specific .cpp from the generic source list
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(FILTER RT_SOURCES EXCLUDE REGEX ".*/src/core/windows/.*")
    list(FILTER RT_SOURCES EXCLUDE REGEX ".*/src/core/darwin/.*")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(FILTER RT_SOURCES EXCLUDE REGEX ".*/src/core/linux/.*")
    list(FILTER RT_SOURCES EXCLUDE REGEX ".*/src/core/darwin/.*")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(FILTER RT_SOURCES EXCLUDE REGEX ".*/src/core/linux/.*")
    list(FILTER RT_SOURCES EXCLUDE REGEX ".*/src/core/windows/.*")
endif()

# =========================
# Main libc archive
# =========================
add_library(voidlibc STATIC)

# One module fileset + impl sources
target_sources(voidlibc
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${RT_MODULES}
        PRIVATE
        ${RT_SOURCES}
)

# Platform CRT / init bits (added explicitly, not via glob)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(voidlibc PRIVATE
            src/core/linux/so_init.S
            src/thread/trampoline_linux.S
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(voidlibc PRIVATE
            src/core/windows/crt0_pe.cpp
            src/core/windows/dllmain.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(voidlibc PRIVATE
            src/core/darwin/crt0_macho.S
            src/core/darwin/dylib_init_fini.cpp
    )
endif()

# Linux crt0 as object library so test executables can link -nostdlib
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_library(voidcrt OBJECT src/core/linux/crt0.S)
endif()

# =========================
# Tests (one exe per tests/*.cpp)
# =========================
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/executables/*.cpp")

foreach(SRC IN LISTS TEST_SOURCES)
    get_filename_component(EXE "${SRC}" NAME_WE)
    add_executable(${EXE} "${SRC}")
    target_link_libraries(${EXE} PRIVATE voidlibc)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_sources(${EXE} PRIVATE $<TARGET_OBJECTS:voidcrt>)
        target_link_options(${EXE} PRIVATE -nostdlib -static -no-pie -Wl,-e,_start)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_options(${EXE} PRIVATE -nostdlib -Wl,-e,mainCRTStartup)
        target_link_libraries(${EXE} PRIVATE kernel32)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        target_link_options(${EXE} PRIVATE -nostdlib -Wl,-e,_start)
    endif()
endforeach()
