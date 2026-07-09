#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGEREVENT_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGEREVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/TimerManagerEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Event Class to manage Timer
 *
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventTemplate.hpp"

#include "areg/component/TimerBase.hpp"
namespace areg {


  /************************************************************************
   * Declared classes
   ************************************************************************/
class TimerManagerEventData;
// class TimerManagerEvent
// class TimerManagerEventConsumer

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. TimerManagerEventData
 *              2. TimerManagerEvent
 *              3. TimerManagerEventConsumer
 *          The Timer Managing Event is used to send events to Timer Manager.
 *          The Timer Manager derives TimerManagerEventConsumer and
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
 * \brief   Container for timer manager event data, including a timer object and the requested
 *          action (start, stop, etc.).
 **/
class TimerManagerEventData
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    inline TimerManagerEventData();

    /**
     * \brief   Initializes the timer manager event data to start the given timer.
     *
     * \param   timer       The timer object to set; may be null if not used.
     **/
    inline explicit TimerManagerEventData(TimerBase* timer);

    /**
     * \brief   Copies the timer manager event data from the source object.
     *
     * \param   src     The source timer manager event data to copy.
     **/
    inline TimerManagerEventData(const TimerManagerEventData& src);

    ~TimerManagerEventData() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies timer manager event data from the source object.
     *
     * \param   src     The source timer manager event data.
     * \return  A reference to this object.
     **/
    inline TimerManagerEventData& operator = (const TimerManagerEventData& src);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns a pointer to the timer object, or null if not set.
     **/
    inline TimerBase* timer() const;

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
// TimerManagerEvent and TimerManagerEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declaration of TimerManagerEvent and TimerManagerEventConsumer.
 *          The TimerManagerEvent is used to send Event Data
 *          The TimerManagerEventConsumer is used to process data.
 **/
AREG_DECLARE_EVENT(TimerManagerEventData, TimerManagerEvent, TimerManagerEventConsumer)

/************************************************************************
 * Inline functions
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// TimerManagerEventData class inline functions declaration
//////////////////////////////////////////////////////////////////////////
inline TimerManagerEventData::TimerManagerEventData()
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

inline TimerBase* TimerManagerEventData::timer() const
{
    return mTimer;
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGEREVENT_HPP
