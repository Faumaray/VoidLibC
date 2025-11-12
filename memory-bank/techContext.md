# Technical Context: VoidLibC

## Technologies Used

### Core Language and Standards
- **C++26**: Primary implementation language leveraging modern features
  - Modules for clean API boundaries and faster compilation
  - Concepts for type safety and generic programming
  - Coroutines for asynchronous operations (future use)
- **C17**: Target API compatibility for standard library functions
- **Freestanding C++**: No dependency on standard library runtime
- **Custom CRT/TLS**: _start/mainCRTStartup, TLS, atexit, errno implemented in-project

### Build System
- **CMake 3.29+**: Cross-platform build configuration
  - Modern C++ module support with `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD`
  - Platform detection and conditional compilation
  - Automatic test executable generation
  - Polyglot packaging hooks for producing single-file executables and dynamic libraries that run on Linux/Windows/macOS without rebuilding

### Platform Support
- **Linux**: Primary development and testing platform
  - Direct syscall interface via `src/syscall/linux/sys_linux.cppm`
  - x86_64 architecture with System V ABI compliance
- **Windows**: NT API integration for freestanding compatibility
  - Win32 API usage where necessary
  - PE executable format support
- **macOS**: Darwin syscall implementations
  - Mach-O executable format
  - XNU kernel interface
- **Polyglot Artifacts**:
  - Build single binaries and dynamic libraries embedding ELF/PE/Mach-O slices; runtime OS selection enables running on any of the three platforms without rebuilding

### Development Tools
- **CLion**: Primary IDE with CMake integration
- **GCC/Clang**: Compiler support with freestanding flags
- **Nix**: Development environment management via `flake.nix`
- **Git**: Version control with GitHub hosting

## Development Setup

### Environment Requirements
- **Compiler**: GCC 13+ or Clang 17+ with C++26 support
- **CMake**: Version 3.29+ for module support
- **Platform SDKs**: Native development tools for target platforms

### Build Configuration
```cmake
# Key CMake settings
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)

# Freestanding compilation flags
add_compile_options(
    -ffreestanding -nostdlib -fno-exceptions -fno-rtti
    -fvisibility=hidden -fno-stack-protector -fno-asynchronous-unwind-tables
    -Wall -Wextra -Werror
)
```

### Directory Structure
```
├── include/voidlibc/     # Public module interfaces
│   ├── c/               # C standard library APIs
│   ├── core/            # Core functionality
│   ├── platform/        # Platform abstractions
│   └── thread/          # Threading APIs
├── src/                 # Implementation files
│   ├── core/            # Core implementations
│   ├── syscall/         # Platform-specific syscalls
│   ├── platform/        # Platform-specific code
│   └── ...               # io, time, mem, fs, net, proc, signal, etc.
├── tests/executables/   # Test programs
└── memory-bank/         # Project documentation
```

## Technical Constraints

### Freestanding Limitations
- **No standard library**: Cannot use `std::` types or functions
- **No exceptions**: All error handling via return codes and errno
- **No RTTI**: Runtime type information unavailable
- **Limited C++ features**: Only freestanding-compatible language features

### Platform-Specific Constraints
- **Linux**: Must handle syscall ABI changes across kernel versions
- **Windows**: Limited freestanding support requires careful API selection
- **macOS**: Mach-O specific initialization requirements
- **Polyglot**: Artifact size/layout must accommodate multiple object formats while preserving each platform’s loader expectations

### Performance Constraints
- **Minimal overhead**: Syscall wrapper functions must be lightweight
- **Memory efficiency**: Small memory footprint for embedded use
- **Compile time**: Module system helps with incremental compilation

## Dependencies

### External Dependencies
- **None**: Pure freestanding implementation with zero external dependencies
- **Self-contained**: All required functionality implemented internally

### Internal Module Dependencies
```
types.cppm (fundamental types)
    ↓
intrinsics.cppm (compiler builtins)
    ↓
tls.cppm (thread-local storage)
environ.cppm (environment handling)
    ↓
platform/os.cppm (OS detection)
    ↓
syscall modules (platform-specific)
    ↓
C API modules (public interfaces)
```

## Tool Usage Patterns

### Compilation Workflow
1. **Module discovery**: CMake globs `.cppm` files for module interface units
2. **Platform filtering**: Exclude platform-specific sources based on `CMAKE_SYSTEM_NAME`
3. **Freestanding build**: Apply `-ffreestanding` and other flags globally
4. **Static linking**: Generate `libvoidlibc.a` for application linking
5. **Polyglot packaging**: (when enabled) combine ELF/PE/Mach-O slices into a single executable or dynamic library runnable on Linux/Windows/macOS without rebuilding

### Testing Approach
- **Individual executables**: Each test in `tests/executables/` builds standalone
- **Static linking**: Tests link against `libvoidlibc.a` with `-nostdlib`
- **Platform-specific entry**: Different CRT startup code per platform, polyglot builds validated end-to-end


### Development Iteration
- **Header-only modules**: Changes to `.cppm` files require full rebuild
- **Incremental implementation**: Add functionality in platform-specific `.cpp` files
- **Cross-platform validation**: Regularly build/run native and polyglot artifacts to ensure ABI and loader correctness across all targets
