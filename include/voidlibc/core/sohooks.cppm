export module voidlibc.core.sohooks;
export extern "C" {
    // Overridable weak hooks called on .so load/unload (all OSes)
    int __vl_so_init() __attribute__((weak));
    int __vl_so_fini() __attribute__((weak));
}
