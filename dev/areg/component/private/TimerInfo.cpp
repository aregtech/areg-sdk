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
    , mLock( )
{
    ; // do nothing
}

ExpiredTimers::~ExpiredTimers( void )
{
    ; // do nothing
}

LISTPOS ExpiredTimers::findTimer(Timer * whichTimer, LISTPOS searchAfter /*= NULL*/)
{
    Lock lock(mLock);

    TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::Block * block = mHead;
    if ( (searchAfter != NULL) && (mCount != 0) )
    {
        block = reinterpret_cast<TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::Block *>(searchAfter)->mNext;
    }

    for ( ; block != NULL; block = block->mNext )
    {
        if ( block->mValue.mTimer == whichTimer )
            break;
    }

    return reinterpret_cast<LISTPOS>(block);
}

int ExpiredTimers::removeAllTimers( Timer * whichTimer )
{
    Lock lock(mLock);

    int result = 0;

    if (whichTimer != NULL)
    {
        TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::Block * block = mHead;

        while (block != NULL)
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
// MapTimerTable
//////////////////////////////////////////////////////////////////////////
MapTimerTable::MapTimerTable(void)
    : TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>( )
{

}

MapTimerTable::~MapTimerTable(void)
{

}

void MapTimerTable::registerObject(const Timer * key, const TimerInfo & object)
{
    ASSERT(mHashTable != static_cast<Block**>(NULL));

    unsigned int hash = TemplateConstants::MAP_INVALID_HASH;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>::Block* block = TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>::blockAt(key, hash);
    if (block == static_cast<Block *>(NULL))
    {
        ASSERT(hash != TemplateConstants::MAP_INVALID_HASH);

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
    ASSERT( mHashTable != static_cast<Block **>(NULL) );

    unsigned int hash = TemplateConstants::MAP_INVALID_HASH;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>::Block* block = TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>::blockAt(key, hash);
    if (block != static_cast<Block *>(NULL))
    {
        block->mValue= object;
        result = false;
    }

    return result;
}

bool MapTimerTable::unregisterObject(const Timer * key)
{
    bool result = false;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>::Block** block = blockAt(key);
    if ((block != NULL) && (*block != NULL))
    {
        removeBlock(block);
        result = true;
    }

    return result;
}

bool MapTimerTable::unregisterObject(const Timer * key, TimerInfo & OUT object)
{
    bool result = false;
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>::Block** block = blockAt(key);
    if ((block != NULL) && (*block != NULL))
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
    TEHashMap<Timer *, TimerInfo, const Timer *, const TimerInfo &, TimerTableImpl>::Block* block = firstValidBlock();
    if (block != NULL)
    {
        key     = block->mKey;
        object  = block->mValue;
        removeBlock(blockReference(*block));
        result = true;
    }

    return result;
}
