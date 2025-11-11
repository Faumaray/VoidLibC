export module rt.abi;

export namespace rt::abi {
    using usize = __UINTPTR_TYPE__;
    using isize = __INTPTR_TYPE__;

    // argv/envp triple extracted from raw stack
    struct args {
        int argc;
        char** argv;
        char** envp;
    };
}
