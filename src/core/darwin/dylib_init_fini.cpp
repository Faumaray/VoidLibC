module;
// weak init/fini you can override
extern "C" int __vl_so_init()  __attribute__((weak));
extern "C" int __vl_so_fini()  __attribute__((weak));

__attribute__((constructor))
static void __vl_ctor() { if (__vl_so_init)  (void)__vl_so_init(); }

__attribute__((destructor))
static void __vl_dtor() { if (__vl_so_fini)  (void)__vl_so_fini(); }
