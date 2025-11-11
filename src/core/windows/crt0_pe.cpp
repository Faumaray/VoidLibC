module;
// Windows import decls (no headers)
extern "C" __attribute__((dllimport)) void  ExitProcess(unsigned int);
extern "C" __attribute__((dllimport)) void* GetCommandLineW(void);
// NOTE: we intentionally avoid parsing argv/env for now (pass nullptrs)

extern "C" int main(int, char**, char**);
extern "C" [[noreturn]] void exit(int);

extern "C" void mainCRTStartup() {
    // minimal: argc=0, argv/env = nullptr
    int code = main(0, (char**)0, (char**)0);
    ExitProcess((unsigned)code);
    __builtin_trap();
}

extern "C" int _DllMainCRTStartup(void*, unsigned long, void*) {
    // If this object is linked as a DLL, provide a benign entry.
    return 1;
}
