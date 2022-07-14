#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGINGEVENT_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGINGEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagingEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform. Event Class to manage Timer
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"

#include <utility>

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
    typedef enum class E_TimerAction : uint8_t
    {
          TimerIgnore   //!< Ignore event, do nothing
        , TimerStart    //!< Start specified timer
        , TimerStop     //!< Stop specified timer
        , TimerCancel   //!< Cancel specified timer
        , TimerExpired  //!< Timer is expired
        , TimerRemove   //!< Removes registered timer.
    } eTimerAction;

    /**
     * \brief   TimerManagingEventData::getString()
     *          Returns the string value of TimerManagingEventData::eTimerAction type
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
     * \param   timer   The Timer object to set in Event Data. Can be nullptr if not used.
     * \param	handle	Timer system timer handle. Can be nullptr if not used.
     **/
    inline TimerManagingEventData( TimerManagingEventData::eTimerAction action, Timer * timer, TIMERHANDLE handle );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline TimerManagingEventData( const TimerManagingEventData & src );

    /**
     * \brief   Move constructor.
     * \param   src     The source of data to move.
     **/
    inline TimerManagingEventData( TimerManagingEventData && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TimerManagingEventData( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Event Data from given source.
     * \param   src     The source of Event Data to copy
     **/
    inline TimerManagingEventData & operator = ( const TimerManagingEventData & src );

    /**
     * \brief   Moves Event Data from given source.
     * \param   src     The source of Event Data to move
     **/
    inline TimerManagingEventData & operator = ( TimerManagingEventData && src ) noexcept;

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

    /**
     * \brief   Returns handle associated with the timer.
     **/
    inline TIMERHANDLE getHandle( void ) const;

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
    /**
     * \brief   The system Timer handler. 
     **/
    TIMERHANDLE     mHandle;
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
    : mAction   (TimerManagingEventData::eTimerAction::TimerIgnore)
    , mTimer    (nullptr)
    , mHandle    (nullptr)
{
}

inline TimerManagingEventData::TimerManagingEventData( TimerManagingEventData::eTimerAction action, Timer * timer, TIMERHANDLE handle )
    : mAction   (action)
    , mTimer    (timer)
    , mHandle   (handle)
{
    ASSERT(action != TimerManagingEventData::eTimerAction::TimerIgnore);
}

inline TimerManagingEventData::TimerManagingEventData( const TimerManagingEventData &src )
    : mAction   (src.mAction)
    , mTimer    (src.mTimer)
    , mHandle   (src.mHandle)
{
}

inline TimerManagingEventData::TimerManagingEventData( TimerManagingEventData && src ) noexcept
    : mAction   (std::move(src.mAction))
    , mTimer    (std::move(src.mTimer))
    , mHandle   (std::move(src.mHandle))
{
}

inline TimerManagingEventData & TimerManagingEventData::operator = ( const TimerManagingEventData & src )
{
    mAction = src.mAction;
    mTimer  = src.mTimer;
    mHandle = src.mHandle;
    
    return (*this);
}

inline TimerManagingEventData & TimerManagingEventData::operator = ( TimerManagingEventData && src ) noexcept
{
    mAction = std::move(src.mAction);
    mTimer  = std::move(src.mTimer);
    mHandle = std::move(src.mHandle);

    return (*this);
}

inline TIMERHANDLE TimerManagingEventData::getHandle( void ) const
{
    return mHandle;
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
    CASE_MAKE_STRING(TimerManagingEventData::eTimerAction::TimerIgnore);
    CASE_MAKE_STRING(TimerManagingEventData::eTimerAction::TimerStart);
    CASE_MAKE_STRING(TimerManagingEventData::eTimerAction::TimerStop);
    CASE_MAKE_STRING(TimerManagingEventData::eTimerAction::TimerCancel);
    CASE_MAKE_STRING(TimerManagingEventData::eTimerAction::TimerExpired);
    CASE_MAKE_STRING(TimerManagingEventData::eTimerAction::TimerRemove);
    CASE_DEFAULT("ERR: Undefined TimerManagingEventData::eTimerAction value!");
    }
}

#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGINGEVENT_HPP
