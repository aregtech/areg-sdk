/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. The fault library: replaces the system calls of
 *              FaultInject.hpp on Linux, macOS and Windows.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
// open() and open64() are both defined here, so the headers must not rename open() to open64().
#if defined(__linux__)
    #undef _FILE_OFFSET_BITS
#endif  // defined(__linux__)

#if defined(_WIN32)
    #include <WinSock2.h>
    #include <Windows.h>
    #include <Psapi.h>
#else   // defined(_WIN32)
    #include <dirent.h>
    #include <errno.h>
    #include <fcntl.h>
#if defined(__linux__)
    #include <poll.h>
    #include <sys/epoll.h>
    #include <sys/eventfd.h>
    #include <termios.h>
#endif  // defined(__linux__)
    #include <pthread.h>
    #include <spawn.h>
    #include <stdarg.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif  // defined(_WIN32)

#include "fault-inject/FaultInject.hpp"

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <new>

//////////////////////////////////////////////////////////////////////////
// The fault points shared by every platform.
//////////////////////////////////////////////////////////////////////////

namespace
{
    constexpr uint32_t POINT_COUNT{ static_cast<uint32_t>(areg_fault::FaultPoint::Count) };

    //!< The value of 'remaining' that fails every call.
    constexpr uint32_t FAIL_ALWAYS{ ~0u };

    //!< The state of one fault point.
    struct FaultSlot
    {
        std::atomic<bool>       armed       { false };  //!< True while the point is armed.
        std::atomic<int>        error       { 0 };      //!< The error of a failed call.
        std::atomic<uint32_t>   skip        { 0u };     //!< Calls that still pass before the first failure.
        std::atomic<uint32_t>   remaining   { 0u };     //!< Calls that still fail, or FAIL_ALWAYS.
        std::atomic<uint32_t>   failed      { 0u };     //!< Calls failed since the point was armed.
        std::atomic<uint32_t>   calls       { 0u };     //!< Calls made since the point was armed.
        std::atomic<uint32_t>   delay       { 0u };     //!< Milliseconds a passing call waits before it runs.
    };

    FaultSlot   gSlots[POINT_COUNT];

    //!< Returns true and the error if this call of the point fails.
    bool _should_fail( areg_fault::FaultPoint point, int & error ) noexcept
    {
        FaultSlot & slot{ gSlots[static_cast<uint32_t>(point)] };
        if ( slot.armed.load( std::memory_order_acquire ) == false )
            return false;

        slot.calls.fetch_add( 1u, std::memory_order_relaxed );
        uint32_t skip{ slot.skip.load( std::memory_order_relaxed ) };
        while ( skip != 0u )
        {
            if ( slot.skip.compare_exchange_weak( skip, skip - 1u, std::memory_order_relaxed ) )
                return false;
        }

        uint32_t remaining{ slot.remaining.load( std::memory_order_relaxed ) };
        while ( remaining != FAIL_ALWAYS )
        {
            if ( remaining == 0u )
                return false;

            if ( slot.remaining.compare_exchange_weak( remaining, remaining - 1u, std::memory_order_relaxed ) )
                break;
        }

        error = slot.error.load( std::memory_order_relaxed );
        slot.failed.fetch_add( 1u, std::memory_order_relaxed );
        return true;
    }

    //!< Returns true if the point is replaced on this platform.
    bool _is_supported( areg_fault::FaultPoint point ) noexcept;

    //!< Replaces the system calls before the first point is armed.
    void _install( ) noexcept;
}

extern "C"
{
    AREG_FAULT_API bool areg_fault_arm( uint32_t point, int error, uint32_t skip, uint32_t count )
    {
        if ( (point >= POINT_COUNT) || (_is_supported( static_cast<areg_fault::FaultPoint>(point) ) == false) )
            return false;

        _install( );
        FaultSlot & slot{ gSlots[point] };
        slot.armed.store( false, std::memory_order_relaxed );
        slot.error.store( error, std::memory_order_relaxed );
        slot.skip.store( skip, std::memory_order_relaxed );
        slot.remaining.store( count == 0u ? FAIL_ALWAYS : count, std::memory_order_relaxed );
        slot.failed.store( 0u, std::memory_order_relaxed );
        slot.calls.store( 0u, std::memory_order_relaxed );
        slot.armed.store( true, std::memory_order_release );
        return true;
    }

    AREG_FAULT_API void areg_fault_disarm_all( void )
    {
        for ( FaultSlot & slot : gSlots )
        {
            slot.armed.store( false, std::memory_order_release );
            slot.failed.store( 0u, std::memory_order_relaxed );
            slot.calls.store( 0u, std::memory_order_relaxed );
            slot.delay.store( 0u, std::memory_order_relaxed );
        }
    }

    AREG_FAULT_API uint32_t areg_fault_failed( uint32_t point )
    {
        return ( point < POINT_COUNT ) ? gSlots[point].failed.load( std::memory_order_relaxed ) : 0u;
    }

    AREG_FAULT_API uint32_t areg_fault_calls( uint32_t point )
    {
        return ( point < POINT_COUNT ) ? gSlots[point].calls.load( std::memory_order_relaxed ) : 0u;
    }

    AREG_FAULT_API bool areg_fault_delay( uint32_t point, uint32_t milliseconds )
    {
        if ( (static_cast<areg_fault::FaultPoint>(point) != areg_fault::FaultPoint::SocketListen)
            || (_is_supported( areg_fault::FaultPoint::SocketListen ) == false) )
            return false;

        _install( );
        gSlots[point].delay.store( milliseconds, std::memory_order_relaxed );
        return true;
    }
}

//////////////////////////////////////////////////////////////////////////
// Linux: definitions that win over the C library, forwarding to the next definition.
//////////////////////////////////////////////////////////////////////////
#if defined(__linux__)

#include <dlfcn.h>

namespace
{
    bool _is_supported( areg_fault::FaultPoint /*point*/ ) noexcept
    {
        return true;
    }

    void _install( ) noexcept
    {
    }

    //!< Returns the next definition of the named function after this library.
    template<typename Function>
    Function _next( const char * name ) noexcept
    {
        return reinterpret_cast<Function>(::dlsym( RTLD_NEXT, name ));
    }

    //!< Reads the mode argument of open() if the flags require one.
    mode_t _open_mode( int flags, va_list args ) noexcept
    {
#if defined(O_TMPFILE)
        const bool hasMode{ ((flags & O_CREAT) != 0) || ((flags & O_TMPFILE) == O_TMPFILE) };
#else   // defined(O_TMPFILE)
        const bool hasMode{ (flags & O_CREAT) != 0 };
#endif  // defined(O_TMPFILE)
        return hasMode ? static_cast<mode_t>(va_arg( args, int )) : static_cast<mode_t>(0);
    }

    using OpenFunc = int (*)( const char *, int, ... );
}

extern "C"
{
    AREG_FAULT_API int pthread_create( pthread_t * thread, const pthread_attr_t * attr, void * (*routine)(void *), void * arg ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::ThreadCreate, error ) )
            return error;

        using Func = int (*)( pthread_t *, const pthread_attr_t *, void * (*)(void *), void * );
        static const Func next{ _next<Func>( "pthread_create" ) };
        return ( next != nullptr ) ? next( thread, attr, routine, arg ) : ENOSYS;
    }

    AREG_FAULT_API int socket( int domain, int type, int protocol ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketCreate, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, int, int );
        static const Func next{ _next<Func>( "socket" ) };
        return ( next != nullptr ) ? next( domain, type, protocol ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int open( const char * path, int flags, ... )
    {
        va_list args;
        va_start( args, flags );
        const mode_t mode{ _open_mode( flags, args ) };
        va_end( args );

        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::FileOpen, error ) )
        {
            errno = error;
            return -1;
        }

        static const OpenFunc next{ _next<OpenFunc>( "open" ) };
        return ( next != nullptr ) ? next( path, flags, mode ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int open64( const char * path, int flags, ... )
    {
        va_list args;
        va_start( args, flags );
        const mode_t mode{ _open_mode( flags, args ) };
        va_end( args );

        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::FileOpen, error ) )
        {
            errno = error;
            return -1;
        }

        static const OpenFunc next{ _next<OpenFunc>( "open64" ) };
        return ( next != nullptr ) ? next( path, flags, mode ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int connect( int fd, const struct sockaddr * addr, socklen_t len )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketConnect, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, const struct sockaddr *, socklen_t );
        static const Func next{ _next<Func>( "connect" ) };
        return ( next != nullptr ) ? next( fd, addr, len ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API ssize_t send( int fd, const void * buf, size_t len, int flags )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketSend, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = ssize_t (*)( int, const void *, size_t, int );
        static const Func next{ _next<Func>( "send" ) };
        return ( next != nullptr ) ? next( fd, buf, len, flags ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API ssize_t sendmsg( int fd, const struct msghdr * msg, int flags )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketSend, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = ssize_t (*)( int, const struct msghdr *, int );
        static const Func next{ _next<Func>( "sendmsg" ) };
        return ( next != nullptr ) ? next( fd, msg, flags ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API ssize_t recv( int fd, void * buf, size_t len, int flags )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketRecv, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = ssize_t (*)( int, void *, size_t, int );
        static const Func next{ _next<Func>( "recv" ) };
        return ( next != nullptr ) ? next( fd, buf, len, flags ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int bind( int fd, const struct sockaddr * addr, socklen_t len )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketBind, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, const struct sockaddr *, socklen_t );
        static const Func next{ _next<Func>( "bind" ) };
        return ( next != nullptr ) ? next( fd, addr, len ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int listen( int fd, int backlog )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketListen, error ) )
        {
            errno = error;
            return -1;
        }

        const uint32_t delay{ gSlots[static_cast<uint32_t>(areg_fault::FaultPoint::SocketListen)].delay.load( std::memory_order_relaxed ) };
        if ( delay != 0u )
        {
            ::usleep( delay * 1000u );
        }

        using Func = int (*)( int, int );
        static const Func next{ _next<Func>( "listen" ) };
        return ( next != nullptr ) ? next( fd, backlog ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int epoll_ctl( int epfd, int op, int fd, struct epoll_event * ev )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::EpollCtl, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, int, int, struct epoll_event * );
        static const Func next{ _next<Func>( "epoll_ctl" ) };
        return ( next != nullptr ) ? next( epfd, op, fd, ev ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API ssize_t read( int fd, void * buf, size_t len )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::FileRead, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = ssize_t (*)( int, void *, size_t );
        static const Func next{ _next<Func>( "read" ) };
        return ( next != nullptr ) ? next( fd, buf, len ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API ssize_t write( int fd, const void * buf, size_t len )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::FileWrite, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = ssize_t (*)( int, const void *, size_t );
        static const Func next{ _next<Func>( "write" ) };
        return ( next != nullptr ) ? next( fd, buf, len ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int pipe( int fds[2] ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::PipeCreate, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int * );
        static const Func next{ _next<Func>( "pipe" ) };
        return ( next != nullptr ) ? next( fds ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int poll( struct pollfd * fds, nfds_t count, int timeout )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::Poll, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( struct pollfd *, nfds_t, int );
        static const Func next{ _next<Func>( "poll" ) };
        return ( next != nullptr ) ? next( fds, count, timeout ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int tcgetattr( int fd, struct termios * modes ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::TermGet, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, struct termios * );
        static const Func next{ _next<Func>( "tcgetattr" ) };
        return ( next != nullptr ) ? next( fd, modes ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int tcsetattr( int fd, int actions, const struct termios * modes ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::TermSet, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, int, const struct termios * );
        static const Func next{ _next<Func>( "tcsetattr" ) };
        return ( next != nullptr ) ? next( fd, actions, modes ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API DIR * opendir( const char * name )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::DirOpen, error ) )
        {
            errno = error;
            return nullptr;
        }

        using Func = DIR * (*)( const char * );
        static const Func next{ _next<Func>( "opendir" ) };
        return ( next != nullptr ) ? next( name ) : ( errno = ENOSYS, nullptr );
    }

    AREG_FAULT_API int posix_spawnp( pid_t * pid, const char * file, const posix_spawn_file_actions_t * actions
                                   , const posix_spawnattr_t * attrs, char * const argv[], char * const envp[] )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::ProcessSpawn, error ) )
            return error;

        using Func = int (*)( pid_t *, const char *, const posix_spawn_file_actions_t *, const posix_spawnattr_t *, char * const [], char * const [] );
        static const Func next{ _next<Func>( "posix_spawnp" ) };
        return ( next != nullptr ) ? next( pid, file, actions, attrs, argv, envp ) : ENOSYS;
    }
}

namespace
{
    using FcntlFunc = int (*)( int, int, ... );

    //!< Fails the call of fcntl() or fcntl64() if the point is armed, otherwise forwards it.
    int _fcntl( FcntlFunc next, int fd, int cmd, void * arg ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::Fcntl, error ) )
        {
            errno = error;
            return -1;
        }

        return ( next != nullptr ) ? next( fd, cmd, arg ) : ( errno = ENOSYS, -1 );
    }

    //!< Returns the error of a failing pthread call of the point, or 0 if the call passes.
    int _pthread_error( areg_fault::FaultPoint point ) noexcept
    {
        int error{ 0 };
        return _should_fail( point, error ) ? error : 0;
    }
}

extern "C"
{
    AREG_FAULT_API int fcntl( int fd, int cmd, ... )
    {
        va_list args;
        va_start( args, cmd );
        void * const arg{ va_arg( args, void * ) };
        va_end( args );
        static const FcntlFunc next{ _next<FcntlFunc>( "fcntl" ) };
        return _fcntl( next, fd, cmd, arg );
    }

    AREG_FAULT_API int fcntl64( int fd, int cmd, ... )
    {
        va_list args;
        va_start( args, cmd );
        void * const arg{ va_arg( args, void * ) };
        va_end( args );
        static const FcntlFunc next{ _next<FcntlFunc>( "fcntl64" ) };
        return _fcntl( next, fd, cmd, arg );
    }

    AREG_FAULT_API int getsockopt( int fd, int level, int name, void * value, socklen_t * length ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketGetOption, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, int, int, void *, socklen_t * );
        static const Func next{ _next<Func>( "getsockopt" ) };
        return ( next != nullptr ) ? next( fd, level, name, value, length ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int setsockopt( int fd, int level, int name, const void * value, socklen_t length ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketSetOption, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, int, int, const void *, socklen_t );
        static const Func next{ _next<Func>( "setsockopt" ) };
        return ( next != nullptr ) ? next( fd, level, name, value, length ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int accept4( int fd, struct sockaddr * addr, socklen_t * length, int flags )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketAccept, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, struct sockaddr *, socklen_t *, int );
        static const Func next{ _next<Func>( "accept4" ) };
        return ( next != nullptr ) ? next( fd, addr, length, flags ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int gethostname( char * name, size_t length ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::HostName, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( char *, size_t );
        static const Func next{ _next<Func>( "gethostname" ) };
        return ( next != nullptr ) ? next( name, length ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int epoll_create1( int flags ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::EpollCreate, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int );
        static const Func next{ _next<Func>( "epoll_create1" ) };
        return ( next != nullptr ) ? next( flags ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int epoll_wait( int epfd, struct epoll_event * events, int count, int timeout )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::EpollWait, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( int, struct epoll_event *, int, int );
        static const Func next{ _next<Func>( "epoll_wait" ) };
        return ( next != nullptr ) ? next( epfd, events, count, timeout ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int eventfd( unsigned int count, int flags ) noexcept
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::EventCreate, error ) )
        {
            errno = error;
            return -1;
        }

        using Func = int (*)( unsigned int, int );
        static const Func next{ _next<Func>( "eventfd" ) };
        return ( next != nullptr ) ? next( count, flags ) : ( errno = ENOSYS, -1 );
    }

    AREG_FAULT_API int pthread_mutex_init( pthread_mutex_t * mutex, const pthread_mutexattr_t * attr ) noexcept
    {
        const int error{ _pthread_error( areg_fault::FaultPoint::MutexInit ) };
        if ( error != 0 )
            return error;

        using Func = int (*)( pthread_mutex_t *, const pthread_mutexattr_t * );
        static const Func next{ _next<Func>( "pthread_mutex_init" ) };
        return ( next != nullptr ) ? next( mutex, attr ) : ENOSYS;
    }

    AREG_FAULT_API int pthread_mutexattr_init( pthread_mutexattr_t * attr ) noexcept
    {
        const int error{ _pthread_error( areg_fault::FaultPoint::MutexAttrInit ) };
        if ( error != 0 )
            return error;

        using Func = int (*)( pthread_mutexattr_t * );
        static const Func next{ _next<Func>( "pthread_mutexattr_init" ) };
        return ( next != nullptr ) ? next( attr ) : ENOSYS;
    }

    AREG_FAULT_API int pthread_mutexattr_settype( pthread_mutexattr_t * attr, int kind ) noexcept
    {
        const int error{ _pthread_error( areg_fault::FaultPoint::MutexAttrSetType ) };
        if ( error != 0 )
            return error;

        using Func = int (*)( pthread_mutexattr_t *, int );
        static const Func next{ _next<Func>( "pthread_mutexattr_settype" ) };
        return ( next != nullptr ) ? next( attr, kind ) : ENOSYS;
    }

    AREG_FAULT_API int pthread_cond_init( pthread_cond_t * cond, const pthread_condattr_t * attr ) noexcept
    {
        const int error{ _pthread_error( areg_fault::FaultPoint::CondInit ) };
        if ( error != 0 )
            return error;

        using Func = int (*)( pthread_cond_t *, const pthread_condattr_t * );
        static const Func next{ _next<Func>( "pthread_cond_init" ) };
        return ( next != nullptr ) ? next( cond, attr ) : ENOSYS;
    }

    AREG_FAULT_API int pthread_condattr_init( pthread_condattr_t * attr ) noexcept
    {
        const int error{ _pthread_error( areg_fault::FaultPoint::CondAttrInit ) };
        if ( error != 0 )
            return error;

        using Func = int (*)( pthread_condattr_t * );
        static const Func next{ _next<Func>( "pthread_condattr_init" ) };
        return ( next != nullptr ) ? next( attr ) : ENOSYS;
    }

    AREG_FAULT_API int pthread_condattr_setclock( pthread_condattr_t * attr, clockid_t clock ) noexcept
    {
        const int error{ _pthread_error( areg_fault::FaultPoint::CondAttrSetClock ) };
        if ( error != 0 )
            return error;

        using Func = int (*)( pthread_condattr_t *, clockid_t );
        static const Func next{ _next<Func>( "pthread_condattr_setclock" ) };
        return ( next != nullptr ) ? next( attr, clock ) : ENOSYS;
    }

    AREG_FAULT_API int pthread_attr_init( pthread_attr_t * attr ) noexcept
    {
        const int error{ _pthread_error( areg_fault::FaultPoint::ThreadAttrInit ) };
        if ( error != 0 )
            return error;

        using Func = int (*)( pthread_attr_t * );
        static const Func next{ _next<Func>( "pthread_attr_init" ) };
        return ( next != nullptr ) ? next( attr ) : ENOSYS;
    }
}

namespace
{
    //!< Returns the next definition of the named allocation function, or null.
    using AllocFunc = void * (*)( std::size_t, const std::nothrow_t & );

    AllocFunc _next_alloc( bool isArray ) noexcept
    {
        static const char * const names32[]{ "_ZnwjRKSt9nothrow_t", "_ZnajRKSt9nothrow_t" };
        static const char * const names64[]{ "_ZnwmRKSt9nothrow_t", "_ZnamRKSt9nothrow_t" };
        const char * const name{ (sizeof( std::size_t ) == 4u ? names32 : names64)[isArray ? 1 : 0] };
        return reinterpret_cast<AllocFunc>(::dlsym( RTLD_NEXT, name ));
    }
}

void * operator new( std::size_t size, const std::nothrow_t & tag ) noexcept
{
    int error{ 0 };
    if ( _should_fail( areg_fault::FaultPoint::Allocation, error ) )
        return nullptr;

    static const AllocFunc next{ _next_alloc( false ) };
    return ( next != nullptr ) ? next( size, tag ) : std::malloc( size != 0u ? size : 1u );
}

void * operator new[]( std::size_t size, const std::nothrow_t & tag ) noexcept
{
    int error{ 0 };
    if ( _should_fail( areg_fault::FaultPoint::Allocation, error ) )
        return nullptr;

    static const AllocFunc next{ _next_alloc( true ) };
    return ( next != nullptr ) ? next( size, tag ) : std::malloc( size != 0u ? size : 1u );
}

namespace
{
    using AlignedAllocFunc = void * (*)( std::size_t, std::align_val_t, const std::nothrow_t & );

    AlignedAllocFunc _next_aligned_alloc( bool isArray ) noexcept
    {
        static const char * const names32[]{ "_ZnwjSt11align_val_tRKSt9nothrow_t", "_ZnajSt11align_val_tRKSt9nothrow_t" };
        static const char * const names64[]{ "_ZnwmSt11align_val_tRKSt9nothrow_t", "_ZnamSt11align_val_tRKSt9nothrow_t" };
        const char * const name{ (sizeof( std::size_t ) == 4u ? names32 : names64)[isArray ? 1 : 0] };
        return reinterpret_cast<AlignedAllocFunc>(::dlsym( RTLD_NEXT, name ));
    }
}

void * operator new( std::size_t size, std::align_val_t align, const std::nothrow_t & tag ) noexcept
{
    int error{ 0 };
    if ( _should_fail( areg_fault::FaultPoint::AlignedAllocation, error ) )
        return nullptr;

    static const AlignedAllocFunc next{ _next_aligned_alloc( false ) };
    return ( next != nullptr ) ? next( size, align, tag ) : nullptr;
}

void * operator new[]( std::size_t size, std::align_val_t align, const std::nothrow_t & tag ) noexcept
{
    int error{ 0 };
    if ( _should_fail( areg_fault::FaultPoint::AlignedAllocation, error ) )
        return nullptr;

    static const AlignedAllocFunc next{ _next_aligned_alloc( true ) };
    return ( next != nullptr ) ? next( size, align, tag ) : nullptr;
}

namespace
{
    //!< The thread whose throwing allocations are counted; 0 counts none.
    std::atomic<uint64_t>   gCountedThread  { 0u };
    //!< The throwing allocations of the counted thread.
    std::atomic<uint32_t>   gThrowingNew    { 0u };

    using ThrowingAllocFunc = void * (*)( std::size_t );

    ThrowingAllocFunc _next_throwing_alloc( bool isArray ) noexcept
    {
        static const char * const names32[]{ "_Znwj", "_Znaj" };
        static const char * const names64[]{ "_Znwm", "_Znam" };
        const char * const name{ (sizeof( std::size_t ) == 4u ? names32 : names64)[isArray ? 1 : 0] };
        return reinterpret_cast<ThrowingAllocFunc>(::dlsym( RTLD_NEXT, name ));
    }

    inline void _count_throwing_new( ) noexcept
    {
        const uint64_t counted{ gCountedThread.load( std::memory_order_relaxed ) };
        if ( (counted != 0u) && (counted == static_cast<uint64_t>(::pthread_self( ))) )
        {
            gThrowingNew.fetch_add( 1u, std::memory_order_relaxed );
        }
    }
}

extern "C"
{
    AREG_FAULT_API void areg_fault_count_throwing_new( uint64_t threadId )
    {
        gThrowingNew.store( 0u, std::memory_order_relaxed );
        gCountedThread.store( threadId, std::memory_order_relaxed );
    }

    AREG_FAULT_API uint32_t areg_fault_throwing_new_count( )
    {
        return gThrowingNew.load( std::memory_order_relaxed );
    }
}

void * operator new( std::size_t size )
{
    _count_throwing_new( );
    static const ThrowingAllocFunc next{ _next_throwing_alloc( false ) };
    return next( size );
}

void * operator new[]( std::size_t size )
{
    _count_throwing_new( );
    static const ThrowingAllocFunc next{ _next_throwing_alloc( true ) };
    return next( size );
}

//////////////////////////////////////////////////////////////////////////
// macOS: replacements registered in the __DATA,__interpose section.
//////////////////////////////////////////////////////////////////////////
#elif defined(__APPLE__)

namespace
{
    bool _is_supported( areg_fault::FaultPoint point ) noexcept
    {
        return ( point == areg_fault::FaultPoint::ThreadCreate )
            || ( point == areg_fault::FaultPoint::SocketCreate )
            || ( point == areg_fault::FaultPoint::FileOpen )
            || ( point == areg_fault::FaultPoint::DirOpen )
            || ( point == areg_fault::FaultPoint::ProcessSpawn );
    }

    void _install( ) noexcept
    {
    }

    int _fault_pthread_create( pthread_t * thread, const pthread_attr_t * attr, void * (*routine)(void *), void * arg )
    {
        int error{ 0 };
        return _should_fail( areg_fault::FaultPoint::ThreadCreate, error ) ? error : ::pthread_create( thread, attr, routine, arg );
    }

    int _fault_socket( int domain, int type, int protocol )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketCreate, error ) )
        {
            errno = error;
            return -1;
        }

        return ::socket( domain, type, protocol );
    }

    int _fault_open( const char * path, int flags, ... )
    {
        int mode{ 0 };
        if ( (flags & O_CREAT) != 0 )
        {
            va_list args;
            va_start( args, flags );
            mode = va_arg( args, int );
            va_end( args );
        }

        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::FileOpen, error ) )
        {
            errno = error;
            return -1;
        }

        return ::open( path, flags, mode );
    }

    DIR * _fault_opendir( const char * name )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::DirOpen, error ) )
        {
            errno = error;
            return nullptr;
        }

        return ::opendir( name );
    }

    int _fault_posix_spawnp( pid_t * pid, const char * file, const posix_spawn_file_actions_t * actions
                           , const posix_spawnattr_t * attrs, char * const argv[], char * const envp[] )
    {
        int error{ 0 };
        return _should_fail( areg_fault::FaultPoint::ProcessSpawn, error ) ? error : ::posix_spawnp( pid, file, actions, attrs, argv, envp );
    }

    //!< One entry of the dyld interposing table: the replacement and the replaced function.
    struct Interpose
    {
        const void * replacement;
        const void * replaced;
    };
}

#define AREG_FAULT_INTERPOSE( replacement, replaced )                                                   \
    __attribute__((used, section("__DATA,__interpose"))) static const Interpose _interpose_##replaced   \
        { reinterpret_cast<const void *>(&replacement), reinterpret_cast<const void *>(&replaced) }

AREG_FAULT_INTERPOSE( _fault_pthread_create , pthread_create );
AREG_FAULT_INTERPOSE( _fault_socket         , socket );
AREG_FAULT_INTERPOSE( _fault_open           , open );
AREG_FAULT_INTERPOSE( _fault_opendir        , opendir );
AREG_FAULT_INTERPOSE( _fault_posix_spawnp   , posix_spawnp );

//////////////////////////////////////////////////////////////////////////
// Windows: replacements written into the import address tables.
//////////////////////////////////////////////////////////////////////////
#elif defined(_WIN32)

#include <mutex>

namespace
{
    using CreateThreadFunc  = HANDLE (WINAPI *)( LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD );
    using SocketFunc        = SOCKET (WSAAPI *)( int, int, int );
    using CreateFileAFunc   = HANDLE (WINAPI *)( LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE );
    using CreateFileWFunc   = HANDLE (WINAPI *)( LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE );

    CreateThreadFunc    gCreateThread   { nullptr };
    SocketFunc          gSocket         { nullptr };
    CreateFileAFunc     gCreateFileA    { nullptr };
    CreateFileWFunc     gCreateFileW    { nullptr };

    HANDLE WINAPI _fault_CreateThread( LPSECURITY_ATTRIBUTES security, SIZE_T stack, LPTHREAD_START_ROUTINE routine, LPVOID arg, DWORD flags, LPDWORD id )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::ThreadCreate, error ) )
        {
            ::SetLastError( static_cast<DWORD>(error) );
            return nullptr;
        }

        return gCreateThread( security, stack, routine, arg, flags, id );
    }

    SOCKET WSAAPI _fault_socket( int domain, int type, int protocol )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::SocketCreate, error ) )
        {
            ::WSASetLastError( error );
            return INVALID_SOCKET;
        }

        return gSocket( domain, type, protocol );
    }

    HANDLE WINAPI _fault_CreateFileA( LPCSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD attributes, HANDLE pattern )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::FileOpen, error ) )
        {
            ::SetLastError( static_cast<DWORD>(error) );
            return INVALID_HANDLE_VALUE;
        }

        return gCreateFileA( name, access, share, security, creation, attributes, pattern );
    }

    HANDLE WINAPI _fault_CreateFileW( LPCWSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD attributes, HANDLE pattern )
    {
        int error{ 0 };
        if ( _should_fail( areg_fault::FaultPoint::FileOpen, error ) )
        {
            ::SetLastError( static_cast<DWORD>(error) );
            return INVALID_HANDLE_VALUE;
        }

        return gCreateFileW( name, access, share, security, creation, attributes, pattern );
    }

    //!< A replaced function: its module, its name, the replacement and the saved original.
    struct Hook
    {
        const char *    module;
        const char *    name;
        void *          replacement;
        void **         original;
    };

    Hook gHooks[]
    {
          { "kernel32.dll", "CreateThread", reinterpret_cast<void *>(&_fault_CreateThread), reinterpret_cast<void **>(&gCreateThread) }
        , { "ws2_32.dll"  , "socket"      , reinterpret_cast<void *>(&_fault_socket)      , reinterpret_cast<void **>(&gSocket) }
        , { "kernel32.dll", "CreateFileA" , reinterpret_cast<void *>(&_fault_CreateFileA) , reinterpret_cast<void **>(&gCreateFileA) }
        , { "kernel32.dll", "CreateFileW" , reinterpret_cast<void *>(&_fault_CreateFileW) , reinterpret_cast<void **>(&gCreateFileW) }
    };

    //!< Writes the replacements into the import address table of the module.
    void _patch_module( HMODULE module ) noexcept
    {
        uint8_t * const base{ reinterpret_cast<uint8_t *>(module) };
        const IMAGE_DOS_HEADER * const dos{ reinterpret_cast<const IMAGE_DOS_HEADER *>(base) };
        if ( dos->e_magic != IMAGE_DOS_SIGNATURE )
            return;

        const IMAGE_NT_HEADERS * const nt{ reinterpret_cast<const IMAGE_NT_HEADERS *>(base + dos->e_lfanew) };
        const IMAGE_DATA_DIRECTORY & imports{ nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };
        if ( imports.VirtualAddress == 0u )
            return;

        for ( const IMAGE_IMPORT_DESCRIPTOR * entry = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR *>(base + imports.VirtualAddress); entry->Name != 0u; ++ entry )
        {
            for ( IMAGE_THUNK_DATA * thunk = reinterpret_cast<IMAGE_THUNK_DATA *>(base + entry->FirstThunk); thunk->u1.Function != 0u; ++ thunk )
            {
                for ( const Hook & hook : gHooks )
                {
                    if ( (*hook.original == nullptr) || (thunk->u1.Function != reinterpret_cast<ULONG_PTR>(*hook.original)) )
                        continue;

                    DWORD protection{ 0u };
                    if ( ::VirtualProtect( &thunk->u1.Function, sizeof( thunk->u1.Function ), PAGE_READWRITE, &protection ) )
                    {
                        thunk->u1.Function = reinterpret_cast<ULONG_PTR>(hook.replacement);
                        ::VirtualProtect( &thunk->u1.Function, sizeof( thunk->u1.Function ), protection, &protection );
                    }
                }
            }
        }
    }

    //!< Returns true if the path starts with the prefix, ignoring the case.
    bool _starts_with( const char * path, const char * prefix, size_t length ) noexcept
    {
        return ( length != 0u ) && ( ::_strnicmp( path, prefix, length ) == 0 );
    }

    bool _is_supported( areg_fault::FaultPoint point ) noexcept
    {
        return ( point == areg_fault::FaultPoint::ThreadCreate )
            || ( point == areg_fault::FaultPoint::SocketCreate )
            || ( point == areg_fault::FaultPoint::FileOpen );
    }

    void _install( ) noexcept
    {
        static std::mutex lock;
        static bool installed{ false };
        std::lock_guard<std::mutex> guard( lock );
        if ( installed )
            return;

        installed = true;
        for ( Hook & hook : gHooks )
        {
            const HMODULE module{ ::GetModuleHandleA( hook.module ) };
            *hook.original = ( module != nullptr ) ? reinterpret_cast<void *>(::GetProcAddress( module, hook.name )) : nullptr;
        }

        char windows[MAX_PATH]{ };
        const size_t windowsLength{ ::GetWindowsDirectoryA( windows, MAX_PATH ) };

        HMODULE self{ nullptr };
        ::GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT
                            , reinterpret_cast<LPCSTR>(&areg_fault_arm), &self );

        HMODULE modules[1024]{ };
        DWORD needed{ 0u };
        if ( ::K32EnumProcessModules( ::GetCurrentProcess( ), modules, sizeof( modules ), &needed ) == FALSE )
            return;

        const DWORD count{ needed / static_cast<DWORD>(sizeof( HMODULE )) };
        for ( DWORD i = 0u; ( i < count ) && ( i < 1024u ); ++ i )
        {
            char path[MAX_PATH]{ };
            if ( (modules[i] == self) || (::GetModuleFileNameA( modules[i], path, MAX_PATH ) == 0u) )
                continue;

            if ( _starts_with( path, windows, windowsLength ) == false )
            {
                _patch_module( modules[i] );
            }
        }
    }
}

#else   // other platforms

namespace
{
    bool _is_supported( areg_fault::FaultPoint /*point*/ ) noexcept
    {
        return false;
    }

    void _install( ) noexcept
    {
    }
}

#endif  // platforms
