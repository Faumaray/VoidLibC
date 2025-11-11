module;
extern "C" int __vl_so_init();
extern "C" int __vl_so_fini();

// Optional DLL entry that forwards to your init/fini hooks.
// Safe no-ops if hooks are not defined elsewhere.
extern "C" int DllMain(void*, unsigned long reason, void*) {
    // 1=PROCESS_ATTACH, 0=PROCESS_DETACH on MinGW values
    if (reason == 1) { (void)__vl_so_init(); }
    else if (reason == 0) { (void)__vl_so_fini(); }
    return 1;
}
