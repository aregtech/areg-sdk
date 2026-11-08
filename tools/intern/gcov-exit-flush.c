/* ===========================================================================
 * Areg SDK - coverage counters of a process that ends without exit()
 * ---------------------------------------------------------------------------
 * Preloaded by coverage.sh in the test and example runs of a GCC build.
 * _Exit and abort write the gcov counters of every loaded module, then end
 * the process the way the C library does.
 *
 * Build: cc -shared -fPIC -O1 -o gcov-exit-flush.so gcov-exit-flush.c -lgcov
 * ===========================================================================
 */
#include <signal.h>
#include <unistd.h>

extern void __gcov_dump(void);

void _Exit(int code)
{
    __gcov_dump();
    _exit(code);
}

void abort(void)
{
    sigset_t mask;
    __gcov_dump();
    sigemptyset(&mask);
    sigaddset(&mask, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    raise(SIGABRT);
    signal(SIGABRT, SIG_DFL);
    raise(SIGABRT);
    _exit(127);
}
