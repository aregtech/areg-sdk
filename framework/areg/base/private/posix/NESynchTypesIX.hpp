#ifndef AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
#define AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/NESynchTypesIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/NECommon.hpp"
#include "areg/base/NEUtilities.hpp"
#include <unistd.h>
#include <time.h>
#include <chrono>

//////////////////////////////////////////////////////////////////////////
// NESynchTypesIX namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This namespace contains main constants and helper methods
 *          shared between other synchronization objects.
 **/
namespace NESynchTypesIX
{
    /**
     * \brief   NESynchTypesIX::POSIX_SUCCESS
     *          Indicates the success of POSIX function call.
     **/
    constexpr int   POSIX_SUCCESS       = 0;

    /**
     * \brief   NESynchTypesIX::eSynchObjectFired
     *          The valid indexes when synchronization event is fired in the waiting list
     *          or error happened.
     **/
    typedef enum  E_SynchObjectFired : int
    {
        /**
         * \brief   Invalid synchronization object.
         **/
          SynchObjectInvalid    =  -1
        /**
         * \brief   The index of first synchronization object.
         **/
        , SynchObject0          =   0
        /**
         * \brief   Indicates that all synchronization events where fired.
         **/
        , SynchObjectAll        =  NECommon::MAXIMUM_WAITING_OBJECTS
        /**
         * \brief   Indicates start of error range
         **/
        , SynchObjectError      =  99
        /**
         * \brief   There was an error while trying to lock first synchronization object.
         **/
        , SynchObject0Error     = 100
        /**
         * \brief   Asynchronously signaled.
         **/
        , SynchAsynchSignal     = 200
        /**
         * \brief   Lock waiting timeout has been expired.
         **/
        , SynchObjectTimeout    = 201
        /**
         * \brief   The waiting was interrupted by some other event, like timer expired.
         **/
        , SynchWaitInterrupted  = 202
    } eSynchObjectFired;

    /**
     * \brief   NESynchTypesIX::eEventResetInfo
     *          The types of reset for event objects.
     **/
    typedef enum  class  E_EventResetInfo
    {
          EventResetManual      //!< The event is reset manually.
        , EventResetAutomatic   //!< The event is reset automatically.
    } eEventResetInfo;
    /**
     * \brief   Returns string value of NESynchTypesIX::eEventResetInfo
     **/
    inline const char * getString(NESynchTypesIX::eEventResetInfo val);

    /**
     * \brief   NESynchTypesIX::eMatchCondition
     *          Event matching condition. Either there should be exact match,
     *          i.e. all events are fired, of any event in the list should be fired.
     **/
    typedef enum class E_MatchCondition
    {
          MatchConditionExact   //!< The should be exact matchin condition, i.e. all event in the list should be fired.
        , MatchConditionAny     //!< Any event in the list should be fired to unlock the thread.
    } eMatchCondition;
    /**
     * \brief   Returns string value of NESynchTypesIX::eMatchCondition
     **/
    inline const char * getString(NESynchTypesIX::eMatchCondition val);

    /**
     * \brief   NESynchTypesIX::eSynchObject
     *          Type of synchronization objects.
     **/
    typedef enum class E_SynchObject : unsigned int
    {
          SoUndefined       = (0 << 0) | 0  //!< No type, it is undefined and invalid.
        , SoWaitable        = (1 << 0) | 1  //!< Waitiable object
        , SoMutex           = (1 << 1) | 0  //!< Simple POSIX mutex.
        , SoSpinLock        = (1 << 2) | 0  //!< Simple POSIX spin lock
        , SoWaitMutex       = (1 << 1) | 1  //!< Waitable mutex, so that it can be used in the waiting list.
        , SoWaitEvent       = (1 << 2) | 1  //!< Waitable event, so that it can be used in the waiting list.
        , SoWaitSemaphore   = (1 << 3) | 1  //!< Waitable semaphore, so that it can be used in the waiting list.
        , SoWaitTimer       = (1 << 4) | 1  //!< Waitable timer, so that it can be used in the waiting list

    } eSynchObject;
    /**
     * \brief   Returns string value of NESynchTypesIX::eSynchObject
     **/
    inline const char * getString(NESynchTypesIX::eSynchObject val);

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

} // namespace NESynchTypesIX

//////////////////////////////////////////////////////////////////////////
// NESynchTypesIX namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool NESynchTypesIX::timeoutFromNow( timespec & out_result, unsigned int msTimeout )
{
    bool result = false;
    if ( NESynchTypesIX::POSIX_SUCCESS == clock_gettime(CLOCK_MONOTONIC, &out_result ) )
    {
        convTimeout(out_result, msTimeout);
        result = true;
    }

    return result;
}

inline void NESynchTypesIX::convTimeout( timespec & out_result, unsigned int msTimeout )
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

inline const char * NESynchTypesIX::getString(NESynchTypesIX::eEventResetInfo val)
{
    switch (val)
    {
    case NESynchTypesIX::eEventResetInfo::EventResetManual:
        return "NESynchTypesIX::EventResetManual";
    case NESynchTypesIX::eEventResetInfo::EventResetAutomatic:
        return "NESynchTypesIX::EventResetAutomatic";
    default:
        return "ERR: Unexpected NESynchTypesIX::eEventResetInfo value!";
    }
}

inline const char * NESynchTypesIX::getString(NESynchTypesIX::eMatchCondition val)
{
    switch (val)
    {
    case NESynchTypesIX::eMatchCondition::MatchConditionExact:
        return "NESynchTypesIX::MatchConditionExact";
    case NESynchTypesIX::eMatchCondition::MatchConditionAny:
        return "NESynchTypesIX::MatchConditionAny";
    default:
        return "ERR: Unexpected NESynchTypesIX::eMatchCondition value!";
    }
}

inline const char * NESynchTypesIX::getString(NESynchTypesIX::eSynchObject val)
{
    switch (val)
    {
    case NESynchTypesIX::eSynchObject::SoUndefined:
        return "NESynchTypesIX::SoUndefined";
    case NESynchTypesIX::eSynchObject::SoWaitable:
        return "NESynchTypesIX::SoWaitable";
    case NESynchTypesIX::eSynchObject::SoMutex:
        return "NESynchTypesIX::SoMutex";
    case NESynchTypesIX::eSynchObject::SoSpinLock:
        return "NESynchTypesIX::SoSpinLock";
    case NESynchTypesIX::eSynchObject::SoWaitMutex:
        return "NESynchTypesIX::SoWaitMutex";
    case NESynchTypesIX::eSynchObject::SoWaitEvent:
        return "NESynchTypesIX::SoWaitEvent";
    case NESynchTypesIX::eSynchObject::SoWaitSemaphore:
        return "NESynchTypesIX::SoWaitSemaphore";
    case NESynchTypesIX::eSynchObject::SoWaitTimer:
        return "NESynchTypesIX::SoWaitTimer";
    default:
        return "ERR: Unexpected NESynchTypesIX::eSynchObject value!";
    }
}

#endif // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
