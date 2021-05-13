/************************************************************************
 * \file        areg/component/private/TimerInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Timer Info class.
 *              Stores timer, timer state and timer fire information.
 *
 ************************************************************************/
#include "areg/component/private/TimerInfo.hpp"

#include "areg/base/Thread.hpp"

//////////////////////////////////////////////////////////////////////////
// TimerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TimerInfo class, constructors / destructor
//////////////////////////////////////////////////////////////////////////
TimerInfo::TimerInfo( void )
    : mTimer        (NULL)
    , mHandle       (NULL)
    , mOwnThreadId  (Thread::INVALID_THREAD_ID)
    , mStartedAt    ( 0 )
    , mExpiredAt    ( 0 )
    , mTimerState   (TimerInfo::TimerInvalid)
{
    ; // do nothing
}

TimerInfo::TimerInfo( Timer &timer, TIMERHANDLE timerHandle, ITEM_ID ownThreadId )
    : mTimer        (&timer)
    , mHandle       (timerHandle)
    , mOwnThreadId  (ownThreadId)
    , mStartedAt    ( 0 )
    , mExpiredAt    ( 0 )
    , mTimerState   (TimerInfo::TimerIdle)
{
    ; // do nothing
}

TimerInfo::TimerInfo( const TimerInfo & src )
    : mTimer        (src.mTimer)
    , mHandle       (src.mHandle)
    , mOwnThreadId  (src.mOwnThreadId)
    , mStartedAt    (src.mStartedAt)
    , mExpiredAt    (src.mExpiredAt)
    , mTimerState   (src.mTimerState)
{
    ; // do nothing
}

TimerInfo::~TimerInfo( void )
{
    // AAvetyan:    The destructor directly called in Hash Map where timer info is saved.
    mTimer      = NULL;
    mHandle     = NULL;
    mOwnThreadId= Thread::INVALID_THREAD_ID;
    mStartedAt  = 0;
    mExpiredAt  = 0;
    mTimerState = TimerInfo::TimerInvalid;
}

//////////////////////////////////////////////////////////////////////////
// TimerInfo class, operators
//////////////////////////////////////////////////////////////////////////
const TimerInfo& TimerInfo::operator = ( const TimerInfo & src )
{
    if (static_cast<const TimerInfo *>(this) != &src)
    {
        mTimer      = src.mTimer;
        mHandle     = src.mHandle;
        mOwnThreadId= src.mOwnThreadId;
        mStartedAt  = src.mStartedAt;
        mExpiredAt  = src.mExpiredAt;
        mTimerState = src.mTimerState;
    }
    return (*this);
}

bool TimerInfo::operator == ( const TimerInfo & other ) const
{
    return (this != &other ? mTimer == other.mTimer : false);
}

//////////////////////////////////////////////////////////////////////////
// TimerInfo class, methods
//////////////////////////////////////////////////////////////////////////
bool TimerInfo::isTimerExpired( unsigned int highValue, unsigned int lowValue )
{
    bool result = false;
    if (mTimerState == TimerInfo::TimerPending)
    {
        uint64_t expiredAt = MACRO_MAKE_64( highValue, lowValue );
        if (mStartedAt <= expiredAt && mExpiredAt <= expiredAt)
        {
            mExpiredAt = expiredAt;
            mTimer->timerIsExpired( highValue, lowValue);
            mTimerState= mTimer->isActive() ? TimerInfo::TimerExpired : TimerInfo::TimerIdle;
            result = true;
        }
    }
    return result;
}

void TimerInfo::isTimerStarting( unsigned int highValue, unsigned int lowValue )
{
    ASSERT(mTimerState == TimerInfo::TimerIdle);
    mStartedAt    = MACRO_MAKE_64(highValue, lowValue);
    mTimerState   = TimerInfo::TimerPending;
}

//////////////////////////////////////////////////////////////////////////
// ExpiredTimerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ExpiredTimerInfo class, methods
//////////////////////////////////////////////////////////////////////////
const ExpiredTimerInfo& ExpiredTimerInfo::operator = ( const ExpiredTimerInfo & src )
{
    if (static_cast<const ExpiredTimerInfo *>(this) != &src)
    {
        mTimer    = src.mTimer;
        mLowValue = src.mLowValue;
        mHighValue= src.mHighValue;
    }
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ExpiredTimers class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ExpiredTimers class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ExpiredTimers::ExpiredTimers( void )
    : TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &> ( )
{
    ; // do nothing
}

ExpiredTimers::~ExpiredTimers( void )
{
    ; // do nothing
}
