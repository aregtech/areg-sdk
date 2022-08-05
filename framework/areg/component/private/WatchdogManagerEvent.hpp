#ifndef AREG_COMPONENT_PRIVATE_WATCHDOGMANAGEREVENT_HPP
#define AREG_COMPONENT_PRIVATE_WATCHDOGMANAGEREVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManagerEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform. Event Class for watchdog manager
 *
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/component/private/Watchdog.hpp"

#include <utility>

  /************************************************************************
   * Declared classes
   ************************************************************************/
class WatchdogManagerEventData;
// class WatchdogManagerEvent
// class IEWatchdogManagerEventConsumer

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. WatchdogManagerEventData
 *              2. WatchdogManagerEvent
 *              3. IEWatchdogManagerEventConsumer
 *          The Timer Managing Event is used to send events to Timer Manager.
 *          The Timer Manager derives IEWatchdogManagerEventConsumer and
 *          processes Event Data.
 *          For mode details, see descriptions bellow and see
 *          class TimerManager
 *
 * \see TimerManager
 ************************************************************************/

 /************************************************************************
  * Dependencies
  ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// WatchdogManagerEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Timer Managing Event Data is a container object.
 *          It contains Timer object and requested action.
 *          The Data object is delivered to Timer Manager and
 *          depending on requested actions, the Timer Manager
 *          is performing action.
 *          For more details, see TimerManager class
 **/
class WatchdogManagerEventData
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   WatchdogManagerEventData::eTimerAction
     *          The actions to perform on Timer Manager side
     **/
    typedef enum class E_TimerAction : uint8_t
    {
          TimerIgnore   //!< Ignore event, do nothing
        , TimerStart    //!< Start specified timer
        , TimerStop     //!< Stop specified timer
        , TimerCancel   //!< Cancel specified timer
        , TimerExpired  //!< Timer is expired
    } eTimerAction;

    /**
     * \brief   WatchdogManagerEventData::getString()
     *          Returns the string value of WatchdogManagerEventData::eTimerAction type
     **/
    static inline const char* getString(WatchdogManagerEventData::eTimerAction timerAction);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor.
     **/
    inline WatchdogManagerEventData( void );

    /**
     * \brief   Initialization constructor.
     *          Creates data object, sets watchdog object and defines action.
     *          The action should not be WatchdogManagerEventData::TimerIgnore
     * \param   action  One of defined actions to perform. It should not be WatchdogManagerEventData::TimerIgnore
     * \param   timer   The Timer object to set in Event Data. Can be nullptr if not used.
     **/
    inline WatchdogManagerEventData( WatchdogManagerEventData::eTimerAction action
                                   , Watchdog &watchdog
                                   , Watchdog::WATCHDOG_ID watchdogId = Watchdog::INVALID_WATCHDOG);

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline WatchdogManagerEventData(const WatchdogManagerEventData& src);

    /**
     * \brief   Destructor
     **/
    ~WatchdogManagerEventData(void) = default;

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Event Data from given source.
     * \param   src     The source of Event Data to copy
     **/
    inline WatchdogManagerEventData& operator = (const WatchdogManagerEventData& src);

    /**
     * \brief   Returns the action value to perform.
     **/
    inline WatchdogManagerEventData::eTimerAction getAction(void) const;

    /**
     * \brief   Returns the Timer object.
     **/
    inline Watchdog* getWatchdog(void) const;

    /**
     * \brief   Returns the watchdog ID value saved in the even data.
     **/
    inline Watchdog::WATCHDOG_ID getWatchdogId( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Action value to perform.
     **/
    eTimerAction            mAction;
    /**
     * \brief   The watchdog object of Event Data
     **/
    Watchdog *              mWatchdog;
    /**
     * \brief   The watchdog ID of the watchdog object.
     **/
    Watchdog::WATCHDOG_ID   mWatchdogId;
};

//////////////////////////////////////////////////////////////////////////
// WatchdogManagerEvent and IEWatchdogManagerEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declaration of WatchdogManagerEvent and IEWatchdogManagerEventConsumer.
 *          The WatchdogManagerEvent is used to send Event Data
 *          The IEWatchdogManagerEventConsumer is used to process data.
 **/
DECLARE_EVENT(WatchdogManagerEventData, WatchdogManagerEvent, IEWatchdogManagerEventConsumer)

/************************************************************************
 * Inline functions
 ************************************************************************/
 //////////////////////////////////////////////////////////////////////////
 // WatchdogManagerEventData class inline functions declaration
 //////////////////////////////////////////////////////////////////////////
inline WatchdogManagerEventData::WatchdogManagerEventData(void)
    : mAction       (WatchdogManagerEventData::eTimerAction::TimerIgnore)
    , mWatchdog     (nullptr)
    , mWatchdogId   ( 0 )
{
}

inline WatchdogManagerEventData::WatchdogManagerEventData( WatchdogManagerEventData::eTimerAction action
                                                         , Watchdog & watchdog
                                                         , Watchdog::WATCHDOG_ID watchdogId /*= Watchdog::INVALID_WATCHDOG*/)
    : mAction       (action)
    , mWatchdog     (&watchdog)
    , mWatchdogId   (watchdogId)
{
    ASSERT(action != WatchdogManagerEventData::eTimerAction::TimerIgnore);
}

inline WatchdogManagerEventData::WatchdogManagerEventData(const WatchdogManagerEventData& src)
    : mAction       (src.mAction)
    , mWatchdog     (src.mWatchdog)
    , mWatchdogId   (src.mWatchdogId)
{
}

inline WatchdogManagerEventData& WatchdogManagerEventData::operator = (const WatchdogManagerEventData& src)
{
    mAction     = src.mAction;
    mWatchdog   = src.mWatchdog;
    mWatchdogId = src.mWatchdogId;

    return (*this);
}

inline Watchdog* WatchdogManagerEventData::getWatchdog(void) const
{
    return mWatchdog;
}

inline Watchdog::WATCHDOG_ID WatchdogManagerEventData::getWatchdogId(void) const
{
    return mWatchdogId;
}

inline WatchdogManagerEventData::eTimerAction WatchdogManagerEventData::getAction(void) const
{
    return mAction;
}

inline const char* WatchdogManagerEventData::getString(WatchdogManagerEventData::eTimerAction timerAction)
{
    switch (timerAction)
    {
        CASE_MAKE_STRING(WatchdogManagerEventData::eTimerAction::TimerIgnore);
        CASE_MAKE_STRING(WatchdogManagerEventData::eTimerAction::TimerStart);
        CASE_MAKE_STRING(WatchdogManagerEventData::eTimerAction::TimerStop);
        CASE_MAKE_STRING(WatchdogManagerEventData::eTimerAction::TimerCancel);
        CASE_MAKE_STRING(WatchdogManagerEventData::eTimerAction::TimerExpired);
        CASE_DEFAULT("ERR: Undefined WatchdogManagerEventData::eTimerAction value!");
    }
}

#endif  // AREG_COMPONENT_PRIVATE_WATCHDOGMANAGEREVENT_HPP
