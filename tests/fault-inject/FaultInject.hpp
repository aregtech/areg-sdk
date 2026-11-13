#ifndef AREG_TESTS_FAULT_INJECT_FAULTINJECT_HPP
#define AREG_TESTS_FAULT_INJECT_FAULTINJECT_HPP

/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/FaultInject.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. Makes chosen system calls fail on request.
 *
 *              The fault library replaces the system calls listed in FaultPoint for every module
 *              of the process, the areg library included. A disarmed point forwards each call
 *              to the system. An armed point lets 'skip' calls through, then fails 'count'
 *              calls with the given error, then forwards again.
 *
 *              Linux:   the library is linked before the C library, so its definitions win.
 *              macOS:   the library interposes with a __DATA,__interpose section; the tests
 *                       also insert it with DYLD_INSERT_LIBRARIES.
 *              Windows: arming patches the import address tables of the loaded modules that
 *                       are not in the system directory.
 *
 *              Arm points only inside a child process (units/ChildProcess.hpp): a failed
 *              system call can leave the process unusable for the next test.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include <cstdint>

#if defined(_WIN32)
    #if defined(AREG_FAULT_EXPORTS)
        #define AREG_FAULT_API  __declspec(dllexport)
    #else   // defined(AREG_FAULT_EXPORTS)
        #define AREG_FAULT_API  __declspec(dllimport)
    #endif  // defined(AREG_FAULT_EXPORTS)
#else   // defined(_WIN32)
    #define AREG_FAULT_API  __attribute__((visibility("default")))
#endif  // defined(_WIN32)

namespace areg_fault
{
    /**
     * \brief   The system calls that can fail on request, and the call each one replaces.
     **/
    enum class FaultPoint : uint32_t
    {
          ThreadCreate  = 0 //!< POSIX pthread_create(), Windows CreateThread(). Error: errno value / Win32 error.
        , SocketCreate      //!< socket(). Error: errno value / WSA error.
        , FileOpen          //!< POSIX open(), open64(), Windows CreateFileA(), CreateFileW(). Error: errno value / Win32 error.
        , DirOpen           //!< POSIX opendir(). Not replaced on Windows.
        , ProcessSpawn      //!< POSIX posix_spawnp(). Not replaced on Windows.
        , SocketConnect     //!< Linux connect(), Windows connect(). Not replaced on macOS. Error: errno value / WSA error.
        , SocketSend        //!< Linux send() and sendmsg(), Windows send() and WSASend(). Not replaced on macOS. Error: errno value / WSA error.
        , SocketRecv        //!< Linux recv(), Windows recv(). Not replaced on macOS. Error: errno value / WSA error.
        , SocketBind        //!< Linux bind(), Windows bind(). Not replaced on macOS. Error: errno value / WSA error.
        , SocketListen      //!< Linux listen(), Windows listen(). Not replaced on macOS. Error: errno value / WSA error.
        , EpollCtl          //!< Linux epoll_ctl(). Not replaced elsewhere. Error: errno value.
        , FileRead          //!< Linux read(), Windows ReadFile(). Not replaced on macOS. Error: errno value / Win32 error.
        , FileWrite         //!< Linux write(), Windows WriteFile(). Not replaced on macOS. Error: errno value / Win32 error.
        , Allocation        //!< Linux operator new(std::nothrow) and operator new[](std::nothrow): returns nullptr. Not replaced elsewhere; the error is ignored.
        , AlignedAllocation //!< Linux operator new(std::align_val_t, std::nothrow) and new[]: returns nullptr. Not replaced elsewhere; the error is ignored.
        , PipeCreate        //!< Linux pipe(). Not replaced elsewhere. Error: errno value.
        , Poll              //!< Linux poll(), Windows WSAPoll() and select(). Not replaced on macOS. Error: errno value / WSA error.
        , TermGet           //!< Linux tcgetattr(). Not replaced elsewhere. Error: errno value.
        , TermSet           //!< Linux tcsetattr(). Not replaced elsewhere. Error: errno value.
        , Fcntl             //!< Linux fcntl() and fcntl64(), Windows ioctlsocket(). Not replaced on macOS. Error: errno value / WSA error.
        , SocketGetOption   //!< Linux getsockopt(), Windows getsockopt(). Not replaced on macOS. Error: errno value / WSA error.
        , SocketSetOption   //!< Linux setsockopt(), Windows setsockopt(). Not replaced on macOS. Error: errno value / WSA error.
        , SocketAccept      //!< Linux accept4(), Windows accept(). Not replaced on macOS. Error: errno value / WSA error.
        , HostName          //!< Linux gethostname(), Windows gethostname(). Not replaced on macOS. Error: errno value / WSA error.
        , EpollCreate       //!< Linux epoll_create1(), Windows CreateIoCompletionPort(). Not replaced on macOS. Error: errno value / Win32 error.
        , EpollWait         //!< Linux epoll_wait(), Windows GetQueuedCompletionStatusEx(). Not replaced on macOS. Error: errno value / Win32 error.
        , EventCreate       //!< Linux eventfd(), Windows CreateEventW(). Not replaced on macOS. Error: errno value / Win32 error.
        , MutexInit         //!< Linux pthread_mutex_init(), Windows CreateMutexW(). Not replaced on macOS. Error: return value / Win32 error.
        , MutexAttrInit     //!< Linux pthread_mutexattr_init(). Not replaced elsewhere. Error: return value.
        , MutexAttrSetType  //!< Linux pthread_mutexattr_settype(). Not replaced elsewhere. Error: return value.
        , CondInit          //!< Linux pthread_cond_init(). Not replaced elsewhere. Error: return value.
        , CondAttrInit      //!< Linux pthread_condattr_init(). Not replaced elsewhere. Error: return value.
        , CondAttrSetClock  //!< Linux pthread_condattr_setclock(). Not replaced elsewhere. Error: return value.
        , ThreadAttrInit    //!< Linux pthread_attr_init(). Not replaced elsewhere. Error: return value.
        , TimerFdCreate     //!< Linux timerfd_create(), Windows CreateThreadpoolTimer(): the system timer of a Timer. Not replaced on macOS. Error: errno value / Win32 error.
        , TimerFdSet        //!< Linux timerfd_settime(). Not replaced elsewhere. Error: errno value.
        , SignalAction      //!< Linux sigaction(). Not replaced elsewhere. Error: errno value.
        , SocketInit        //!< Windows WSAStartup(). Not replaced elsewhere. Error: the returned WSA error.
        , SocketName        //!< Windows getsockname(). Not replaced elsewhere. Error: WSA error.
        , SemaphoreCreate   //!< Windows CreateSemaphoreW(). Not replaced elsewhere. Error: Win32 error.
        , WaitableTimerCreate //!< Windows CreateWaitableTimerExW(). Not replaced elsewhere. Error: Win32 error.
        , FileSeek          //!< Windows SetFilePointer(). Not replaced elsewhere. Error: Win32 error.
        , ServiceManagerOpen  //!< Windows OpenSCManagerW(). Not replaced elsewhere. Error: Win32 error.
        , ServiceOpen       //!< Windows OpenServiceW(). Not replaced elsewhere. Error: Win32 error.
        , ConsoleMode       //!< Windows SetConsoleMode(). Not replaced elsewhere. Error: Win32 error.
        , Count             //!< The number of points.
    };
}

extern "C"
{
    /**
     * \brief   Arms a fault point.
     * \param   point   The FaultPoint value.
     * \param   error   The error the failed calls report.
     * \param   skip    The number of calls that pass before the first failure.
     * \param   count   The number of calls that fail; 0 fails every call until disarmed.
     * \return  True if the point is armed; false if it is not replaced on this platform.
     **/
    AREG_FAULT_API bool areg_fault_arm( uint32_t point, int error, uint32_t skip, uint32_t count );

    /**
     * \brief   Disarms every fault point and resets the counters of failed calls.
     **/
    AREG_FAULT_API void areg_fault_disarm_all( void );

    /**
     * \brief   Returns the number of calls the point failed since it was armed.
     * \param   point   The FaultPoint value.
     **/
    AREG_FAULT_API uint32_t areg_fault_failed( uint32_t point );

    /**
     * \brief   Returns the number of calls of the point since it was armed, passed or failed.
     * \param   point   The FaultPoint value.
     **/
    AREG_FAULT_API uint32_t areg_fault_calls( uint32_t point );

    /**
     * \brief   Makes every passing call of the point wait, until disarmed. Only SocketListen can wait.
     * \param   point           The FaultPoint value.
     * \param   milliseconds    The time a call waits before it runs.
     * \return  True if the wait is set; false if the point cannot wait on this platform.
     **/
    AREG_FAULT_API bool areg_fault_delay( uint32_t point, uint32_t milliseconds );

    /**
     * \brief   Linux only. Counts the calls of the throwing operator new and new[] made by the
     *          thread from now on; 0 stops the counting. The calls still allocate.
     * \param   threadId    The value of pthread_self() of the thread.
     **/
    AREG_FAULT_API void areg_fault_count_throwing_new( uint64_t threadId );

    /**
     * \brief   Linux only. Returns the calls counted since areg_fault_count_throwing_new().
     **/
    AREG_FAULT_API uint32_t areg_fault_throwing_new_count( );
}

namespace areg_fault
{
    /**
     * \brief   Arms the point. See areg_fault_arm().
     **/
    inline bool arm( FaultPoint point, int error, uint32_t skip = 0u, uint32_t count = 1u )
    {
        return ::areg_fault_arm( static_cast<uint32_t>(point), error, skip, count );
    }

    /**
     * \brief   Returns the number of calls the point failed since it was armed.
     **/
    inline uint32_t failed( FaultPoint point )
    {
        return ::areg_fault_failed( static_cast<uint32_t>(point) );
    }

    /**
     * \brief   Returns the number of calls of the point since it was armed, passed or failed.
     **/
    inline uint32_t calls( FaultPoint point )
    {
        return ::areg_fault_calls( static_cast<uint32_t>(point) );
    }

    /**
     * \brief   Makes the calls of the point wait. See areg_fault_delay().
     **/
    inline bool delay( FaultPoint point, uint32_t milliseconds )
    {
        return ::areg_fault_delay( static_cast<uint32_t>(point), milliseconds );
    }

    /**
     * \brief   Disarms every point when the scope ends.
     **/
    class FaultScope
    {
    public:
        FaultScope( ) = default;
        ~FaultScope( )
        {
            ::areg_fault_disarm_all( );
        }

        FaultScope( const FaultScope & ) = delete;
        FaultScope & operator = ( const FaultScope & ) = delete;
    };
}

#endif  // AREG_TESTS_FAULT_INJECT_FAULTINJECT_HPP
