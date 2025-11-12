export module voidlibc.c.proc;
export import voidlibc.core.types;

export extern "C" {

    int  getpid() noexcept;
    int  getppid() noexcept;
    int  gettid() noexcept;

    int  kill(int pid, int sig) noexcept;

    int  fork() noexcept;
    int  execve(const char* path, char* const argv[], char* const envp[]) noexcept;

    // waitpid() subset
    inline constexpr int WNOHANG   = 1;
    inline constexpr int WUNTRACED = 2;

    int  waitpid(int pid, int* wstatus, int options) noexcept;

} // extern "C"
