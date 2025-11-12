# Project Brief: VoidLibC

## Overview
VoidLibC is a freestanding C standard library implementation written in modern C++26, designed to provide essential libc functionality for systems operating without a host operating system. It targets embedded systems, OS kernels, and other freestanding environments across multiple platforms (Linux, Windows, macOS).

## Core Requirements
- **Freestanding Compatibility**: All code must compile and run in freestanding environments with no external dependencies (including not depending on an existing libc); custom CRT, TLS, and startup.
- **C/POSIX Surface Compatibility**: Provide implementations of core C standard library functions with compatible APIs (and a practical POSIX subset) sufficient to replace libc/glibc for typical userland programs; stable C ABI.
- **Cross-Platform Support**: Platform-specific implementations for Linux, Windows, and Darwin under the same module interfaces
- **Modern C++ Implementation**: Utilize C++26 features including modules for better organization and performance, no <std…> usage; explicit low-level code (inline asm where needed), attributes, and constexpr layout.
- **Minimal Overhead**: Focus on efficiency and small footprint suitable for resource-constrained environments

## Project Goals
- Deliver a complete, production-ready freestanding libc alternative that can self-host (build itself) and act as a drop-in for freestanding userland.
- Maintain API/ABI compatibility with standard C library functions
- Provide robust platform abstractions for system calls and low-level operations behind uniform module interfaces.
- Enable development of freestanding applications using familiar C interfaces and a modern C++26 codebase.
- Support both static linking and dynamic library usage where applicable and deliver polyglot single-file executables and dynamic libraries.

## Success Criteria
- All core C library functions implemented and tested
- Successful compilation and execution of test programs across supported platforms
- Minimal runtime overhead compared to standard libc implementations
- Clear, maintainable codebase using modern C++ best practices
