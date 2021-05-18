#ifndef AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
#define AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
/************************************************************************
 * \file        areg/base/private/posix/NESynchTypesIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#ifdef _POSIX

#include "areg/base/NECommon.hpp"
#include <unistd.h>
#include <time.h>

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
     * \brief   NESynchTypesIX::POSIX_SUCSS
     *          Indicates the success of POSIX function call.
     **/
    const int   POSIX_SUCSS                   = 0;

    /**
     * \brief   NESynchTypesIX::eSynchObjectFired
     *          The valid indexes when synchronization event is fired in the waiting list
     *          or error happened.
     **/
    typedef enum E_SynchObjectFired
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
    typedef enum    E_EventResetInfo
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
    typedef enum E_MatchCondition
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
    typedef enum E_SynchObject
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
    if ( NESynchTypesIX::POSIX_SUCSS == clock_gettime( CLOCK_REALTIME, &out_result ) )
    {
        convTimeout(out_result, msTimeout);
        result = true;
    }

    return result;
}

inline void NESynchTypesIX::convTimeout( timespec & out_result, unsigned int msTimeout )
{
    msTimeout += out_result.tv_nsec / 1000000;
    unsigned int nsec = (out_result.tv_nsec % 1000000);

    out_result.tv_sec   +=  msTimeout / 1000;
    out_result.tv_nsec   = ((msTimeout % 1000) * 1000000) + nsec;
}

inline const char * NESynchTypesIX::getString(NESynchTypesIX::eEventResetInfo val)
{
    switch (val)
    {
    case NESynchTypesIX::EventResetManual:
        return "NESynchTypesIX::EventResetManual";
    case NESynchTypesIX::EventResetAutomatic:
        return "NESynchTypesIX::EventResetAutomatic";
    default:
        return "ERR: Unexpected NESynchTypesIX::eEventResetInfo value!";
    }
}

inline const char * NESynchTypesIX::getString(NESynchTypesIX::eMatchCondition val)
{
    switch (val)
    {
    case NESynchTypesIX::MatchConditionExact:
        return "NESynchTypesIX::MatchConditionExact";
    case NESynchTypesIX::MatchConditionAny:
        return "NESynchTypesIX::MatchConditionAny";
    default:
        return "ERR: Unexpected NESynchTypesIX::eMatchCondition value!";
    }
}

inline const char * NESynchTypesIX::getString(NESynchTypesIX::eSynchObject val)
{
    switch (val)
    {
    case NESynchTypesIX::SoUndefined:
        return "NESynchTypesIX::SoUndefined";
    case NESynchTypesIX::SoWaitable:
        return "NESynchTypesIX::SoWaitable";
    case NESynchTypesIX::SoMutex:
        return "NESynchTypesIX::SoMutex";
    case NESynchTypesIX::SoSpinLock:
        return "NESynchTypesIX::SoSpinLock";
    case NESynchTypesIX::SoWaitMutex:
        return "NESynchTypesIX::SoWaitMutex";
    case NESynchTypesIX::SoWaitEvent:
        return "NESynchTypesIX::SoWaitEvent";
    case NESynchTypesIX::SoWaitSemaphore:
        return "NESynchTypesIX::SoWaitSemaphore";
    case NESynchTypesIX::SoWaitTimer:
        return "NESynchTypesIX::SoWaitTimer";
    default:
        return "ERR: Unexpected NESynchTypesIX::eSynchObject value!";
    }
}

#endif // _POSIX

#endif // AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
