/************************************************************************
 * \file        areg/src/component/private/CETimerInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Timer Info class.
 *              Stores timer, timer state and timer fire information.
 *
 ************************************************************************/
#include "areg/src/component/private/CETimerInfo.hpp"

#include "areg/src/base/CEThread.hpp"

//////////////////////////////////////////////////////////////////////////
// CETimerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETimerInfo class, constructors / destructor
//////////////////////////////////////////////////////////////////////////
CETimerInfo::CETimerInfo( void )
    : mTimer        (NULL)
    , mHandle       (NULL)
    , mOwnThreadId  (CEThread::INVALID_THREAD_ID)
    , mStartedAt    ( 0 )
    , mExpiredAt    ( 0 )
    , mTimerState   (CETimerInfo::TimerInvalid)
{
    ; // do nothing
}

CETimerInfo::CETimerInfo( CETimer &timer, TIMERHANDLE timerHandle, ITEM_ID ownThreadId )
    : mTimer        (&timer)
    , mHandle       (timerHandle)
    , mOwnThreadId  (ownThreadId)
    , mStartedAt    ( 0 )
    , mExpiredAt    ( 0 )
    , mTimerState   (CETimerInfo::TimerIdle)
{
    ; // do nothing
}

CETimerInfo::CETimerInfo( const CETimerInfo & src )
    : mTimer        (src.mTimer)
    , mHandle       (src.mHandle)
    , mOwnThreadId  (src.mOwnThreadId)
    , mStartedAt    (src.mStartedAt)
    , mExpiredAt    (src.mExpiredAt)
    , mTimerState   (src.mTimerState)
{
    ; // do nothing
}

CETimerInfo::~CETimerInfo( void )
{
    // AAvetyan:    The destructor directly called in Hash Map where timer info is saved.
    mTimer      = NULL;
    mHandle     = NULL;
    mOwnThreadId= CEThread::INVALID_THREAD_ID;
    mStartedAt  = 0;
    mExpiredAt  = 0;
    mTimerState = CETimerInfo::TimerInvalid;
}

//////////////////////////////////////////////////////////////////////////
// CETimerInfo class, operators
//////////////////////////////////////////////////////////////////////////
const CETimerInfo& CETimerInfo::operator = ( const CETimerInfo & src )
{
    if (static_cast<const CETimerInfo *>(this) != &src)
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

bool CETimerInfo::operator == ( const CETimerInfo & other ) const
{
    return (this != &other ? mTimer == other.mTimer : false);
}

//////////////////////////////////////////////////////////////////////////
// CETimerInfo class, methods
//////////////////////////////////////////////////////////////////////////
bool CETimerInfo::TimerIsExpired( unsigned int highValue, unsigned int lowValue )
{
    bool result = false;
    if (mTimerState == CETimerInfo::TimerPending)
    {
        uint64_t expiredAt = MACRO_MAKE_64( highValue, lowValue );
        if (mStartedAt <= expiredAt && mExpiredAt <= expiredAt)
        {
            mExpiredAt = expiredAt;
            mTimer->TimerIsExpired( highValue, lowValue);
            mTimerState= mTimer->IsActive() ? CETimerInfo::TimerExpired : CETimerInfo::TimerIdle;
            result = true;
        }
    }
    return result;
}

void CETimerInfo::TimerIsStarting( unsigned int highValue, unsigned int lowValue )
{
    ASSERT(mTimerState == CETimerInfo::TimerIdle);
    mStartedAt    = MACRO_MAKE_64(highValue, lowValue);
    mTimerState   = CETimerInfo::TimerPending;
}

//////////////////////////////////////////////////////////////////////////
// CEExpiredTimerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEExpiredTimerInfo class, methods
//////////////////////////////////////////////////////////////////////////
const CEExpiredTimerInfo& CEExpiredTimerInfo::operator = ( const CEExpiredTimerInfo & src )
{
    if (static_cast<const CEExpiredTimerInfo *>(this) != &src)
    {
        mTimer    = src.mTimer;
        mLowValue = src.mLowValue;
        mHighValue= src.mHighValue;
    }
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// CEExpiredTimers class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEExpiredTimers class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEExpiredTimers::CEExpiredTimers( void )
    : TELinkedList<CEExpiredTimerInfo, const CEExpiredTimerInfo &> ( )
{
    ; // do nothing
}

CEExpiredTimers::~CEExpiredTimers( void )
{
    ; // do nothing
}
