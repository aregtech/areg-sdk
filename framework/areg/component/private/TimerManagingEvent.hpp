#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGINGEVENT_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGINGEVENT_HPP
/************************************************************************
 * \file        areg/component/private/TimerManagingEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Event Class to manage Timer
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class TimerManagingEventData;
// class TimerManagingEvent
// class IETimerManagingEventConsumer

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. TimerManagingEventData
 *              2. TimerManagingEvent
 *              3. IETimerManagingEventConsumer
 *          The Timer Managing Event is used to send events to Timer Manager.
 *          The Timer Manager derives IETimerManagingEventConsumer and
 *          processes Event Data.
 *          For mode details, see descriptions bellow and see 
 *          class TimerManager
 *
 * \see TimerManager
 ************************************************************************/

/************************************************************************
 * Dependencies
 ************************************************************************/
class Timer;

//////////////////////////////////////////////////////////////////////////
// TimerManagingEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Timer Managing Event Data is a container object.
 *          It contains Timer object and requested action.
 *          The Data object is delivered to Timer Manager and
 *          depending on requested actions, the Timer Manager
 *          is performing action.
 *          For more details, see TimerManager class
 **/
class TimerManagingEventData
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   TimerManagingEventData::eTimerAction
     *          The actions to perform on Timer Manager side
     **/
    typedef enum E_TimerAction
    {
          TimerIgnore       //!< Ignore event, do nothing
        , TimerStart        //!< Start specified timer
        , TimerStop         //!< Stop specified timer
        , TimerCancel       //!< Cancel specified timer
        , TimerExpired      //!< Timer is expired

    } eTimerAction;

    /**
     * \brief   TimerManagingEventData::getString()
     *          Retunrs the string value of TimerManagingEventData::eTimerAction type
     **/
    static inline const char * getString( TimerManagingEventData::eTimerAction timerAction );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     *          Creates empty data with ignore action.
     **/
    inline TimerManagingEventData( void );

    /**
     * \brief   Initialization constructor.
     *          Creates data object, sets timer object and defines action.
     *          The action should not be TimerManagingEventData::TimerIgnore
     * \param   action  One of defined actions to perform. It should not be TimerManagingEventData::TimerIgnore
     * \param   timer   The Timer object to set in Event Data.
     **/
    inline TimerManagingEventData( TimerManagingEventData::eTimerAction action, Timer * timer );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline TimerManagingEventData( const TimerManagingEventData & src );

    /**
     * \brief   Destructor
     **/
    inline ~TimerManagingEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies Event Data from given source.
     * \param   src     The source of Event Data to copy
     **/
    const TimerManagingEventData & operator = ( const TimerManagingEventData & src );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the action value to perform.
     **/
    inline TimerManagingEventData::eTimerAction getAction( void ) const;

    /**
     * \brief   Returns the Timer object.
     **/
    inline Timer * getTimer( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Action value to perform.
     **/
    eTimerAction    mAction;
    /**
     * \brief   The Timer object of Event Data
     **/
    Timer *         mTimer;
};

//////////////////////////////////////////////////////////////////////////
// TimerManagingEvent and IETimerManagingEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declaration of TimerManagingEvent and IETimerManagingEventConsumer.
 *          The TimerManagingEvent is used to send Event Data
 *          The IETimerManagingEventConsumer is used to process data.
 **/
DECLARE_EVENT(TimerManagingEventData, TimerManagingEvent, IETimerManagingEventConsumer)

/************************************************************************
 * Inline functions
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// TimerManagingEventData class inline functions declaration
//////////////////////////////////////////////////////////////////////////
inline TimerManagingEventData::TimerManagingEventData( void )
    : mAction   (TimerManagingEventData::TimerIgnore)
    , mTimer    (NULL)
{
}

inline TimerManagingEventData::TimerManagingEventData( TimerManagingEventData::eTimerAction action, Timer * timer )
    : mAction   (action)
    , mTimer    (timer)
{
    ASSERT(action != TimerManagingEventData::TimerIgnore);
}

inline TimerManagingEventData::TimerManagingEventData( const TimerManagingEventData &src )
    : mAction   (src.mAction)
    , mTimer    (src.mTimer)
{
}

inline TimerManagingEventData::~TimerManagingEventData( void )
{
}

inline Timer* TimerManagingEventData::getTimer( void ) const
{
    return mTimer;
}

inline TimerManagingEventData::eTimerAction TimerManagingEventData::getAction( void ) const
{
    return mAction;
}

inline const char * TimerManagingEventData::getString( TimerManagingEventData::eTimerAction timerAction )
{
    switch ( timerAction )
    {
    CASE_MAKE_STRING(TimerManagingEventData::TimerIgnore);
    CASE_MAKE_STRING(TimerManagingEventData::TimerStart);
    CASE_MAKE_STRING(TimerManagingEventData::TimerStop);
    CASE_MAKE_STRING(TimerManagingEventData::TimerCancel);
    CASE_MAKE_STRING(TimerManagingEventData::TimerExpired);
    CASE_DEFAULT("ERR: Undefined TimerManagingEventData::eTimerAction value!");
    }
}

#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGINGEVENT_HPP
