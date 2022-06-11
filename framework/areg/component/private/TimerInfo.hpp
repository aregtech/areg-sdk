#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerInfo.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform. Timer Info class.
 *              Stores timer, timer state and timer fire information.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/Containers.hpp"
#include "areg/component/Timer.hpp"


/************************************************************************
 * \brief       Timer Information class, Timer Expiration information,
 *              Timer Table and Expired Timer list classes used by
 *              Timer manager to handle timers.
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class TimerInfo;
class ExpiredTimerInfo;
class ExpiredTimers;

/************************************************************************
 * Dependencies
 ************************************************************************/
class Timer;

//////////////////////////////////////////////////////////////////////////
// TimerInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The TimerInfo class contains information about valid pending Timers.
 *          It has valid Handle of Timer, the thread ID of timer where
 *          it was started, first starting and last expired information, and
 *          the timer state.
 *
 * \note    <b>Attentions:</b> The Timer owning thread must be Dispatcher thread.
 *                      Otherwise the sending fired events will fail.
 * \note    The used timer values may differ on different systems.
 *          For example, on Windows machine the epoch is  Jan 1, 1601.
 *          On Unix / Linux machines the epoch is Jan 1, 1970.
 *          The difference of time between these 2: 116444736000000000LL of 100-nanoseconds.
 *
 **/
class AREG_API TimerInfo
{
//////////////////////////////////////////////////////////////////////////
// Friend class to access internals
//////////////////////////////////////////////////////////////////////////
    friend class TimerManager;

//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   TimerInfo::eTimerState
     *          Timer states
     **/
    typedef enum class E_TimerState
    {
          TimerInvalid  = -1    //!< Invalid state
        , TimerIdle             //!< Timer is idle
        , TimerPending          //!< Timer is pending
        , TimerExpired          //!< Timer is expired

    } eTimerState;

    /**
     * \brief   TimerInfo::getString()
     *          Returns the string value of TimerInfo::eTimerState type
     **/
    static const char* getString(TimerInfo::eTimerState timerState);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Required by Timer Table object
     **/
    TimerInfo( void );
    /**
     * \brief   Initializing Timer information
     * \param   timer           The valid timer object
     * \param   timerHandle     The valid timer handle
     * \param   ownThreadId     The timer owning dispatcher thread ID.
     **/
    TimerInfo( Timer & timer, TIMERHANDLE timerHandle, id_type ownThreadId);
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    TimerInfo( const TimerInfo & src );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    TimerInfo( TimerInfo && src ) noexcept;
    /**
     * \brief   Destructor
     **/
    ~TimerInfo( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Timer information from given source.
     **/
    TimerInfo & operator = ( const TimerInfo & src );

    /**
     * \brief   Moves Timer information from given source.
     **/
    TimerInfo & operator = ( TimerInfo && src ) noexcept;

    /**
     * \brief   Checks equality of 2 timer objects and returns true, if equal. 
     * \remark  Required by Timer Table object. 2 Timer Information objects are equal, if the timer
     *          objects have same handle.
     **/
    bool operator == ( const TimerInfo & other ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the pointer to Timer object
     **/
    inline Timer * getTimer( void ) const;

    /**
     * \brief   Returns timer handle
     **/
    inline TIMERHANDLE getHandle( void ) const;

    /**
     * \brief   Returns the owning dispatcher thread ID
     **/
    inline id_type getOwnerThreadId( void ) const;

    /**
     * \brief   Returns first starting time. The value is system dependent.
     **/
    inline const uint64_t & getStartingTime( void ) const;
    /**
     * \brief   Returns last fired time. The value is system dependent.
     **/
    inline const uint64_t & getExprirationTime( void ) const;

    /**
     * \brief   Returns the current state of timer.
     **/
    inline TimerInfo::eTimerState getTimerState( void ) const;

    /**
     * \brief   Returns true, if can start timer.
     *          The timer can be started, if the state is either Idle and
     *          the timer event count is more than zero.
     **/
    inline bool canStartTimer( void );

    /**
     * \brief   Returns true if timer is still active, i.e. the state is not Idle and
     *          it did not reach event count limit.
     **/
    inline bool isTimerActive( void ) const;

    /**
     * \brief   Resets the state of active timer to pending state and returns true if operation succeeded.
     **/
    inline bool resetActiveTimer( void );

    /**
     * \brief   Called when timer is expired. Returns true if successfully
     *          changed timer state and updated values. If timer state is not
     *          TimerInfo::TimerPending, it will ignore updating values.
     *          The function updates timer state.
     * \return  Returns true if timer state has been updated.
     **/
    bool timerExpired( unsigned int highValue, unsigned int lowValue );

    /**
     * \brief   Called when going to start timer. It will starting time.
     * \param   highValue   The start time high value
     * \param   lowValue    The start time low value
     **/
    void timerStarting(unsigned int highValue, unsigned int lowValue);

    /**
     * \brief   Called when timer is expired. Returns true, if timer is not in Idle state,
     *          the timer events can be fired and can be continued. Otherwise, it returns
     *          false indicating the timer manager can unregister the timer.
     * \param   expiredTimer    The instance of expired timer information.
     * \return  Returns true if timer is not Idle and can be continued.
     *          Otherwise, returns false and the timer should be stopped.
     */
    bool canContinueTimer( const ExpiredTimerInfo & expiredTimer );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to Timer object.
     **/
    Timer *         mTimer;
    /**
     * \brief   Timer Handle
     **/
    TIMERHANDLE     mHandle;
    /**
     * \brief   The ID of dispatcher thread started timer
     **/
    id_type         mOwnThreadId;

    /**
     * \brief   First starting time. The value is system dependent.
     **/
    uint64_t        mStartedAt;
    /**
     * \brief   Last fired time. The value is system dependent.
     **/
    uint64_t        mExpiredAt;
    /**
     * \brief   Timer state
     **/
    eTimerState     mTimerState;
};

//////////////////////////////////////////////////////////////////////////
// ExpiredTimerInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The ExpiredTimerInfo class contains information of expired Timers.
 *          It has value of fired time in absolute Coordinated Universal Time (UTC) format.
 *
 **/
class AREG_API ExpiredTimerInfo
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets Timer object and fired time in absolute Coordinated Universal Time (UTC) format.
     * \param   timer           The expired Timer object
     * \param   timeLowValue    Low 32-bit value of time in absolute Coordinated Universal Time (UTC) format.
     * \param   timerHighValue  High 32-bit value of time in absolute Coordinated Universal Time (UTC) format.
     **/
    inline ExpiredTimerInfo( Timer * timer, unsigned int timeLowValue, unsigned int timerHighValue);
    /**
     * \brief   Default constructor.
     **/
    inline ExpiredTimerInfo( void );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline ExpiredTimerInfo( const ExpiredTimerInfo & src );
    /**
     * \brief   Move constructor.
     * \param   src     The source of data to move.
     **/
    inline ExpiredTimerInfo( ExpiredTimerInfo && src ) noexcept;
    /**
     * \brief   Destructor
     **/
    inline ~ExpiredTimerInfo( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     **/
    ExpiredTimerInfo & operator = ( const ExpiredTimerInfo & src );

    /**
     * \brief   Moves data from given source.
     **/
    ExpiredTimerInfo & operator = ( ExpiredTimerInfo && src ) noexcept;

    /**
     * \brief   Checks equality of objects and returns true if they are equal.
     * \remark  Required by list of expired timers.
     **/
    inline bool operator == (const ExpiredTimerInfo & other) const;

//////////////////////////////////////////////////////////////////////////
// Member variables. Publicly accessible
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Pointer of expired Timer object.
     **/
    Timer *         mTimer;
    /**
     * \brief   Low 32-bit value of time. The value is system dependent.
     **/
    unsigned int    mLowValue;
    /**
     * \brief   High 32-bit value of time. The value is system dependent.
     **/
    unsigned        mHighValue;
};

//////////////////////////////////////////////////////////////////////////
// MapTimerTable
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Overrides methods required by Time Table hash map object.
 **/
using ImplTimerTable    = TEPointerHashMapImpl<Timer *, TimerInfo>;
using TimerTableBase    = TEHashMap<Timer*, TimerInfo, ImplTimerTable>;

/**
 * \brief   Timer Table Hash Map contains list of registered timers. 
 *          The Timer is registered when it is requested to start. 
 *          The timer is unregistered when it is stopped or
 *          no longer active (i.e. no more needed to be fired).
 *          The Continuous timers are remaining active until they are not stopped.
 **/
class MapTimerTable    : private TimerTableBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor
     **/
    MapTimerTable( void ) = default;

    /**
     * \brief   Destructor.
     **/
    ~MapTimerTable( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the map is empty.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns number of entries in the map.
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Checks whether specified timer exists in the hash map.
     * \param   key     The pointer to timer object to check.
     * \return  Returns true if specified timer exists in the map.
     **/
    inline bool keyExists( Timer * key ) const;

    /**
     * \brief   Searches specified timer pointer as a key. If the entry in hash map exists
     *          the parameter 'object' on output contains valid TimerInfo data.
     * \param   key     The pointer to timer object to search.
     * \param   object  On output, it contains data that is stored in the hash map.
     * \return  Returns true if specified timer exists in the hash map and the 
     *          'object' parameter contains valid stored data.
     **/
    inline bool findObject( Timer * key, TimerInfo & OUT object ) const;

    /**
     * \brief   Searches specified timer pointer as a key. If the entry in hash map exists
     *          returns valid TimerInfo pointer. Otherwise, returns nullptr.
     * \param   key     The pointer to timer object to search.
     * \return  Returns valid pointer to TimerInfor if specified Timer key exists in the map.
     *          Otherwise, returns nullptr.
     **/
    inline TimerInfo * findObject( Timer * key );

    /**
     * \brief   Removes all timer entries from hash map.
     **/
    inline void removeAll( void );

    /**
     * \brief   Registers specified TimerInfor data in the hash map, where the key
     *          is specified pointer to Timer object and the value is TimerInfo object.
     **/
    void registerObject(Timer * key, const TimerInfo & object);

    /**
     * \brief   Updates existing entry in the hash map. If specified pointer to Timer
     *          is not registered in the system, no data is updated and the return 
     *          value is false.
     * \param   key     The pointer to valid timer to update. If the entry does not exist
     *                  in the hash map, nothing is modified.
     * \param   object  The timer information object to update.
     * \return  Returns true if specified timer is registered in the map and the
     *          value is updated.
     **/
    bool updateObject( Timer * key, const TimerInfo & object );

    /**
     * \brief   Unregisters previously registered key as a pointer to Timer object.
     * \param   key     The pointer to valid timer to unregister.
     * \return  Returns true if a specified pointer to timer is in the system
     *          and it could unregister.
     **/
    bool unregisterObject( Timer * key );

    /**
     * \brief   Unregisters previously registered key as a pointer to Timer object.
     *          On output the 'object' parameter contains unregistered object data.
     * \param   key     The pointer to valid timer to unregister.
     * \param   object  On output this contains unregistered data.
     * \return  Returns true if a specified pointer to timer is in the system
     *          and it could unregister.
     **/
    bool unregisterObject( Timer * key, TimerInfo & OUT object );

    /**
     * \brief   Unregisters the first entry in the hash-map.
     *          On output, the specified parameters contain unregistered pointer
     *          to Timer and unregistered TimerInfor objects.
     * \param   key     On output this contains the pointer to unregistered Timer.
     * \param   object  On output this contains the unregistered Timer information saved in the map.
     * \return  Returns true if succeeded to unregister first entry. Returns nullptr, if there are
     *          no more entries in the list.
     **/
    bool unregisterFirstObject( Timer* & OUT key, TimerInfo & OUT object );

    /**
     * \brief   Resets the active timer state to pending
     * \param   key         The timer to search in the table.
     * \return  Returns true if the specified timer is registered in table and is active.
     **/
    bool resetActiveTimerState(Timer * key);

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( MapTimerTable );
};

//////////////////////////////////////////////////////////////////////////
// ExpiredTimers class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Expired Timer List contains information of expired timers.
 *          If timer is valid, it will be processed and the event will be
 *          sent to appropriate owning thread.
 **/
class ExpiredTimers  : private TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    ExpiredTimers( void ) = default;
    /**
     * \brief   Destructor
     **/
    ~ExpiredTimers( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the list is empty.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns number of entries in the list.
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Push expired timer information to the list.
     **/
    inline void pushTimer( ExpiredTimerInfo & timerInfo );

    /**
     * \brief   Pops expired timer information from the list, reduces size by one.
     **/
    inline ExpiredTimerInfo popTimer( void );

    /**
     * \brief   Removes of all entries from the list, makes it empty.
     **/
    inline void removeAll( void );

    /**
     * \brief   Searches specified timer in the list of expired timers and returns
     *          valid position if found. Searching starts either from beginning
     *          of the list from specified position.
     * \param   whichTimer      The pointer to timer object to search in the list.
     * \param   searchAfter     If nullptr, searches from beginning of the list.
     *                          Otherwise, searches from specified valid position.
     * \return  Returns valid position if found an entry. Otherwise, returns nullptr.
     **/
    LISTPOS findTimer(Timer * whichTimer, LISTPOS searchAfter = nullptr);

    /**
     * \brief   Removes all matches of specified timer in the list. The expired timer list
     *          may have several entries that have same timer. For example, if the timeout is very small,
     *          it might be more than one entry in the list.
     * \param   whichTimer      The pointer to valid timer object to remove from the list.
     * \return  Returns the number of entries that were removed.
     **/
    int removeAllTimers( Timer * whichTimer );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ExpiredTimers );
};

/************************************************************************
 * Inline function implementation
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// MapTimerTable class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool MapTimerTable::isEmpty(void) const
{
    return TimerTableBase::isEmpty();
}

inline int MapTimerTable::getSize(void) const
{
    return TimerTableBase::getSize();
}

inline bool MapTimerTable::keyExists(Timer * key) const
{
    return TimerTableBase::contains(key);
}

inline bool MapTimerTable::findObject(Timer * key, TimerInfo & OUT object) const
{
    return TimerTableBase::find(key, object);
}

inline TimerInfo * MapTimerTable::findObject(Timer * key)
{
    TimerInfo * result = nullptr;
    if (TimerTableBase::contains(key))
    {
        result = &TimerTableBase::getAt(key);
    }

    return result;
}

inline void MapTimerTable::removeAll(void)
{
    TimerTableBase::removeAll();
}

//////////////////////////////////////////////////////////////////////////
// ExpiredTimers class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool ExpiredTimers::isEmpty(void) const
{
    return TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::isEmpty();
}

inline int ExpiredTimers::getSize( void ) const
{
    return TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::getSize( );
}

inline void ExpiredTimers::pushTimer(ExpiredTimerInfo & timerInfo)
{
    TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::pushLast( timerInfo );
}

inline ExpiredTimerInfo ExpiredTimers::popTimer(void)
{
    return TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::removeFirst();
}

inline void ExpiredTimers::removeAll(void)
{
    return TELinkedList<ExpiredTimerInfo, const ExpiredTimerInfo &>::removeAll();
}

//////////////////////////////////////////////////////////////////////////
// TimerInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline Timer* TimerInfo::getTimer( void ) const
{
    return mTimer;
}

inline TIMERHANDLE TimerInfo::getHandle( void ) const
{
    return mHandle;
}

inline const uint64_t & TimerInfo::getStartingTime( void ) const
{
    return mStartedAt;
}

inline const uint64_t& TimerInfo::getExprirationTime( void ) const
{
    return mExpiredAt;
}

inline id_type TimerInfo::getOwnerThreadId( void ) const
{
    return mOwnThreadId;
}

inline TimerInfo::eTimerState TimerInfo::getTimerState( void ) const
{
    return mTimerState;
}

inline bool TimerInfo::isTimerActive( void ) const
{
    return ((mTimerState != TimerInfo::eTimerState::TimerIdle) && mTimer->isActive());
}

inline bool TimerInfo::canStartTimer( void )
{
    ASSERT(mTimer != nullptr);
    return ((mTimerState == TimerInfo::eTimerState::TimerIdle) && mTimer->timerIsStarting());
}

inline bool TimerInfo::resetActiveTimer( void )
{
    bool result = false;
    if (isTimerActive())
    {
        mTimerState = TimerInfo::eTimerState::TimerPending;
        result      = true;
    }

    return result;
}

inline const char* TimerInfo::getString( TimerInfo::eTimerState timerState )
{
    switch (timerState)
    {
        CASE_MAKE_STRING(TimerInfo::eTimerState::TimerInvalid);
        CASE_MAKE_STRING(TimerInfo::eTimerState::TimerIdle);
        CASE_MAKE_STRING(TimerInfo::eTimerState::TimerPending);
        CASE_MAKE_STRING(TimerInfo::eTimerState::TimerExpired);
        CASE_DEFAULT("ERR: Undefined TimerInfo::eTimerState value!");
    }
}

//////////////////////////////////////////////////////////////////////////
// ExpiredTimerInfo class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline ExpiredTimerInfo::ExpiredTimerInfo( void )
    : mTimer    ( nullptr )
    , mLowValue ( 0 )
    , mHighValue( 0 )
{
}

inline ExpiredTimerInfo::ExpiredTimerInfo( Timer *timer, unsigned int highValue, unsigned int lowValue )
    : mTimer    ( timer     )
    , mLowValue ( lowValue  )
    , mHighValue( highValue )
{
}

inline ExpiredTimerInfo::ExpiredTimerInfo( const ExpiredTimerInfo & src )
    : mTimer    ( src.mTimer    )
    , mLowValue ( src.mLowValue )
    , mHighValue( src.mHighValue)
{
}

inline ExpiredTimerInfo::ExpiredTimerInfo( ExpiredTimerInfo && src ) noexcept
    : mTimer    ( src.mTimer )
    , mLowValue ( src.mLowValue )
    , mHighValue( src.mHighValue )
{
}

inline bool ExpiredTimerInfo::operator == ( const ExpiredTimerInfo & other ) const
{
    return (mTimer == other.mTimer);
}
