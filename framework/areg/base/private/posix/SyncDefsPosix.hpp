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
namespace areg::os {

    /**
     * \brief   areg::os::POSIX_SUCCESS
     *          Indicates the success of POSIX function call.
     **/
    constexpr int   POSIX_SUCCESS       = 0;

    /**
     * \brief   areg::os::eSyncObjectFired
     *          The valid indexes when synchronization event is fired in the waiting list
     *          or error happened.
     **/
    typedef enum  E_SyncObjectFired : int
    {
        /**
         * \brief   Invalid synchronization object.
         **/
          SyncObjectInvalid     =  -1
        /**
         * \brief   The index of first synchronization object.
         **/
        , SyncObject0           =   0
        /**
         * \brief   Indicates that all synchronization events where fired.
         **/
        , SyncObjectAll         =  areg::MAXIMUM_WAITING_OBJECTS
        /**
         * \brief   Indicates start of error range
         **/
        , SyncObjectError       =  99
        /**
         * \brief   There was an error while trying to lock first synchronization object.
         **/
        , SyncObject0Error      = 100
        /**
         * \brief   Asynchronously signaled.
         **/
        , SyncAsyncSignal       = 200
        /**
         * \brief   Lock waiting timeout has been expired.
         **/
        , SyncObjectTimeout     = 201
        /**
         * \brief   The waiting was interrupted by some other event, like timer expired.
         **/
        , SyncWaitInterrupted   = 202
    } eSyncObjectFired;

    /**
     * \brief   areg::os::eEventResetInfo
     *          The types of reset for event objects.
     **/
    typedef enum  class  E_EventResetInfo
    {
          EventResetManual      //!< The event is reset manually.
        , EventResetAutomatic   //!< The event is reset automatically.
    } eEventResetInfo;
    /**
     * \brief   Returns string value of areg::os::eEventResetInfo
     **/
    inline const char * getString(areg::os::eEventResetInfo val);

    /**
     * \brief   areg::os::eMatchCondition
     *          Event matching condition. Either there should be exact match,
     *          i.e. all events are fired, of any event in the list should be fired.
     **/
    typedef enum class E_MatchCondition
    {
          MatchConditionExact   //!< The should be exact matchin condition, i.e. all event in the list should be fired.
        , MatchConditionAny     //!< Any event in the list should be fired to unlock the thread.
    } eMatchCondition;
    /**
     * \brief   Returns string value of areg::os::eMatchCondition
     **/
    inline const char * getString(areg::os::eMatchCondition val);

    /**
     * \brief   areg::os::eSyncObject
     *          Type of synchronization objects.
     **/
    typedef enum class E_SyncObject : unsigned int
    {
          SoUndefined       = (0 << 0) | 0  //!< No type, it is undefined and invalid.
        , SoWaitable        = (1 << 0) | 1  //!< Waitiable object
        , SoMutex           = (1 << 1) | 0  //!< Simple POSIX mutex.
        , SoSpinLock        = (1 << 2) | 0  //!< Simple POSIX spin lock
        , SoWaitMutex       = (1 << 1) | 1  //!< Waitable mutex, so that it can be used in the waiting list.
        , SoWaitEvent       = (1 << 2) | 1  //!< Waitable event, so that it can be used in the waiting list.
        , SoWaitSemaphore   = (1 << 3) | 1  //!< Waitable semaphore, so that it can be used in the waiting list.
        , SoWaitTimer       = (1 << 4) | 1  //!< Waitable timer, so that it can be used in the waiting list

    } eSyncObject;
    /**
     * \brief   Returns string value of areg::os::eSyncObject
     **/
    inline const char * getString(areg::os::eSyncObject val);

    /**
     * \brief   Calculates the timeout value starting from now.
     * \param   out_result  The object that contains timeout information in nanosecond range.
     * \param   msTimeout   The timeout to be calculated.
     * \return  Returns true if succeeded to get time and convert.
     **/
    inline bool timeoutFromNow( timespec & out_result, unsigned int msTimeout );

    /**
     * \brief   Converts the given timeout value into POSIX time structure.
     * \param   out_result  The object that contains timeout information in nanosecond range.
     * \param   msTimeout   The timeout to be calculated.
     **/
    inline void convTimeout( timespec & out_result, unsigned int msTimeout );

} // namespace areg::os

//////////////////////////////////////////////////////////////////////////
// NESyncTypesIX namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool areg::os::timeoutFromNow( timespec & out_result, unsigned int msTimeout )
{
    bool result = false;
    if ( areg::os::POSIX_SUCCESS == ::clock_gettime(CLOCK_REALTIME, &out_result ) )
    {
        convTimeout(out_result, msTimeout);
        result = true;
    }

    return result;
}

inline void areg::os::convTimeout( timespec & out_result, unsigned int msTimeout )
{
	constexpr std::chrono::nanoseconds _sec_to_nano{areg::SEC_TO_NS};

	std::chrono::seconds		sec{ out_result.tv_sec };
	std::chrono::nanoseconds  	ns { out_result.tv_nsec };

	ns += std::chrono::nanoseconds(msTimeout * areg::MILLISEC_TO_NS);
	sec+= std::chrono::duration_cast<std::chrono::seconds>(ns);
	ns = ns % _sec_to_nano;

    out_result.tv_sec 	= static_cast<int64_t>(sec.count());
    out_result.tv_nsec  = static_cast<int64_t>(ns.count());
}

inline const char * areg::os::getString(areg::os::eEventResetInfo val)
{
    switch (val)
    {
    case areg::os::eEventResetInfo::EventResetManual:
        return "areg::os::EventResetManual";
    case areg::os::eEventResetInfo::EventResetAutomatic:
        return "areg::os::EventResetAutomatic";
    default:
        return "ERR: Unexpected areg::os::eEventResetInfo value!";
    }
}

inline const char * areg::os::getString(areg::os::eMatchCondition val)
{
    switch (val)
    {
    case areg::os::eMatchCondition::MatchConditionExact:
        return "areg::os::MatchConditionExact";
    case areg::os::eMatchCondition::MatchConditionAny:
        return "areg::os::MatchConditionAny";
    default:
        return "ERR: Unexpected areg::os::eMatchCondition value!";
    }
}

inline const char * areg::os::getString(areg::os::eSyncObject val)
{
    switch (val)
    {
    case areg::os::eSyncObject::SoUndefined:
        return "areg::os::SoUndefined";
    case areg::os::eSyncObject::SoWaitable:
        return "areg::os::SoWaitable";
    case areg::os::eSyncObject::SoMutex:
        return "areg::os::SoMutex";
    case areg::os::eSyncObject::SoSpinLock:
        return "areg::os::SoSpinLock";
    case areg::os::eSyncObject::SoWaitMutex:
        return "areg::os::SoWaitMutex";
    case areg::os::eSyncObject::SoWaitEvent:
        return "areg::os::SoWaitEvent";
    case areg::os::eSyncObject::SoWaitSemaphore:
        return "areg::os::SoWaitSemaphore";
    case areg::os::eSyncObject::SoWaitTimer:
        return "areg::os::SoWaitTimer";
    default:
        return "ERR: Unexpected areg::os::eSyncObject value!";
    }
}

#endif // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SYNCDEFSPOSIX_HPP
