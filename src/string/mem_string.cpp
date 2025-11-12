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
extern "C" int memcmp(const void* a, const void* b, size_t n){
    const auto* pa=(const unsigned char*)a;
    const auto* pb=(const unsigned char*)b;
    for(size_t i=0;i<n;++i){
        if(pa[i]!=pb[i]) return (int)pa[i] - (int)pb[i];
    }
    return 0;
}
extern "C" void* memchr(const void* s, int c, size_t n){
    const auto* ps=(const unsigned char*)s;
    unsigned char ch=(unsigned char)c;
    for(size_t i=0;i<n;++i){
        if(ps[i]==ch) return (void*)(ps+i);
    }
    return nullptr;
}
extern "C" void* memset(void* d, int c, size_t n){
    auto* p=(unsigned char*)d; for(size_t i=0;i<n;++i){ p[i]=(unsigned char)c; }
    return d;
}
extern "C" char* strcpy(char* d, const char* s){
    auto* dst=d; do{ *dst++=*s; }while(*s++);
    return d;
}
extern "C" size_t strlen(const char* s){
    size_t i=0; while(s[i]) ++i; return i;
}
extern "C" int strcmp(const char* a, const char* b){
    for(;*a && (*a==*b); ++a,++b){} return (unsigned char)*a - (unsigned char)*b;
}
extern "C" int strncmp(const char* a, const char* b, size_t n){
    for(size_t i=0;i<n;++i){
        unsigned char ca=(unsigned char)a[i];
        unsigned char cb=(unsigned char)b[i];
        if(ca!=cb || ca==0 || cb==0){
            return ca-cb;
        }
    }
    return 0;
}
extern "C" char* strcpy(char* d, const char* s){
    char* r=d;
    while((*d++=*s++)!=0){}
    return r;
}
extern "C" char* strncpy(char* d, const char* s, size_t n){
    char* r=d;
    size_t i=0;
    for(;i<n && s[i];++i){ d[i]=s[i]; }
    for(;i<n;++i){ d[i]='\0'; }
    return r;
}
extern "C" char* strcat(char* d, const char* s){
    char* r=d;
    while(*d) ++d;
    while((*d++=*s++)!=0){}
    return r;
}
extern "C" char* strncat(char* d, const char* s, size_t n){
    char* r=d;
    while(*d) ++d;
    size_t i=0;
    for(;i<n && s[i];++i){ d[i]=s[i]; }
    d[i]='\0';
    return r;
}
extern "C" char* strchr(const char* s, int c){
    char ch=(char)c;
    for(;*s;++s){ if(*s==ch) return (char*)s; }
    if(ch=='\0') return (char*)s;
    return nullptr;
}
extern "C" char* strrchr(const char* s, int c){
    char ch=(char)c;
    const char* last=nullptr;
    for(;*s;++s){ if(*s==ch) last=s; }
    if(ch=='\0') return (char*)s;
    return (char*)last;
}
extern "C" size_t strnlen(const char* s, size_t maxlen){
    size_t i=0;
    for(;i<maxlen && s[i];++i){}
    return i;
}
extern "C" size_t strspn(const char* s, const char* accept){
    size_t count=0;
    for(;*s;++s){
        bool ok=false;
        for(const char* a=accept; *a; ++a){ if(*a==*s){ ok=true; break; } }
        if(!ok) break;
        ++count;
    }
    return count;
}
extern "C" size_t strcspn(const char* s, const char* reject){
    size_t count=0;
    for(;*s;++s){
        bool hit=false;
        for(const char* r=reject; *r; ++r){ if(*r==*s){ hit=true; break; } }
        if(hit) break;
        ++count;
    }
    return count;
}
extern "C" char* strpbrk(const char* s, const char* accept){
    for(;*s;++s){
        for(const char* a=accept; *a; ++a){ if(*a==*s) return (char*)s; }
    }
    return nullptr;
}
extern "C" char* strstr(const char* h, const char* n){
    if(!*n) return (char*)h;
    size_t needle_len=strlen(n);
    for(;*h;++h){
        if(h[0]==n[0] && strncmp(h, n, needle_len)==0){
            return (char*)h;
        }
        if(h[0]=='\0') break;
    }
    return nullptr;
}
extern "C" char* strtok(char* str, const char* delim){
    static char* save=nullptr;
    if(!str) str=save;
    if(!str) return nullptr;

    str+=strspn(str, delim);
    if(*str=='\0'){ save=nullptr; return nullptr; }

    char* end=str+strcspn(str, delim);
    if(*end){ *end='\0'; save=end+1; }
    else { save=nullptr; }
    return str;
}
