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
