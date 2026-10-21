/************************************************************************
 * Test of the fail-fast path when the system cannot create a lock (Linux).
 *
 * The executable defines pthread_mutex_init(), so the areg library calls this one: it fails
 * while 'gFailMutexInit' is set and forwards to the next definition (the C library, or a
 * sanitizer) otherwise. A child process sets the flag and creates an areg::Mutex. The test
 * passes when the child is aborted with the reason on its standard error output: areg never
 * hands out a lock that cannot lock.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncPrimitives.hpp"

#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <string>

namespace
{
    //!< pthread_mutex_init() fails while this is set.
    volatile bool gFailMutexInit{ false };

    using MutexInit = int (*)( pthread_mutex_t *, const pthread_mutexattr_t * );
}

//!< Exported and kept, so the library resolves pthread_mutex_init() to this definition.
extern "C" __attribute__((visibility("default"), used))
int pthread_mutex_init( pthread_mutex_t * mutex, const pthread_mutexattr_t * attr )
{
    if ( gFailMutexInit )
        return EAGAIN;

    static MutexInit next{ reinterpret_cast<MutexInit>(::dlsym( RTLD_NEXT, "pthread_mutex_init" )) };
    return ( next != nullptr ) ? next( mutex, attr ) : ENOSYS;
}

int main( )
{
    int pipeFds[2]{ -1, -1 };
    if ( ::pipe( pipeFds ) != 0 )
    {
        std::perror( "pipe" );
        return 1;
    }

    const pid_t child{ ::fork( ) };
    if ( child == 0 )
    {
        ::dup2( pipeFds[1], STDERR_FILENO );
        ::close( pipeFds[0] );

        // Restores the default SIGABRT action, which a sanitizer replaces with its own report.
        struct sigaction defaultAbort{ };
        defaultAbort.sa_handler = SIG_DFL;
        ::sigaction( SIGABRT, &defaultAbort, nullptr );
        gFailMutexInit = true;
        areg::Mutex mutex;
        static_cast<void>(mutex.lock( ));
        std::fprintf( stderr, "the Mutex was created\n" );
        ::_exit( 0 );
    }

    ::close( pipeFds[1] );
    std::string output;
    char buffer[256];
    ssize_t got{ 0 };
    while ( ( got = ::read( pipeFds[0], buffer, sizeof( buffer ) ) ) > 0 )
        output.append( buffer, static_cast<size_t>(got) );
    ::close( pipeFds[0] );

    int status{ 0 };
    ::waitpid( child, &status, 0 );

    const bool aborted{ WIFSIGNALED( status ) && ( WTERMSIG( status ) == SIGABRT ) };
    const bool reason{ output.find( "areg fatal error: pthread_mutex_init failed" ) != std::string::npos };
    std::printf( "fatal create test: child %s, reason %s\n", aborted ? "aborted" : "NOT aborted", reason ? "written" : "MISSING" );
    if ( ( aborted == false ) || ( reason == false ) )
        std::printf( "child output: %s\n", output.c_str( ) );

    return ( aborted && reason ) ? 0 : 1;
}
