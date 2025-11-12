# Product Context: VoidLibC

## Why VoidLibC Exists

### The Problem
Traditional C standard libraries (glibc, musl, etc.) are designed for hosted environments with operating system support. When developing:
- Operating system kernels
- Embedded systems firmware
- Bootloaders and early-stage system software
- Standalone applications without OS dependencies

Developers face significant challenges:
- **Dependency on OS services**: Standard libc implementations require system calls and OS abstractions
- **Large footprint**: Full libc implementations include unnecessary hosted environment features
- **Limited portability**: Platform-specific assumptions make cross-compilation difficult
- **Performance overhead**: General-purpose implementations prioritize compatibility over efficiency
- **Tight coupling to hosted stacks**: Linkers, crti/crtn, dynamic loaders, and libgcc/libstdc++ assumptions.

### What VoidLibC Solves

VoidLibC provides a **self-contained libc replacement** that enables developers to:
- **Build without OS dependencies**: Pure freestanding implementation with no external runtime requirements
- **Maintain familiar APIs**: Drop-in replacement for standard C library functions
- **Target multiple platforms**: Unified codebase supporting Linux, Windows, and macOS freestanding environments
- **Optimize for constrained environments**: Minimal overhead suitable for embedded and kernel development
- **Leverage modern C++**: Clean, maintainable implementation using C++26 features

## How It Should Work

### User Experience Goals
- **Zero-configuration setup**: Simple CMake-based build system with automatic platform detection
- **Familiar development workflow**: Standard C function calls with expected behavior
- **Comprehensive testing**: Extensive test suite ensuring reliability across platforms
- **Clear documentation**: Well-documented APIs and implementation details

### Core Functionality
- **Memory management**: malloc/free implementations with efficient allocation strategies
- **String operations**: Complete string.h function implementations
- **File I/O**: Platform-appropriate file system abstractions
- **Threading support**: POSIX thread compatibility for concurrent programming
- **Network operations**: Socket and network function implementations
- **System calls**: Direct syscall interfaces with platform-specific optimizations

### Platform-Specific Behavior
- **Linux**: Native syscall usage with optimized assembly sequences
- **Windows**: Win32 API integration for freestanding compatibility
- **macOS**: Mach-O and Darwin syscall implementations

## Target Users
- **OS Kernel Developers**: Building custom operating systems without libc dependencies
- **Embedded Systems Engineers**: Developing firmware for resource-constrained devices
- **Bootloader Authors**: Creating early-stage system software
- **Academic Researchers**: Studying low-level system programming concepts
- **Hobby OS Projects**: Community developers building alternative operating systems
- **Polyglot distributors** aiming to ship single-file executables and dynamic libraries across Linux/Windows/macOS.
