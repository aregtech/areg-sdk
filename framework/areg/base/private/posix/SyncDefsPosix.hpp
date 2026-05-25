#ifndef AREG_BASE_PRIVATE_POSIX_SYNCDEFSPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_SYNCDEFSPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncDefsPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/CommonDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include <unistd.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////
// Posix specific data
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This namespace contains main constants and helper methods
 *          shared between other synchronization objects.
 **/
namespace areg::os {
    /**
     * \brief   areg::os::POSIX_SUCCESS
     *          Indicates the success of POSIX function call.
     **/
    constexpr int   POSIX_SUCCESS       { 0 };

    /**
     * \brief   areg::os::SyncSignal
     *          The valid indexes when synchronization event is fired in the waiting list
     *          or error happened.
     **/
    enum class SyncSignal   : int32_t
    {
          Invalid     =  -1 // Invalid synchronization object.
        , First       =  0  // First synchronization object in the waiting list is fired.
        , All         =  areg::MAXIMUM_WAITING_OBJECTS // All synchronization objects in the waiting list are fired.
        , Error       =  99 // Indicates start of error range
        , FirstError  = 100 // First error index, so that it is possible to detect which event in the waiting list caused the error.
        , AsyncSignal = 200 // Indicates that the waiting thread is signaled by some asynchronous event.
        , Timeout     = 201 // Indicates that waiting timeout is expired.
        , Interrupted = 202 // The waiting was interrupted by some other event, like timer expired.
    };

    /**
     * \brief   areg::os::ResetMode
     *          The types of reset for event objects.
     **/
    enum  class  ResetMode
    {
          Manual    //!< The event is reset manually.
        , Automatic //!< The event is reset automatically.
    };
    /**
     * \brief   Returns the string representation of a ResetMode value.
     **/
    [[nodiscard]]
    inline constexpr const char * as_string(areg::os::ResetMode val) noexcept;

    /**
     * \brief   areg::os::WaitCondition
     *          Event matching condition. Either there should be exact match,
     *          i.e. all events are fired, of any event in the list should be fired.
     **/
    enum class WaitCondition
    {
          Exact   //!< The should be exact matching condition, i.e. all event in the list should be fired.
        , Any     //!< Any event in the list should be fired to unlock the thread.
    };
    /**
     * \brief   Returns the string representation of a WaitCondition value.
     **/
    [[nodiscard]]
    inline constexpr const char * as_string(areg::os::WaitCondition val) noexcept;

    /**
     * \brief   areg::os::SyncKind
     *          Type of synchronization objects.
     **/
    enum class SyncKind : uint32_t
    {
          SoUndefined       = (0 << 0) | 0  //!< No type, it is undefined and invalid.
        , SoWaitable        = (1 << 0) | 1  //!< Waitiable object
        , SoMutex           = (1 << 1) | 0  //!< Simple POSIX mutex.
        , SoSpinLock        = (1 << 2) | 0  //!< Simple POSIX spin lock
        , SoWaitMutex       = (1 << 1) | 1  //!< Waitable mutex, so that it can be used in the waiting list.
        , SoWaitEvent       = (1 << 2) | 1  //!< Waitable event, so that it can be used in the waiting list.
        , SoWaitSemaphore   = (1 << 3) | 1  //!< Waitable semaphore, so that it can be used in the waiting list.
        , SoWaitTimer       = (1 << 4) | 1  //!< Waitable timer, so that it can be used in the waiting list
    };
    /**
     * \brief   Returns the string representation of a SyncKind value.
     **/
    [[nodiscard]]
    inline constexpr const char * as_string(areg::os::SyncKind val) noexcept;

    /**
     * \brief   Calculates the absolute timeout starting from the current time.
     *
     * \param[out] out_result   The timespec structure that receives the timeout value in
     *                          nanosecond range.
     * \param   msTimeout       The relative timeout in milliseconds to be converted.
     * \return  Returns true if the calculation succeeded; false if current time could not be
     *          obtained.
     **/
    inline bool timeout_from_now( timespec & out_result, uint32_t msTimeout ) noexcept;

    /**
     * \brief   Converts the timeout value to a POSIX timespec structure.
     *
     * \param[out] out_result   The timespec structure that receives the converted timeout.
     * \param   msTimeout       The timeout value in milliseconds to convert.
     **/
    inline constexpr void conv_timeout( timespec & out_result, uint32_t msTimeout ) noexcept;

#if defined(__APPLE__)
    //! __ulock operation code for compare-and-wait on macOS (equal to UL_COMPARE_AND_WAIT).
    constexpr uint32_t APPLE_ULOCK_COMPARE_AND_WAIT { 1u };
#endif  // defined(__APPLE__)

} // namespace areg::os

//////////////////////////////////////////////////////////////////////////
// areg::os namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool areg::os::timeout_from_now( timespec & out_result, uint32_t msTimeout ) noexcept
{
    bool result = false;
    if ( areg::os::POSIX_SUCCESS == ::clock_gettime(CLOCK_REALTIME, &out_result ) )
    {
        conv_timeout(out_result, msTimeout);
        result = true;
    }

    return result;
}

inline constexpr void areg::os::conv_timeout( timespec & out_result, uint32_t msTimeout ) noexcept
{
    int64_t ns          = static_cast<int64_t>(out_result.tv_nsec) + static_cast<int64_t>(msTimeout) * areg::MILLISEC_TO_NS;
    out_result.tv_sec  += static_cast<time_t>(ns / areg::SEC_TO_NS);
    out_result.tv_nsec  = static_cast<long>(ns % areg::SEC_TO_NS);
}

inline constexpr const char * areg::os::as_string(areg::os::ResetMode val) noexcept
{
    switch (val)
    {
    case areg::os::ResetMode::Manual:
        return "areg::os::ResetMode::Manual";
    case areg::os::ResetMode::Automatic:
        return "areg::os::ResetMode::Automatic";
    default:
        return "ERR: Unexpected areg::os::ResetMode value!";
    }
}

inline constexpr const char * areg::os::as_string(areg::os::WaitCondition val) noexcept
{
    switch (val)
    {
    case areg::os::WaitCondition::Exact:
        return "areg::os::WaitCondition::Exact";
    case areg::os::WaitCondition::Any:
        return "areg::os::WaitCondition::Any";
    default:
        return "ERR: Unexpected areg::os::WaitCondition value!";
    }
}

inline constexpr const char * areg::os::as_string(areg::os::SyncKind val) noexcept
{
    switch (val)
    {
    case areg::os::SyncKind::SoUndefined:
        return "areg::os::SyncKind::SoUndefined";
    case areg::os::SyncKind::SoWaitable:
        return "areg::os::SyncKind::SoWaitable";
    case areg::os::SyncKind::SoMutex:
        return "areg::os::SyncKind::SoMutex";
    case areg::os::SyncKind::SoSpinLock:
        return "areg::os::SyncKind::SoSpinLock";
    case areg::os::SyncKind::SoWaitMutex:
        return "areg::os::SyncKind::SoWaitMutex";
    case areg::os::SyncKind::SoWaitEvent:
        return "areg::os::SyncKind::SoWaitEvent";
    case areg::os::SyncKind::SoWaitSemaphore:
        return "areg::os::SyncKind::SoWaitSemaphore";
    case areg::os::SyncKind::SoWaitTimer:
        return "areg::os::SyncKind::SoWaitTimer";
    default:
        return "ERR: Unexpected areg::os::SyncKind value!";
    }
}

#endif // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SYNCDEFSPOSIX_HPP
