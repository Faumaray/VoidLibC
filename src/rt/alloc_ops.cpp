// Non-module TU that provides the global operators.
// It imports the rt.alloc module and calls its helpers.

import rt.alloc;

using size_t = __SIZE_TYPE__;

void* operator new(size_t n) noexcept { return rt::alloc::operator_new(n); }
void  operator delete(void* p) noexcept { rt::alloc::operator_delete(p); }

void* operator new[](size_t n) noexcept { return rt::alloc::operator_new(n); }
void  operator delete[](void* p) noexcept { rt::alloc::operator_delete(p); }

void* operator new(size_t, void* p) noexcept { return p; }
void  operator delete(void*, void*) noexcept {}

void* operator new[](size_t, void* p) noexcept { return p; }
void  operator delete[](void*, void*) noexcept {}
