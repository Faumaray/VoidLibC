module voidlibc.thread.pthread;

import voidlibc.core.types;
import voidlibc.core.tls;
import voidlibc.sys.linux;
import voidlibc.c.memory;

using namespace vl::sys::linux;

static constexpr int FUTEX_WAIT = 0;
static constexpr int FUTEX_WAKE = 1;

static constexpr unsigned long CLONE_VM      = 0x00000100;
static constexpr unsigned long CLONE_FS      = 0x00000200;
static constexpr unsigned long CLONE_FILES   = 0x00000400;
static constexpr unsigned long CLONE_SIGHAND = 0x00000800;
static constexpr unsigned long CLONE_THREAD  = 0x00010000;
static constexpr unsigned long CLONE_SYSVSEM = 0x00040000;
static constexpr unsigned long CLONE_PARENT_SETTID = 0x00100000;
static constexpr unsigned long CLONE_CHILD_CLEARTID= 0x00200000;
static constexpr unsigned long CLONE_CHILD_SETTID  = 0x01000000;

struct ThreadBlock {
  void* (*fn)(void*);
  void*  arg;
  void*  result;
  void*  stack_base;
  size_t stack_size;

  int    child_tid;   // kernel writes TID here, clears to 0 on exit (CLONE_*TID)
  int    started;     // 0 until child sets it to 1, then WAKEs
  int    pad[2];
};


// Assembly trampoline we jump to in the child; never returns.
extern "C" void vl_thread_trampoline(void* blk) __attribute__((noreturn));

// The C entry that runs on the child stack; called by the trampoline.
extern "C" long vl_thread_entry(ThreadBlock* blk) {
  // Signal parent that we’re alive before calling user code
  __atomic_store_n(&blk->started, 1, __ATOMIC_RELEASE);
  (void)sc4(SYS_futex, (long)&blk->started, FUTEX_WAKE | 128 /*PRIVATE*/, 1, 0);

  void* r = blk->fn(blk->arg);
  blk->result = r;
  // kernel will CLEAR+WAKE child_tid on SYS_exit in the trampoline
  return 0;
}


extern "C" int pthread_create(pthread_t* t, const pthread_attr_t /*attr*/,
                       pthread_start_routine start, void* arg) noexcept {
  // Map a 2 MiB stack for the child
  size_t stack_sz = 2u * 1024u * 1024u;
  unsigned char* stack = (unsigned char*)sc6(SYS_mmap, 0, (long)stack_sz, 3, 0x22, -1, 0);
  if ((long)stack < 0) { vl_errno() = -(long)stack; return -1; }

  // Control block
  ThreadBlock* blk = (ThreadBlock*)sc6(SYS_mmap, 0, 4096, 3, 0x22, -1, 0);
  if ((long)blk < 0) { vl_errno() = -(long)blk; return -1; }

  blk->fn = start; blk->arg = arg; blk->result = nullptr;
  blk->stack_base = stack; blk->stack_size = stack_sz;
  blk->child_tid = 0;
  blk->started   = 0;
  blk->pad[0] = blk->pad[1] = 0;                  // kernel will set to TID on start

  // Align child stack to 16B and place blk pointer on it
  unsigned char* child_sp = stack + stack_sz;
  child_sp = (unsigned char*)(((unsigned long)child_sp) & ~15ul);
  child_sp -= sizeof(void*);
  *(void**)child_sp = (void*)blk;

  // flags: user thread + clear/set tid plumbing for join
  unsigned long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                      CLONE_THREAD | CLONE_SYSVSEM |
                      CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID;

  // clone(flags, child_stack, parent_tidptr, child_tidptr, tls)
  long r = sc5(SYS_clone, (long)flags, (long)child_sp,
             (long)&blk->child_tid,  // parent_tidptr
             (long)&blk->child_tid,  // child_tidptr
             0);                     // tls (FS set in trampoline)
  if (r < 0) { vl_errno() = -r; return -1; }

  if (r == 0) {
    asm volatile(
      "mov (%%rsp), %%rdi \n\t"
      "add $8, %%rsp      \n\t"
      "jmp vl_thread_trampoline \n\t"
      : : : "rdi","memory"
    );
    __builtin_unreachable();
  }


  *t = (void*)blk;
  return 0;
}

extern "C" int pthread_join(void* t, void** retval) noexcept {
  ThreadBlock* blk = (ThreadBlock*)t;

  // Phase 1: ensure the thread has either started (started==1)
  // or already exited (child_tid==0). If not, wait on `started`.
  for (;;) {
    int st = __atomic_load_n(&blk->started, __ATOMIC_ACQUIRE);
    int ct = __atomic_load_n(&blk->child_tid, __ATOMIC_ACQUIRE);
    if (st || ct == 0) break;
    (void)sc4(SYS_futex, (long)&blk->started, FUTEX_WAIT | 128 /*PRIVATE*/, 0, 0);
  }

  // Phase 2: if it started, wait for kernel to clear child_tid to 0 on exit.
  for (;;) {
    int ct = __atomic_load_n(&blk->child_tid, __ATOMIC_ACQUIRE);
    if (ct == 0) break;
    (void)sc4(SYS_futex, (long)&blk->child_tid, FUTEX_WAIT, ct, 0);
  }

  if (retval) *retval = blk->result;

  // Now it's safe to unmap: the thread is gone and won't touch its stack.
  (void)sc2(SYS_munmap, (long)blk->stack_base, (long)blk->stack_size);
  (void)sc2(SYS_munmap, (long)blk, 4096);
  return 0;
}


