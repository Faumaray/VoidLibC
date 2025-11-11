extern "C" void* malloc(__SIZE_TYPE__) noexcept;
extern "C" void  free(void*) noexcept;

// Freestanding: make default operator new non-throwing.
void* operator new(__SIZE_TYPE__ n) { return malloc(n > 0 ? n : static_cast<__SIZE_TYPE__>(1)); }
void  operator delete(void* p) noexcept      { if (p) free(p); }
void  operator delete(void* p, __SIZE_TYPE__) noexcept { if (p) free(p); }
