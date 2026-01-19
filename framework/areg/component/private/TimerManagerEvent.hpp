#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGEREVENT_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGEREVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerEvent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform. Event Class to manage Timer
 *
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"

#include "areg/component/TimerBase.hpp"


  /************************************************************************
   * Declared classes
   ************************************************************************/
class TimerManagerEventData;
// class TimerManagerEvent
// class IETimerManagerEventConsumer

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. TimerManagerEventData
 *              2. TimerManagerEvent
 *              3. IETimerManagerEventConsumer
 *          The Timer Managing Event is used to send events to Timer Manager.
 *          The Timer Manager derives IETimerManagerEventConsumer and
 *          processes Event Data.
 *          For mode details, see descriptions bellow and see
 *          class TimerManager
 *
 * \see TimerManagerBase
 ************************************************************************/

 /************************************************************************
  * Dependencies
  ************************************************************************/
class Timer;

//////////////////////////////////////////////////////////////////////////
// TimerManagerEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Timer Manager Event Data is a container object.
 *          It contains Timer object and requested action.
 *          The Data object is delivered to Timer Manager and
 *          depending on requested actions, the Timer Manager
 *          is performing action.
 *          For more details, see TimerManager class
 **/
class TimerManagerEventData
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     *          Creates empty data with ignore action.
     **/
    inline TimerManagerEventData(void);

    /**
     * \brief   Initializes data object to start timer.
     * \param   timer   The Timer object to set in Event Data. Can be nullptr if not used.
     **/
    inline explicit TimerManagerEventData(TimerBase* timer);

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline TimerManagerEventData(const TimerManagerEventData& src);

    /**
     * \brief   Destructor
     **/
    ~TimerManagerEventData(void) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Event Data from given source.
     * \param   src     The source of Event Data to copy
     **/
    inline TimerManagerEventData& operator = (const TimerManagerEventData& src);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the Timer object.
     **/
    inline TimerBase* getTimer(void) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Timer object of Event Data
     **/
    TimerBase*      mTimer;
};

//////////////////////////////////////////////////////////////////////////
// TimerManagerEvent and IETimerManagerEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declaration of TimerManagerEvent and IETimerManagerEventConsumer.
 *          The TimerManagerEvent is used to send Event Data
 *          The IETimerManagerEventConsumer is used to process data.
 **/
DECLARE_EVENT(TimerManagerEventData, TimerManagerEvent, IETimerManagerEventConsumer)

/************************************************************************
 * Inline functions
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// TimerManagerEventData class inline functions declaration
//////////////////////////////////////////////////////////////////////////
inline TimerManagerEventData::TimerManagerEventData(void)
    : mTimer    ( nullptr   )
{
}

inline TimerManagerEventData::TimerManagerEventData(TimerBase* timer)
    : mTimer    ( timer     )
{
}

inline TimerManagerEventData::TimerManagerEventData(const TimerManagerEventData& src)
    : mTimer    ( src.mTimer    )
{
}

inline TimerManagerEventData& TimerManagerEventData::operator = (const TimerManagerEventData& src)
{
    mTimer    = src.mTimer;
    return (*this);
}

inline TimerBase* TimerManagerEventData::getTimer(void) const
{
    return mTimer;
}

#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGEREVENT_HPP
