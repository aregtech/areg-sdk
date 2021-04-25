#ifndef AREG_COMPONENT_PRIVATE_CETIMERINFO_HPP
#define AREG_COMPONENT_PRIVATE_CETIMERINFO_HPP
/************************************************************************
 * \file        areg/src/component/private/CETimerInfo.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Timer Info class.
 *              Stores timer, timer state and timer fire information.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/TEHashMap.hpp"
#include "areg/src/base/TELinkedList.hpp"
#include "areg/src/component/CETimer.hpp"


/************************************************************************
 * \brief       Timer Information class, Timer Expiration information,
 *              Timer Table and Expired Timer list classes used by
 *              Timer manager to handle timers.
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CETimerInfo;
class CEExpiredTimerInfo;
class CEExpiredTimers;

/************************************************************************
 * Dependencies
 ************************************************************************/
class CETimer;

//////////////////////////////////////////////////////////////////////////
// CETimerInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The CETimerInfo class contains information about valid pending Timers.
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
class AREG_API CETimerInfo
{
//////////////////////////////////////////////////////////////////////////
// Friend class to access internals
//////////////////////////////////////////////////////////////////////////
    friend class CETimerManager;

//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CETimerInfo::eTimerState
     *          Timer states
     **/
    typedef enum E_TimerState
    {
          TimerInvalid  = -1    //!< Invalid state
        , TimerIdle             //!< Timer is idle
        , TimerPending          //!< Timer is pending
        , TimerExpired          //!< Timer is expired

    } eTimerState;

    /**
     * \brief   CETimerInfo::GetString()
     *          Retunrs the string value of CETimerInfo::eTimerState type
     **/
    static const char* GetString(CETimerInfo::eTimerState timerState);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializing Timer information
     * \param   timer           The valid timer object
     * \param   timerHandle     The valid timer handle
     * \param   ownThreadId     The timer owning dispatcher thread ID.
     **/
    CETimerInfo( CETimer & timer, TIMERHANDLE timerHandle, ITEM_ID ownThreadId);
    /**
     * \brief   Default constructor. Required by Timer Table object
     **/
    CETimerInfo( void );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    CETimerInfo( const CETimerInfo & src );
    /**
     * \brief   Destructor
     **/
    ~CETimerInfo( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Timer information from given source.
     **/
    const CETimerInfo & operator = ( const CETimerInfo & src );

    /**
     * \brief   Checks equality of 2 timer objects and returns true, if equal. 
     * \remark  Required by Timer Table object. 2 Timer Information objects are equal, if the timer
     *          objects have same handle.
     **/
    bool operator == ( const CETimerInfo & other ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the pointer to Timer object
     **/
    inline CETimer * GetTimer( void ) const;

    /**
     * \brief   Returns timer handle
     **/
    inline TIMERHANDLE GetHandle( void ) const;

    /**
     * \brief   Returns the owning dispatcher thread ID
     **/
    inline ITEM_ID GetOwnThreadId( void ) const;

    /**
     * \brief   Returns first starting time. The value is system dependent.
     **/
    inline const uint64_t & GetStartingTime( void ) const;
    /**
     * \brief   Returns last fired time. The value is system dependent.
     **/
    inline const uint64_t & GetExprirationTime( void ) const;

    /**
     * \brief   Returns the current state of timer.
     **/
    inline CETimerInfo::eTimerState GetState( void ) const;

    /**
     * \brief   Returns true, if if can start timer.
     *          The timer can be started, if the state is either Idle and
     *          the timer event count is more than zero.
     **/
    inline bool CanStartTimer( void );

    /**
     * \brief   Returns true if timer is still active, i.e. the state is not Idle and
     *          it did not reach event count limit.
     **/
    inline bool IsActive( void ) const;

    /**
     * \brief   Called when timer is expired. Returns true if successfully
     *          changed timer state and updated values. If timer state is not
     *          CETimerInfo::TimerPending, it will ignore updating values.
     *          The function updates timer state.
     * \return  Returns true if timer state has been updated.
     **/
    bool TimerIsExpired( unsigned int highValue, unsigned int lowValue );

    /**
     * \brief   Called when going to start timer. It will starting time.
     * \param   highValue   The start time high value
     * \param   lowValue    The start time low value
     **/
    void TimerIsStarting(unsigned int highValue, unsigned int lowValue);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to Timer object.
     **/
    CETimer *       mTimer;
    /**
     * \brief   Timer Handle
     **/
    TIMERHANDLE     mHandle;
    /**
     * \brief   The ID of dispatcher thread started timer
     **/
    ITEM_ID         mOwnThreadId;

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
// CEExpiredTimerInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The CEExpiredTimerInfo class contains information of expired Timers.
 *          It has value of fired time in absolute Coordinated Universal Time (UTC) format.
 *
 **/
class AREG_API CEExpiredTimerInfo
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
    inline CEExpiredTimerInfo( CETimer * timer, unsigned int timeLowValue, unsigned int timerHighValue);
    /**
     * \brief   Default constructor.
     **/
    inline CEExpiredTimerInfo( void );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline CEExpiredTimerInfo( const CEExpiredTimerInfo & src );
    /**
     * \brief   Destructor
     **/
    inline ~CEExpiredTimerInfo( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     **/
    const CEExpiredTimerInfo & operator = ( const CEExpiredTimerInfo & src );

    /**
     * \brief   Checks equality of objects and returns true if they are equal.
     * \remark  Required by list of expired timers.
     **/
    inline bool operator == (const CEExpiredTimerInfo & other) const;

//////////////////////////////////////////////////////////////////////////
// Member variables. Publicly accessible
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Pointer of expired Timer object.
     **/
    CETimer *       mTimer;
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
class AREG_API CETimerTableImpl : public TEHashMapImpl<const CETimer *, const CETimerInfo &>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    inline unsigned int ImplHashKey( const CETimer * Key ) const
    {
        return MACRO_PTR2COUNT( Key );
    }
};

/**
 * \brief   Timer Table Hash Map contains list of registered timers. 
 *          The Timer is registered when it is requested to start. 
 *          The timer is unregistered when it is stopped or
 *          no longer active (i.e. no more needed to be fired).
 *          The Continuous timers are remaining active until they are not stopped.
 **/
typedef TEHashMap<CETimer *, CETimerInfo, const CETimer *, const CETimerInfo &, CETimerTableImpl>   MapTimerTable;

//////////////////////////////////////////////////////////////////////////
// CEExpiredTimers class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Expired Timer List contains information of expired timers.
 *          If timer is valid, it will be processed and the event will be
 *          sent to appropriate owning thread.
 **/
class AREG_API CEExpiredTimers  : public TELinkedList<CEExpiredTimerInfo, const CEExpiredTimerInfo &>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    CEExpiredTimers( void );
    /**
     * \brief   Destructor
     **/
    ~CEExpiredTimers( void );
};

/************************************************************************
 * Inline function implementation
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CETimerInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CETimer* CETimerInfo::GetTimer( void ) const
{   return mTimer;      }

inline TIMERHANDLE CETimerInfo::GetHandle( void ) const
{   return mHandle;     }

inline const uint64_t & CETimerInfo::GetStartingTime( void ) const
{   return mStartedAt;  }

inline const uint64_t& CETimerInfo::GetExprirationTime( void ) const
{   return mExpiredAt;  }

inline ITEM_ID CETimerInfo::GetOwnThreadId( void ) const
{   return mOwnThreadId;}

inline CETimerInfo::eTimerState CETimerInfo::GetState( void ) const
{   return mTimerState; }

inline bool CETimerInfo::IsActive( void ) const
{   return (mTimerState != TimerIdle && mTimer->IsActive());    }

inline bool CETimerInfo::CanStartTimer( void )
{   ASSERT(mTimer != NULL); return (mTimerState == CETimerInfo::TimerIdle && mTimer->TimerIsStarting());    }

inline const char* CETimerInfo::GetString( CETimerInfo::eTimerState timerState )
{
    switch (timerState)
    {
        CASE_MAKE_STRING(CETimerInfo::TimerInvalid);
        CASE_MAKE_STRING(CETimerInfo::TimerIdle);
        CASE_MAKE_STRING(CETimerInfo::TimerPending);
        CASE_MAKE_STRING(CETimerInfo::TimerExpired);
        CASE_DEFAULT("ERR: Undefined CETimerInfo::eTimerState value!");
    }
}

//////////////////////////////////////////////////////////////////////////
// CEExpiredTimerInfo class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CEExpiredTimerInfo::CEExpiredTimerInfo( void )
    : mTimer    ( NULL )
    , mLowValue ( 0 )
    , mHighValue( 0 )
{   ;   }

inline CEExpiredTimerInfo::CEExpiredTimerInfo( CETimer *timer, unsigned int highValue, unsigned int lowValue )
    : mTimer    ( timer     )
    , mLowValue ( lowValue  )
    , mHighValue( highValue )
{   ;   }

inline CEExpiredTimerInfo::CEExpiredTimerInfo( const CEExpiredTimerInfo & src )
    : mTimer    ( src.mTimer    )
    , mLowValue ( src.mLowValue )
    , mHighValue( src.mHighValue)
{   ;   }

inline CEExpiredTimerInfo::~CEExpiredTimerInfo( void )
{   ;   }

inline bool CEExpiredTimerInfo::operator == ( const CEExpiredTimerInfo & other ) const
{   return (mTimer == other.mTimer);    }

#endif  // AREG_COMPONENT_PRIVATE_CETIMERINFO_HPP
