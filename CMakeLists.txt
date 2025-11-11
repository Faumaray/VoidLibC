cmake_minimum_required(VERSION 3.29)  # 3.29+ has solid C++ modules support

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)

project(freestanding_rt LANGUAGES C CXX ASM)

# Freestanding, no CRT, no stdlib
add_compile_options(
        $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector>
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility=hidden>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-builtin>
)

# Linker: no stdlib, custom entry, keep sections
add_link_options(
        -nostdlib
        -static
        -Wl,--gc-sections
        -Wl,-z,nostart-stop-gc
        -Wl,-T,${CMAKE_SOURCE_DIR}/linker.ld
)

# =========================
# Source auto-discovery
# =========================
file(GLOB_RECURSE RT_MODULES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/**/*.cppm"
        "${CMAKE_SOURCE_DIR}/src/**/*.ixx"
)
file(GLOB_RECURSE RT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/**/*.c"
        "${CMAKE_SOURCE_DIR}/src/**/*.S"
)

set(START_SRC "${CMAKE_SOURCE_DIR}/src/start_x86_64.S")

list(REMOVE_ITEM RT_SOURCES "${START_SRC}")
list(FILTER RT_SOURCES EXCLUDE REGEX ".*/main_demo\\.cpp$")

# =========================
# Build runtime as an exe (demo) and as a static archive (future link)
# =========================
add_library(rt_static STATIC ${RT_SOURCES})
target_sources(rt_static
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src"
        FILES ${RT_MODULES}
)
target_include_directories(rt_static PRIVATE "${CMAKE_SOURCE_DIR}/src")
set_target_properties(rt_static PROPERTIES LINKER_LANGUAGE CXX)

# Primary demo binary to prove bring-up works (remove later)
add_executable(freestanding_rt
        ${START_SRC}
        src/main_demo.cpp)
target_link_libraries(freestanding_rt PRIVATE rt_static)
target_include_directories(freestanding_rt PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_link_options(freestanding_rt PRIVATE
        -Wl,-e,_start
        -no-pie
        -nostdlib
        -static
        -Wl,--gc-sections
        -Wl,-z,nostart-stop-gc
        -Wl,-T,${CMAKE_SOURCE_DIR}/linker.ld
)
set_target_properties(freestanding_rt PROPERTIES
        ENABLE_EXPORTS OFF
        OUTPUT_NAME "freestanding_rt"
        LINK_DEPENDS "${CMAKE_SOURCE_DIR}/linker.ld"
)
