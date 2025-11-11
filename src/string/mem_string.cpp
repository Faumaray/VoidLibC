module voidlibc.c.string;
import voidlibc.core.types;

extern "C" void* memcpy(void* d, const void* s, size_t n){
    auto* dst=(unsigned char*)d; const auto* src=(const unsigned char*)s;
    for(size_t i=0;i<n;++i){ dst[i]=src[i]; }
    return d;
}
extern "C" void* memmove(void* d, const void* s, size_t n){
    auto* dst=(unsigned char*)d; const auto* src=(const unsigned char*)s;
    if (dst<=src){ for(size_t i=0;i<n;++i){ dst[i]=src[i]; } }
    else { for(size_t i=n;i>0;--i){ dst[i-1]=src[i-1]; } }
    return d;
}
extern "C" void* memset(void* d, int c, size_t n){
    auto* p=(unsigned char*)d; for(size_t i=0;i<n;++i){ p[i]=(unsigned char)c; }
    return d;
}
extern "C" size_t strlen(const char* s){
    size_t i=0; while(s[i]) ++i; return i;
}
extern "C" int strcmp(const char* a, const char* b){
    for(;*a && (*a==*b); ++a,++b){} return (unsigned char)*a - (unsigned char)*b;
}
