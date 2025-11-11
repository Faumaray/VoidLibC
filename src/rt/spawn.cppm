export module rt.spawn;

import rt.errno;
import rt.proc_exec;   // fork/execve/waitpid from your module

namespace rt::spawn {
    // spawn child to run path with argv/envp; parent returns child pid (>=0) or -1 on error
    export int spawn_pid(const char* path, char* const argv[], char* const envp[]) noexcept {
        int pid = rt::proc::fork();
        if (pid < 0) return -1;          // fork failed
        if (pid == 0) {
            // child
            rt::proc::execve(path, (char* const*)argv, (char* const*)envp);
            // if exec fails:
            rt::set_errno(127);
            __builtin_trap(); // or _exit(127) if you have it
        }
        return pid;
    }

    // spawn and wait (WNOHANG=0) → returns child exit code or -1
    export int spawn_wait(const char* path, char* const argv[], char* const envp[]) noexcept {
        int pid = spawn_pid(path, argv, envp);
        if (pid < 0) return -1;
        int st = 0;
        int r = rt::proc::waitpid(pid, &st, 0);
        if (r < 0) return -1;
        if ( (st & 0x7f) == 0 ) return (st >> 8) & 0xff;  // exit code
        return -1; // signaled
    }
}
