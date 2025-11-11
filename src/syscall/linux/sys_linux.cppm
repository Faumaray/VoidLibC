export module voidlibc.sys.linux;
import voidlibc.core.types;

export namespace vl::sys::linux {
  enum : long {
    SYS_read=0, SYS_write=1, SYS_open=2, SYS_close=3,
    SYS_lseek=8,
    SYS_mmap=9, SYS_mprotect=10, SYS_munmap=11,
    SYS_nanosleep=35,
    SYS_exit=60,
    SYS_arch_prctl=158,
    SYS_gettimeofday=96,
    SYS_futex=202,
    SYS_clock_gettime=228,
    SYS_openat=257,
    SYS_clone=56
  };

  [[gnu::always_inline]] inline long sc1(long n, long a){
    register long rax asm("rax") = n;
    register long rdi asm("rdi") = a;
    asm volatile("syscall" : "+a"(rax) : "D"(rdi) : "rcx","r11","cc","memory");
    return rax;
  }
  [[gnu::always_inline]] inline long sc2(long n, long a,long b){
    register long rax asm("rax") = n;
    register long rdi asm("rdi") = a;
    register long rsi asm("rsi") = b;
    asm volatile("syscall" : "+a"(rax) : "D"(rdi),"S"(rsi) : "rcx","r11","cc","memory");
    return rax;
  }
  [[gnu::always_inline]] inline long sc3(long n, long a,long b,long c){
    register long rax asm("rax") = n;
    register long rdi asm("rdi") = a;
    register long rsi asm("rsi") = b;
    register long rdx asm("rdx") = c;
    asm volatile("syscall" : "+a"(rax) : "D"(rdi),"S"(rsi),"d"(rdx) : "rcx","r11","cc","memory");
    return rax;
  }
  [[gnu::always_inline]] inline long sc4(long n, long a,long b,long c,long d){
    register long rax asm("rax") = n;
    register long rdi asm("rdi") = a;
    register long rsi asm("rsi") = b;
    register long rdx asm("rdx") = c;
    register long r10 asm("r10") = d;
    asm volatile("syscall" : "+a"(rax) : "D"(rdi),"S"(rsi),"d"(rdx),"r"(r10)
                 : "rcx","r11","cc","memory");
    return rax;
  }
  [[gnu::always_inline]] inline long sc5(long n, long a,long b,long c,long d,long e){
    register long rax asm("rax") = n;
    register long rdi asm("rdi") = a;
    register long rsi asm("rsi") = b;
    register long rdx asm("rdx") = c;
    register long r10 asm("r10") = d;
    register long r8  asm("r8")  = e;
    asm volatile("syscall" : "+a"(rax) : "D"(rdi),"S"(rsi),"d"(rdx),"r"(r10),"r"(r8)
                 : "rcx","r11","cc","memory");
    return rax;
  }
  [[gnu::always_inline]] inline long sc6(long n, long a,long b,long c,long d,long e,long f){
    register long rax asm("rax") = n;
    register long rdi asm("rdi") = a;
    register long rsi asm("rsi") = b;
    register long rdx asm("rdx") = c;
    register long r10 asm("r10") = d;
    register long r8  asm("r8")  = e;
    register long r9  asm("r9")  = f;
    asm volatile("syscall" : "+a"(rax) : "D"(rdi),"S"(rsi),"d"(rdx),"r"(r10),"r"(r8),"r"(r9)
                 : "rcx","r11","cc","memory");
    return rax;
  }
}