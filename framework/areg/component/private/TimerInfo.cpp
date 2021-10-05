/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform. Timer Info class.
 *              Stores timer, timer state and timer fire information.
 *
 ************************************************************************/
#include "areg/component/private/TimerInfo.hpp"

#include "areg/base/Thread.hpp"

#include <utility>

 //////////////////////////////////////////////////////////////////////////
// TimerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TimerInfo class, constructors / destructor
//////////////////////////////////////////////////////////////////////////
TimerInfo::TimerInfo( void )
    : mTimer        (nullptr)
    , mHandle       (nullptr)
    , mOwnThreadId  (Thread::INVALID_THREAD_ID)
    , mStartedAt    ( 0 )
    , mExpiredAt    ( 0 )
    , mTimerState   (TimerInfo::eTimerState::TimerInvalid)
{
}

TimerInfo::TimerInfo( Timer &timer, TIMERHANDLE timerHandle, id_type ownThreadId )
    : mTimer        (&timer)
    , mHandle       (timerHandle)
    , mOwnThreadId  (ownThreadId)
    , mStartedAt    ( 0 )
    , mExpiredAt    ( 0 )
    , mTimerState   (TimerInfo::eTimerState::TimerIdle)
{
}

TimerInfo::TimerInfo( const TimerInfo & src )
    : mTimer        (src.mTimer)
    , mHandle       (src.mHandle)
    , mOwnThreadId  (src.mOwnThreadId)
    , mStartedAt    (src.mStartedAt)
    , mExpiredAt    (src.mExpiredAt)
    , mTimerState   (src.mTimerState)
{
}

TimerInfo::TimerInfo( TimerInfo && src ) noexcept
    : mTimer        ( std::move(src.mTimer) )
    , mHandle       ( std::move( src.mHandle) )
    , mOwnThreadId  ( std::move( src.mOwnThreadId) )
    , mStartedAt    ( std::move( src.mStartedAt) )
    , mExpiredAt    ( std::move( src.mExpiredAt) )
    , mTimerState   ( std::move( src.mTimerState) )
{
}

//////////////////////////////////////////////////////////////////////////
// TimerInfo class, operators
//////////////////////////////////////////////////////////////////////////
TimerInfo& TimerInfo::operator = ( const TimerInfo & src )
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

TimerInfo & TimerInfo::operator = ( TimerInfo && src ) noexcept
{
    if ( static_cast<TimerInfo *>(this) != &src )
    {
        mTimer      = std::move( src.mTimer );
        mHandle     = std::move( src.mHandle );
        mOwnThreadId= std::move( src.mOwnThreadId );
        mStartedAt  = std::move( src.mStartedAt );
        mExpiredAt  = std::move( src.mExpiredAt );
        mTimerState = std::move( src.mTimerState );
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
bool TimerInfo::timerExpired( unsigned int highValue, unsigned int lowValue )
{
    bool result = false;
    if (mTimerState == TimerInfo::eTimerState::TimerPending)
    {
        uint64_t expiredAt = MACRO_MAKE_64( highValue, lowValue );
        if ((mStartedAt <= expiredAt) && (mExpiredAt <= expiredAt))
        {
            mTimerState= TimerInfo::eTimerState::TimerIdle;
            mExpiredAt = expiredAt;
            if (mTimer->timerIsExpired( highValue, lowValue))
            {
                mTimerState = TimerInfo::eTimerState::TimerExpired;
                result      = true;
            }
        }
#ifdef _DEBUG
        else
        {
            OUTPUT_ERR("Missing timer expired value. mStartedAt = [ %u ], mExpiredAt = [ %u ], expiredAt = [ %u ]", mStartedAt, mExpiredAt, expiredAt);
        }
#endif // !_DEBUG
    }
    return result;
}

bool TimerInfo::canContinueTimer( const ExpiredTimerInfo & expiredTimer )
{
    bool result = false;
    if ( isTimerActive() )
    {
        uint32_t highValue  = expiredTimer.mHighValue;
        uint32_t lowValue   = expiredTimer.mLowValue;
        uint64_t expiredAt  = MACRO_MAKE_64( highValue, lowValue );
        mExpiredAt = expiredAt;
        if (mTimer->timerIsExpired( highValue, lowValue))
        {
            mTimerState = TimerInfo::eTimerState::TimerPending;
            result      = true;
        }
    }

    return result;
}

void TimerInfo::timerStarting( unsigned int highValue, unsigned int lowValue )
{
    ASSERT(mTimerState == TimerInfo::eTimerState::TimerIdle);
    mStartedAt    = MACRO_MAKE_64(highValue, lowValue);
    mTimerState   = TimerInfo::eTimerState::TimerPending;
}

//////////////////////////////////////////////////////////////////////////
// ExpiredTimerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ExpiredTimerInfo class, methods
//////////////////////////////////////////////////////////////////////////
ExpiredTimerInfo & ExpiredTimerInfo::operator = ( const ExpiredTimerInfo & src )
{
    if (static_cast<const ExpiredTimerInfo *>(this) != &src)
    {
        mTimer    = src.mTimer;
        mLowValue = src.mLowValue;
        mHighValue= src.mHighValue;
    }

    return (*this);
}

ExpiredTimerInfo & ExpiredTimerInfo::operator = ( ExpiredTimerInfo && src ) noexcept
{
    if ( static_cast<ExpiredTimerInfo *>(this) != &src )
    {
        mTimer    = std::move( src.mTimer );
        mLowValue = std::move( src.mLowValue );
        mHighValue= std::move( src.mHighValue );
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ExpiredTimers class implementation
//////////////////////////////////////////////////////////////////////////

LISTPOS ExpiredTimers::findTimer(Timer * whichTimer, LISTPOS searchAfter /*= nullptr*/)
{
    TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::Block * block = mHead;
    if ( (searchAfter != nullptr) && (mCount != 0) )
    {
        block = reinterpret_cast<TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::Block *>(searchAfter)->mNext;
    }

    for ( ; block != nullptr; block = block->mNext )
    {
        if ( block->mValue.mTimer == whichTimer )
            break;
    }

    return reinterpret_cast<LISTPOS>(block);
}

int ExpiredTimers::removeAllTimers( Timer * whichTimer )
{
    int result = 0;

    if (whichTimer != nullptr)
    {
        TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::Block * block = mHead;

        while (block != nullptr)
        {
            TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::Block * next = block->mNext;
            if ( block->mValue.mTimer == whichTimer )
            {
                TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::removeAt( reinterpret_cast<LISTPOS>(block) );
                ++ result;
            }

            block = next;
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// MapTimerTable implementation
//////////////////////////////////////////////////////////////////////////

void MapTimerTable::registerObject(const Timer * key, const TimerInfo & object)
{
    ASSERT(mHashTable != nullptr);

    unsigned int hash = NECommon::MAP_INVALID_HASH;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::Block* block = TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::blockAt(key, hash);
    if (block == nullptr)
    {
        ASSERT(hash != NECommon::MAP_INVALID_HASH);

        // it doesn't exist, add a new Block
        int idx     = static_cast<int>(hash % mHashTableSize);
        block       = initNewBlock();
        block->mHash= hash;
        block->mKey	= const_cast<Timer *>(key);
        block->mNext= mHashTable[idx];
        mHashTable[idx]= block;
    }

    block->mValue= object;
}

bool MapTimerTable::updateObject(const Timer * key, const TimerInfo & object)
{
    bool result = false;
    ASSERT( mHashTable != nullptr );

    unsigned int hash = NECommon::MAP_INVALID_HASH;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::Block* block = TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::blockAt(key, hash);
    if (block != nullptr)
    {
        block->mValue= object;
        result = false;
    }

    return result;
}

bool MapTimerTable::unregisterObject(const Timer * key)
{
    bool result = false;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::Block** block = blockAtRef(key);
    if ((block != nullptr) && (*block != nullptr))
    {
        removeBlock(block);
        result = true;
    }

    return result;
}

bool MapTimerTable::unregisterObject(const Timer * key, TimerInfo & OUT object)
{
    bool result = false;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::Block** block = blockAtRef(key);
    if ((block != nullptr) && (*block != nullptr))
    {
        object = (*block)->mValue;
        removeBlock(block);
        result = true;
    }

    return result;
}

bool MapTimerTable::unregisterFirstObject(Timer * & OUT key, TimerInfo & OUT object)
{
    bool result = false;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::Block* block = firstValidBlock();
    if (block != nullptr)
    {
        key     = block->mKey;
        object  = block->mValue;
        removeBlock(blockReference(*block));
        result = true;
    }

    return result;
}

bool MapTimerTable::resetActiveTimerState(const Timer * key)
{
    bool result = false;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, ImplTimerTable>::Block** block = blockAtRef(key);
    if ((block != nullptr) && (*block != nullptr))
    {
        result = (*block)->mValue.resetActiveTimer();
    }

    return result;
}
