/************************************************************************
 * TSan visibility probe for areg's custom Mutex.
 * ---------------------------------------------------------------------
 * Two threads increment a shared counter, each access guarded by a single
 * shared areg::Mutex. The program is race-FREE by construction.
 *
 * Build it with ThreadSanitizer against the TSan-instrumented libareg and run:
 *   - If TSan reports a data race on g_shared, it proves TSan cannot observe
 *     areg's futex/__ulock-based Mutex handoff as a happens-before edge -> every
 *     "race" TSan reports on areg-Mutex-protected data is a FALSE POSITIVE.
 *   - If TSan stays silent, areg's Mutex IS visible to TSan and such reports
 *     would be genuine.
 *
 * Compile (see tools/sanitize.sh for the libareg path):
 *   g++ -std=c++17 -fsanitize=thread -g -I framework tools/sanitizer/tsan_lock_probe.cpp \
 *       <libareg.a> -lpthread -lrt -o /tmp/tsan_lock_probe
 ************************************************************************/
#include <thread>
#include <cstdio>

// Build with -DUSE_STD_MUTEX to validate the harness with a known-good std::mutex
// (must yield exactly 400000); default uses areg::Mutex.
#if defined(USE_STD_MUTEX)
#  include <mutex>
#else
#  include "areg/base/SyncPrimitives.hpp"
#endif

namespace
{
#if defined(USE_STD_MUTEX)
    using LockType = std::mutex;
#else
    using LockType = areg::Mutex;
#endif

    int g_shared{ 0 };

    void worker( LockType& lk )
    {
        for ( int i = 0; i < 200000; ++i )
        {
#if defined(USE_STD_MUTEX)
            std::lock_guard<std::mutex> lock( lk );
#else
            areg::Lock lock( lk );
#endif
            ++g_shared;                // protected by lk on every access
        }
    }
}

int main()
{
    // Construct the lock locally (after main starts) to avoid any static-init-order
    // confound. areg::Mutex(false) = created, not initially owned.
#if defined(USE_STD_MUTEX)
    LockType lk;
#else
    LockType lk( false );
#endif
    std::thread a( worker, std::ref( lk ) );
    std::thread b( worker, std::ref( lk ) );
    a.join();
    b.join();
    std::printf( "g_shared = %d (expected 400000)\n", g_shared );
    return 0;
}
