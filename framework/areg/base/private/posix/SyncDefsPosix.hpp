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
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/CommonDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include <unistd.h>
#include <time.h>
#include <chrono>

//////////////////////////////////////////////////////////////////////////
// NESyncTypesIX namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This namespace contains main constants and helper methods
 *          shared between other synchronization objects.
 **/
namespace NESyncTypesIX
{
    /**
     * \brief   NESyncTypesIX::POSIX_SUCCESS
     *          Indicates the success of POSIX function call.
     **/
    constexpr int   POSIX_SUCCESS       = 0;

    /**
     * \brief   NESyncTypesIX::SyncSignal
     *          The valid indexes when synchronization event is fired in the waiting list
     *          or error happened.
     **/
    enum class SyncSignal   : int32_t
    {
          Invalid     =  -1 // Invalid synchronization object.
        , First       =  0  // First synchronization object in the waiting list is fired.
        , All         =  NECommon::MAXIMUM_WAITING_OBJECTS // All synchronization objects in the waiting list are fired.
        , Error       =  99 // Indicates start of error range
        , FirstError  = 100 // First error index, so that it is possible to detect which event in the waiting list caused the error.
        , AsyncSignal = 200 // Indicates that the waiting thread is signaled by some asynchronous event.
        , Timeout     = 201 // Indicates that waiting timeout is expired.
        , Interrupted = 202 // The waiting was interrupted by some other event, like timer expired.
    };

    /**
     * \brief   NESyncTypesIX::ResetMode
     *          The types of reset for event objects.
     **/
    enum  class  ResetMode
    {
          Manual    //!< The event is reset manually.
        , Automatic //!< The event is reset automatically.
    };
    /**
     * \brief   Returns string value of NESyncTypesIX::ResetMode
     **/
    inline const char * getString(NESyncTypesIX::ResetMode val);

    /**
     * \brief   NESyncTypesIX::WaitCondition
     *          Event matching condition. Either there should be exact match,
     *          i.e. all events are fired, of any event in the list should be fired.
     **/
    enum class WaitCondition
    {
          Exact   //!< The should be exact matchin condition, i.e. all event in the list should be fired.
        , Any     //!< Any event in the list should be fired to unlock the thread.
    };
    /**
     * \brief   Returns string value of NESyncTypesIX::WaitCondition
     **/
    inline const char * getString(NESyncTypesIX::WaitCondition val);

    /**
     * \brief   NESyncTypesIX::SyncKind
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
     * \brief   Returns string value of NESyncTypesIX::SyncKind
     **/
    inline const char * getString(NESyncTypesIX::SyncKind val);

    /**
     * \brief   Calculates the timeout value starting from now.
     * \param   out_result  The object that contains timeout information in nanosecond range.
     * \param   msTimeout   The timeout to be calculated.
     * \return  Returns true if succeeded to get time and convert.
     **/
    inline bool timeoutFromNow( timespec & out_result, uint32_t msTimeout );

    /**
     * \brief   Converts the given timeout value into POSIX time structure.
     * \param   out_result  The object that contains timeout information in nanosecond range.
     * \param   msTimeout   The timeout to be calculated.
     **/
    inline void convTimeout( timespec & out_result, uint32_t msTimeout );

} // namespace NESyncTypesIX

//////////////////////////////////////////////////////////////////////////
// NESyncTypesIX namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool NESyncTypesIX::timeoutFromNow( timespec & out_result, uint32_t msTimeout )
{
    bool result = false;
    if ( NESyncTypesIX::POSIX_SUCCESS == ::clock_gettime(CLOCK_REALTIME, &out_result ) )
    {
        convTimeout(out_result, msTimeout);
        result = true;
    }

    return result;
}

inline void NESyncTypesIX::convTimeout( timespec & out_result, uint32_t msTimeout )
{
	constexpr std::chrono::nanoseconds _sec_to_nano{NEUtilities::SEC_TO_NS};

	std::chrono::seconds		sec{ out_result.tv_sec };
	std::chrono::nanoseconds  	ns { out_result.tv_nsec };

	ns += std::chrono::nanoseconds(msTimeout * NEUtilities::MILLISEC_TO_NS);
	sec+= std::chrono::duration_cast<std::chrono::seconds>(ns);
	ns = ns % _sec_to_nano;

    out_result.tv_sec 	= static_cast<int64_t>(sec.count());
    out_result.tv_nsec  = static_cast<int64_t>(ns.count());
}

inline const char * NESyncTypesIX::getString(NESyncTypesIX::ResetMode val)
{
    switch (val)
    {
    case NESyncTypesIX::ResetMode::Manual:
        return "NESyncTypesIX::Manual";
    case NESyncTypesIX::ResetMode::Automatic:
        return "NESyncTypesIX::Automatic";
    default:
        return "ERR: Unexpected NESyncTypesIX::ResetMode value!";
    }
}

inline const char * NESyncTypesIX::getString(NESyncTypesIX::WaitCondition val)
{
    switch (val)
    {
    case NESyncTypesIX::WaitCondition::Exact:
        return "NESyncTypesIX::Exact";
    case NESyncTypesIX::WaitCondition::Any:
        return "NESyncTypesIX::Any";
    default:
        return "ERR: Unexpected NESyncTypesIX::WaitCondition value!";
    }
}

inline const char * NESyncTypesIX::getString(NESyncTypesIX::SyncKind val)
{
    switch (val)
    {
    case NESyncTypesIX::SyncKind::SoUndefined:
        return "NESyncTypesIX::SoUndefined";
    case NESyncTypesIX::SyncKind::SoWaitable:
        return "NESyncTypesIX::SoWaitable";
    case NESyncTypesIX::SyncKind::SoMutex:
        return "NESyncTypesIX::SoMutex";
    case NESyncTypesIX::SyncKind::SoSpinLock:
        return "NESyncTypesIX::SoSpinLock";
    case NESyncTypesIX::SyncKind::SoWaitMutex:
        return "NESyncTypesIX::SoWaitMutex";
    case NESyncTypesIX::SyncKind::SoWaitEvent:
        return "NESyncTypesIX::SoWaitEvent";
    case NESyncTypesIX::SyncKind::SoWaitSemaphore:
        return "NESyncTypesIX::SoWaitSemaphore";
    case NESyncTypesIX::SyncKind::SoWaitTimer:
        return "NESyncTypesIX::SoWaitTimer";
    default:
        return "ERR: Unexpected NESyncTypesIX::SyncKind value!";
    }
}

#endif // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SYNCDEFSPOSIX_HPP
