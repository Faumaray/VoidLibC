export module rt.env;

namespace rt {
    // internal storage set by runtime_entry
    static int g_argc = 0;
    static char** g_argv = nullptr;
    static char** g_envp = nullptr;

    // set by runtime_entry (not exported)
    export void _env_set(int argc, char** argv, char** envp) noexcept {
        g_argc = argc; g_argv = argv; g_envp = envp;
    }

    export int argc() noexcept { return g_argc; }
    export char** argv() noexcept { return g_argv; }
    export char** envp() noexcept { return g_envp; }

    // simple getenv (linear scan), returns pointer into envp
    export const char* getenv(const char* key) noexcept {
        if (!key) return nullptr;
        unsigned long klen = 0; while (key[klen]) ++klen;
        for (char** e = g_envp; e && *e; ++e) {
            const char* s = *e;
            unsigned long i = 0;
            while (i < klen && s[i] && s[i] == key[i]) ++i;
            if (i == klen && s[i] == '=') return s + i + 1;
        }
        return nullptr;
    }
}
