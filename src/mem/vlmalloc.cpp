module voidlibc.c.memory;

import voidlibc.core.types;
import voidlibc.core.tls;        // vl_errno()
import voidlibc.sys.linux;       // scN, SYS_*
import voidlibc.thread.mutex;    // pthread_mutex_*
import voidlibc.thread.once;     // pthread_once

using namespace vl::sys::linux;

static constexpr __SIZE_TYPE__ PAGE_SIZE = 4096;
static constexpr __SIZE_TYPE__ MIN_ALIGN = 16;

static inline __SIZE_TYPE__ align_up(__SIZE_TYPE__ n, __SIZE_TYPE__ a) {
  return (n + (a - 1)) & ~(__SIZE_TYPE__)(a - 1);
}

/* -------- size classes (keep NCLASS==32 in sync with TLS heap[32]) -------- */
static constexpr __SIZE_TYPE__ CLASSES[] = {
  16, 32, 48, 64, 80, 96, 112, 128,
  160, 192, 224, 256, 320, 384, 448, 512,
  640, 768, 896, 1024, 1280, 1536, 1792, 2048,
  2560, 3072, 3584, 4096, 5120, 6144, 8192, 12288
};
static constexpr int NCLASS = (int)(sizeof(CLASSES)/sizeof(CLASSES[0]));

static inline int class_index(__SIZE_TYPE__ n) {
  if (n == 0) n = 1;
  for (int i = 0; i < NCLASS; ++i) if (n <= CLASSES[i]) return i;
  return -1;
}

/* -------- free-list node and TLS/global heads -------- */
struct FreeNode { FreeNode* next; };

static FreeNode* g_heads_fallback[NCLASS] = { nullptr };

static inline FreeNode** tls_heads() {
  void** h = __vl_tls_heap_heads();
  if (!h) return g_heads_fallback;
  return (FreeNode**)h;
}

/* -------- global lists + locks (short critical sections only) -------- */
struct GlobalLists { pthread_mutex_t lk[NCLASS]; FreeNode* head[NCLASS]; };
static GlobalLists g_global{};              // zeroed BSS

static pthread_once_t g_alloc_once{};

static void alloc_global_init() {
  for (int i = 0; i < NCLASS; ++i) {
    pthread_mutex_init(&g_global.lk[i], nullptr);
    g_global.head[i] = nullptr;
  }
}

static inline void glock(int ci)   { pthread_mutex_lock(&g_global.lk[ci]); }
static inline void gunlock(int ci) { pthread_mutex_unlock(&g_global.lk[ci]); }

/* -------- raw sys helpers -------- */
static inline void* sys_mmap(__SIZE_TYPE__ n, int prot, int flags) {
  long r = sc6(SYS_mmap, 0, (long)n, prot, flags, -1, 0);
  return (r < 0) ? (void*)0 : (void*)r;
}
static inline int sys_munmap(void* p, __SIZE_TYPE__ n) {
  long r = sc2(SYS_munmap, (long)p, (long)n);
  return (r < 0) ? -1 : 0;
}

/* -------- 16-byte header (shared for small/large) --------
   Small block:
     tag: 0xBEEF (any non-PAGE multiple)
     cls: size-class index (0..NCLASS-1)
   Large block (page-mapped or aligned):
     tag: mapping size in bytes (PAGE multiple)
     base_ptr: 8-byte base address of the mapping (for munmap)
---------------------------------------------------------------- */
struct SmallHdr {
  __SIZE_TYPE__   tag;     // small: marker (!PAGE multiple); large: mapping size (PAGE multiple)
  unsigned short  cls;     // small only
  unsigned short  pad16;
  unsigned int    pad32;
};
static_assert(sizeof(SmallHdr) == MIN_ALIGN, "SmallHdr must match MIN_ALIGN");

struct LargeHdr {
  __SIZE_TYPE__ size;
  void*         base;
};
static_assert(sizeof(LargeHdr) == MIN_ALIGN, "LargeHdr must match MIN_ALIGN");

static inline bool is_large_tag(__SIZE_TYPE__ tag) {
  return (tag & (PAGE_SIZE - 1)) == 0 && tag >= PAGE_SIZE;
}
static inline SmallHdr* hdr_from_user(void* p) {
  return (SmallHdr*)((char*)p - MIN_ALIGN);
}

/* -------- helpers to write headers -------- */
static inline void write_small_hdr(void* nd, int ci) {
  SmallHdr* h = (SmallHdr*)nd;
  h->tag = 0xBEEF;                 // not PAGE multiple
  h->cls = (unsigned short)ci;
}
static inline void write_large_hdr(void* hdr_addr, void* base, __SIZE_TYPE__ size) {
  LargeHdr* h = (LargeHdr*)hdr_addr;
  h->size = size;                  // PAGE multiple
  h->base = base;                  // mapping base for munmap
}

/* -------- large aligned allocator (used by aligned_alloc/posix_memalign) -------- */
static void* large_alloc_aligned(__SIZE_TYPE__ align, __SIZE_TYPE__ size) {
  if (align < MIN_ALIGN) align = MIN_ALIGN;
  if ((align & (align - 1)) != 0) return (void*)0;               // not power of two

  // Over-map so we can trim to get: header at H, user at R=H+MIN_ALIGN, R % align == 0
  __SIZE_TYPE__ over = size + align + MIN_ALIGN + PAGE_SIZE;
  void* raw = sys_mmap(over, 3 /*PROT_READ|WRITE*/, 0x22 /*MAP_PRIVATE|ANON*/);
  if (!raw) return (void*)0;

  __SIZE_TYPE__ raw_addr = (__SIZE_TYPE__)(raw);
  __SIZE_TYPE__ ret      = align_up(raw_addr + MIN_ALIGN, align);   // user pointer (aligned)
  __SIZE_TYPE__ hdr      = ret - MIN_ALIGN;                         // header lives here

  // Trim to page boundaries so only [keep_start, keep_end) remains mapped
  __SIZE_TYPE__ keep_start = (__SIZE_TYPE__)(hdr) & ~(__SIZE_TYPE__)(PAGE_SIZE - 1);
  __SIZE_TYPE__ keep_end   = align_up(ret + size, PAGE_SIZE);

  // Unmap prefix
  __SIZE_TYPE__ raw_end = raw_addr + over;
  if (keep_start > raw_addr) {
    sys_munmap((void*)raw_addr, keep_start - raw_addr);
  }
  // Unmap suffix
  if (raw_end > keep_end) {
    sys_munmap((void*)keep_end, raw_end - keep_end);
  }

  // Header for free()
  write_large_hdr((void*)hdr, (void*)keep_start, keep_end - keep_start);
  return (void*)ret;
}

/* -------- public API -------- */
extern "C" void* malloc(__SIZE_TYPE__ n) {
  pthread_once(&g_alloc_once, &alloc_global_init);

  if (n == 0) n = 1;
  __SIZE_TYPE__ need = align_up(n, MIN_ALIGN);

  int ci = class_index(need);
  if (ci < 0) {
    /* large (page-mapped by requested size) */
    __SIZE_TYPE__ sz = align_up(need + MIN_ALIGN, PAGE_SIZE);
    void* base = sys_mmap(sz, 3 /*PROT_READ|WRITE*/, 0x22 /*MAP_PRIVATE|ANON*/);
    if (!base) return (void*)0;
    write_large_hdr(base, base, sz);
    return (void*)((char*)base + MIN_ALIGN);
  }

  const __SIZE_TYPE__ chunk = CLASSES[ci];

  /* IMPORTANT: if chunk >= PAGE_SIZE, treat as large mapping (no small slab) */
  if (chunk >= PAGE_SIZE) {
    __SIZE_TYPE__ sz = align_up(need + MIN_ALIGN, PAGE_SIZE);
    void* base = sys_mmap(sz, 3, 0x22);
    if (!base) return (void*)0;
    write_large_hdr(base, base, sz);
    return (void*)((char*)base + MIN_ALIGN);
  }

  FreeNode** heads = tls_heads();

  /* fast local pop */
  if (FreeNode* nd = heads[ci]) {
    heads[ci] = nd->next;
    write_small_hdr(nd, ci);
    return (void*)((char*)nd + MIN_ALIGN);
  }

  /* try global pop (short critical section) */
  glock(ci);
  FreeNode* g = g_global.head[ci];
  if (g) g_global.head[ci] = g->next;
  gunlock(ci);
  if (g) {
    write_small_hdr(g, ci);
    return (void*)((char*)g + MIN_ALIGN);
  }

  /* slow path: build a slab (one page) unlocked, push into local, return one */
  __SIZE_TYPE__ slab = PAGE_SIZE;
  void* base = sys_mmap(slab, 3, 0x22);
  if (!base) return (void*)0;

  char* it  = (char*)base;
  char* end = (char*)base + slab;

  while (it + chunk <= end) {
    FreeNode* nd = (FreeNode*)it;
    nd->next = heads[ci];
    heads[ci] = nd;
    it += chunk;
  }

  /* after carving, there must be at least one node */
  FreeNode* nd = heads[ci];
  // heads[ci] must not be null here; but guard defensively
  if (!nd) {
    // extremely unlikely; fallback to large to avoid crash
    void* b2 = sys_mmap(align_up(need + MIN_ALIGN, PAGE_SIZE), 3, 0x22);
    if (!b2) return (void*)0;
    write_large_hdr(b2, b2, align_up(need + MIN_ALIGN, PAGE_SIZE));
    return (void*)((char*)b2 + MIN_ALIGN);
  }
  heads[ci] = nd->next;
  write_small_hdr(nd, ci);
  return (void*)((char*)nd + MIN_ALIGN);
}

extern "C" void* calloc(__SIZE_TYPE__ count, __SIZE_TYPE__ size) {
  // Avoid overflow: if count * size would wrap, fail with ENOMEM.
  if (count && size > (__SIZE_TYPE__)-1 / count) {
    vl_errno() = 12; // ENOMEM
    return (void*)0;
  }

  __SIZE_TYPE__ total = count * size;
  void* p = malloc(total);
  if (!p) {
    vl_errno() = 12; // ENOMEM
    return (void*)0;
  }

  unsigned char* bytes = (unsigned char*)p;
  for (__SIZE_TYPE__ i = 0; i < total; ++i) {
    bytes[i] = 0;
  }
  return p;
}

extern "C" void free(void* p) {
  if (!p) return;

  SmallHdr* sh = hdr_from_user(p);
  __SIZE_TYPE__ tag = sh->tag;

  if (is_large_tag(tag)) {
    /* large mapping: header actually holds {size, base} */
    LargeHdr* lh = (LargeHdr*)sh;
    if (lh->base && lh->size) sys_munmap(lh->base, lh->size);
    return;
  }

  int ci = (int)sh->cls;
  if (ci < 0 || ci >= NCLASS) return;  // corrupted; ignore

  FreeNode* nd = (FreeNode*)sh;

  /* push to GLOBAL list (short critical section) */
  glock(ci);
  nd->next = g_global.head[ci];
  g_global.head[ci] = nd;
  gunlock(ci);
}

extern "C" void* realloc(void* p, __SIZE_TYPE__ n) {
  if (!p) return malloc(n);
  if (n == 0) { free(p); return (void*)0; }

  SmallHdr* sh = hdr_from_user(p);
  __SIZE_TYPE__ tag = sh->tag;

  if (is_large_tag(tag)) {
    /* large: remap by copy */
    LargeHdr* lh = (LargeHdr*)sh;
    void* np = malloc(n);
    if (!np) return (void*)0;

    __SIZE_TYPE__ old = lh->size - MIN_ALIGN;
    __SIZE_TYPE__ to  = (old < n) ? old : n;

    char* dst = (char*)np;
    char* src = (char*)p;
    for (__SIZE_TYPE__ i = 0; i < to; ++i) dst[i] = src[i];

    sys_munmap(lh->base, lh->size);
    return np;
  } else {
    /* small → copy up to old class size (if recognized) */
    int ci = (int)sh->cls;
    __SIZE_TYPE__ old = (ci >= 0 && ci < NCLASS) ? CLASSES[ci] : (__SIZE_TYPE__)0;

    void* np = malloc(n);
    if (!np) return (void*)0;

    __SIZE_TYPE__ to = old ? ((old < n) ? old : n) : n;

    char* dst = (char*)np;
    char* src = (char*)p;
    for (__SIZE_TYPE__ i = 0; i < to; ++i) dst[i] = src[i];

    free(p);
    return np;
  }
}

/* -------- aligned_alloc (C11): size must be a multiple of alignment -------- */
extern "C" void* aligned_alloc(__SIZE_TYPE__ alignment, __SIZE_TYPE__ size) {
  if (alignment < MIN_ALIGN || (alignment & (alignment - 1))) {
    vl_errno() = 22; // EINVAL
    return (void*)0;
  }
  if ((size & (alignment - 1)) != 0) {
    vl_errno() = 22; // EINVAL: size not multiple of alignment
    return (void*)0;
  }

  if (alignment <= MIN_ALIGN) {
    // our small/large alloc already guarantees MIN_ALIGN
    return malloc(size);
  }

  // Use large aligned mapping so free() works (header carries base+size)
  void* p = large_alloc_aligned(alignment, size);
  if (!p) { vl_errno() = 12; } // ENOMEM
  return p;
}

/* -------- posix_memalign: power-of-two align >= sizeof(void*) -------- */
extern "C" int posix_memalign(void** out, __SIZE_TYPE__ alignment, __SIZE_TYPE__ size) {
  if (!out) return 22; // EINVAL
  if (alignment < sizeof(void*) || (alignment & (alignment - 1))) return 22; // EINVAL

  if (alignment <= MIN_ALIGN) {
    void* p = malloc(size);
    if (!p) return 12; // ENOMEM
    *out = p;
    return 0;
  }

  // No "size multiple-of-alignment" requirement here
  void* p = large_alloc_aligned(alignment, size);
  if (!p) return 12; // ENOMEM
  *out = p;
  return 0;
}
