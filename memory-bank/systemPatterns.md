# System Patterns: VoidLibC

## Architecture Overview

VoidLibC follows a modular architecture designed for freestanding environments:

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
├─────────────────────────────────────────────────────────────┤
│                    VoidLibC API Layer                       │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │  C API Modules     │  C++ API Modules  │ Platform API   │ │
│  └─────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│                    Platform Abstraction                     │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ Linux Syscalls     │ Windows NT API    │ Darwin Syscalls│ │
│  └─────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│                    Hardware Layer                           │
└─────────────────────────────────────────────────────────────┘
```

## Key Technical Decisions

### Module Organization
- **C API Modules** (`include/voidlibc/c/`): Standard C library function implementations
- **Core Modules** (`include/voidlibc/core/`): Fundamental types, intrinsics, and platform abstractions
- **Platform Modules** (`include/voidlibc/platform/`): OS-specific implementations
- **Thread Modules** (`include/voidlibc/thread/`): Threading and synchronization primitives

### Implementation Strategy
- **Header-Only Modules**: Core functionality exposed through C++20 modules for compile-time optimization
- **Platform-Specific Sources**: Implementation files filtered by CMake based on target platform
- **Freestanding Compilation**: All code compiled with `-ffreestanding` flag, no standard library dependencies, custom CRT (_start/mainCRTStartup/Mach _start) and TLS bootstrap

## Design Patterns

### Platform Abstraction Pattern
```cpp
// Platform-agnostic interface
export namespace voidlibc::platform {
    void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
}

// Platform-specific implementation
module voidlibc.platform.linux;
import voidlibc.platform;

void* voidlibc::platform::mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return syscall(SYS_mmap, addr, length, prot, flags, fd, offset);
}
```

### Syscall Wrapper Pattern
- Direct syscall invocation with minimal overhead
- Error handling through thread-local errno
- Consistent parameter passing conventions

### CRT Initialization Pattern
- Platform-specific entry points (`_start`, `mainCRTStartup`)
- Proper TLS and stack setup
- Dynamic linker integration where supported

## Component Relationships

### Core Dependencies
- **types.cppm**: Fundamental type definitions used throughout the library
- **intrinsics.cppm**: Compiler intrinsics and low-level operations
- **tls.cppm**: Thread-local storage management
- **environ.cppm**: Environment variable handling

### Platform Integration
- **os.cppm**: Operating system detection and feature flags
- **syscall modules**: Direct kernel interface implementations
- **crt0 files**: Platform-specific program initialization

### Library Structure
- **Static Library**: `libvoidlibc.a` containing all implementations
- **Module Interface**: Public API exposed through C++ modules
- **Test Executables**: Individual test programs for each functionality area

## Critical Implementation Paths

### Memory Allocation
1. `malloc()` → `vlmalloc.cpp` → Platform-specific `mmap()`/`VirtualAlloc()`
2. Thread-safe allocation with mutex protection
3. Efficient free list management for small allocations

### Thread Creation
1. `pthread_create()` → Platform-specific thread creation
2. TLS setup and cleanup handling
3. Signal masking and stack allocation

### System Call Dispatch
1. Function call → Syscall number lookup → Assembly invocation
2. Error code translation to errno
3. Platform-specific register usage optimization
