#ifndef AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
#define AREG_BASE_PRIVATE_POSIX_NESYNCHTYPESIX_HPP
/************************************************************************
 * \file        areg/src/base/private/posix/NESynchTypesIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#ifdef _POSIX

#include <unistd.h>
#include <time.h>

namespace NESynchTypesIX
{

    const int   POSIX_SUCCESS                   = 0;


    typedef enum E_SynchObjectFired
    {
          SynchObjectInvalid    =  -1

        , SynchObject0          =   0
        , SynchObject1          =   1
        , SynchObject2          =   2
        , SynchObject3          =   3
        , SynchObject4          =   4
        , SynchObject5          =   5
        , SynchObject6          =   6
        , SynchObject7          =   7
        , SynchObject8          =   8
        , SynchObject9          =   9
        , SynchObject10         =  10
        , SynchObject11         =  11
        , SynchObject12         =  12
        , SynchObject13         =  13
        , SynchObject14         =  14
        , SynchObject15         =  15
        , SynchObject16         =  16
        , SynchObject17         =  17
        , SynchObject18         =  18
        , SynchObject19         =  19
        , SynchObject20         =  20
        , SynchObject21         =  21
        , SynchObject22         =  22
        , SynchObject23         =  23
        , SynchObject24         =  24
        , SynchObject25         =  25
        , SynchObject26         =  26
        , SynchObject27         =  27
        , SynchObject28         =  28
        , SynchObject29         =  29
        , SynchObject30         =  30
        , SynchObject31         =  31

        , SynchObjectAll        =  32

        , SynchObjectError      =  99
        , SynchObject0Error     = 100
        , SynchObject1Error     = 101
        , SynchObject2Error     = 102
        , SynchObject3Error     = 103
        , SynchObject4Error     = 104
        , SynchObject5Error     = 105
        , SynchObject6Error     = 106
        , SynchObject7Error     = 107
        , SynchObject8Error     = 108
        , SynchObject9Error     = 109
        , SynchObject10Error    = 110
        , SynchObject11Error    = 111
        , SynchObject12Error    = 112
        , SynchObject13Error    = 113
        , SynchObject14Error    = 114
        , SynchObject15Error    = 115
        , SynchObject16Error    = 116
        , SynchObject17Error    = 117
        , SynchObject18Error    = 118
        , SynchObject19Error    = 119
        , SynchObject20Error    = 120
        , SynchObject21Error    = 121
        , SynchObject22Error    = 122
        , SynchObject23Error    = 123
        , SynchObject24Error    = 124
        , SynchObject25Error    = 125
        , SynchObject26Error    = 126
        , SynchObject27Error    = 127
        , SynchObject28Error    = 128
        , SynchObject29Error    = 129
        , SynchObject30Error    = 130
        , SynchObject31Error    = 131

        , SynchAsynchSignal     = 200
        , SynchObjectTimeout    = 201
        , SynchWaitInterrupted  = 202
    } eSynchObjectFired;

    typedef enum    E_EventResetInfo
    {
          EventResetManual
        , EventResetAutomatic
    } eEventResetInfo;
    inline const char * GetString(NESynchTypesIX::eEventResetInfo val);

    typedef enum E_MatchCondition
    {
          MatchConditionExact
        , MatchConditionAny
    } eMatchCondition;
    inline const char * GetString(NESynchTypesIX::eMatchCondition val);

    typedef enum E_SynchObject
    {
          SoUndefined       = (0 << 0) | 0
        , SoWaitable        = (1 << 0) | 1
        , SoMutex           = (1 << 1) | 0
        , SoSpinLock        = (1 << 2) | 0
        , SoWaitMutex       = (1 << 1) | 1
        , SoWaitEvent       = (1 << 2) | 1
        , SoWaitSemaphore   = (1 << 3) | 1
        , SoWaitTimer       = (1 << 4) | 1

    } eSynchObject;
    inline const char * GetString(NESynchTypesIX::eSynchObject val);

    inline bool TimeoutFromNow( timespec & out_result, unsigned int msTimeout );

    inline void ConvertTimeout( timespec & out_result, unsigned int msTimeout );

} // namespace NESynchTypesIX

inline bool NESynchTypesIX::TimeoutFromNow( timespec & out_result, unsigned int msTimeout )
{
    bool result = false;
    if ( NESynchTypesIX::POSIX_SUCCESS == clock_gettime( CLOCK_REALTIME, &out_result ) )
    {
        ConvertTimeout(out_result, msTimeout);
        result = true;
    }

    return result;
}

inline void NESynchTypesIX::ConvertTimeout( timespec & out_result, unsigned int msTimeout )
{
    msTimeout += out_result.tv_nsec / 1000000;
    unsigned int nsec = (out_result.tv_nsec % 1000000);

    out_result.tv_sec   +=  msTimeout / 1000;
    out_result.tv_nsec   = ((msTimeout % 1000) * 1000000) + nsec;
}

inline const char * NESynchTypesIX::GetString(NESynchTypesIX::eEventResetInfo val)
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

inline const char * NESynchTypesIX::GetString(NESynchTypesIX::eMatchCondition val)
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

inline const char * NESynchTypesIX::GetString(NESynchTypesIX::eSynchObject val)
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
