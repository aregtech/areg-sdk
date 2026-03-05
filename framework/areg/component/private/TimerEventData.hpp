#ifndef AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
#define AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerEventData.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer Event Data class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventTemplate.hpp"
namespace areg {

/************************************************************************
 * Declared classes
 ************************************************************************/
class TimerEventData;
class TimerEvent;

/************************************************************************
 * Dependencies
 ************************************************************************/
class Timer;

//////////////////////////////////////////////////////////////////////////
// TimerEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Container for timer event data, including the expired timer object and related
 *          information.
 **/
class AREG_API TimerEventData
{
//////////////////////////////////////////////////////////////////////////
// Friend class declaration
//////////////////////////////////////////////////////////////////////////
    friend class TimerEvent;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    inline TimerEventData();
    /**
     * \brief   Initializes the timer event data with the given timer object.
     *
     * \param   timer       The fired timer object.
     **/
    inline explicit TimerEventData( Timer & timer );
    /**
     * \brief   Copies the timer event data from the source object.
     *
     * \param   src     The source timer event data to copy.
     **/
    inline explicit TimerEventData( const TimerEventData & src );
    /**
     * \brief   Moves the timer event data from the source object.
     *
     * \param   src     The source timer event data to move. Takes ownership.
     **/
    inline TimerEventData( TimerEventData && src ) noexcept;
    inline ~TimerEventData() = default;

public:
//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Copies timer event data from the source object.
     *
     * \param   src     The source timer event data.
     * \return  A reference to this object.
     **/
    inline TimerEventData & operator = ( const TimerEventData & src );

    /**
     * \brief   Moves timer event data from the source object.
     *
     * \param   src     The source timer event data. Takes ownership.
     * \return  A reference to this object.
     **/
    inline TimerEventData & operator = ( TimerEventData && src ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns a pointer to the timer object, or null if not set.
     **/
    inline Timer * timer() const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to Timer object. Should not be nullptr.
     **/
    Timer *   mTimer;
};

AREG_DECLARE_EVENT(TimerEventData, TimerEventBase, TimerEventConsumerBase)

//////////////////////////////////////////////////////////////////////////
// TimerEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Timer event containing an expired timer, created and sent by the Timer Manager for
 *          dispatch to the timer consumer's owner thread.
 **/
class AREG_API TimerEvent : public TimerEventBase
{
//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(TimerEvent)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and sends a timer event to the specified dispatcher thread.
     *
     * \param   timer               The expired timer object.
     * \param   dispatchThreadId    The ID of the dispatcher thread to receive the event.
     * \return  Returns true if the event was sent successfully; false otherwise.
     **/
    static bool send_event( Timer & timer, id_type dispatchThreadId );
    /**
     * \brief   Creates and sends a timer event to the specified dispatcher thread.
     *
     * \param   timer               The expired timer object.
     * \param   dispatchThread      The dispatcher thread object to receive the event.
     * \return  Returns true if the event was sent successfully; false otherwise.
     * \note    Overload accepting DispatcherThread reference instead of thread ID.
     **/
    static bool send_event( Timer & timer, DispatcherThread & dispatchThread );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes the timer event with the given event data.
     *
     * \param   data    The timer event data to initialize with.
     **/
    explicit TimerEvent( const TimerEventData & data );
   /**
    * \brief   Initializes the timer event with data extracted from the given timer object.
    *
    * \param   timer       The timer object from which to extract event data.
    **/
   explicit TimerEvent( Timer & timer );
   /**
    * \brief   Initializes the timer event, sets the timer consumer, and registers it for the
    *          specified target thread. The event is ready to send after construction.
    *
    * \param   timer       The timer object to set in the event data.
    * \param   target      The target dispatcher thread to process the event.
    **/
   TimerEvent( Timer & timer, DispatcherThread & target );
    virtual ~TimerEvent();

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TimerEvent() = delete;
    AREG_NOCOPY_NOMOVE( TimerEvent );
};

//////////////////////////////////////////////////////////////////////////
// TimerEventData class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline TimerEventData::TimerEventData()
    : mTimer(nullptr)
{
}

inline TimerEventData::TimerEventData(Timer &timer)
    : mTimer(&timer)
{
}

inline TimerEventData::TimerEventData(const TimerEventData & src)
    : mTimer(src.mTimer)
{
}

inline TimerEventData::TimerEventData( TimerEventData && src ) noexcept
    : mTimer( src.mTimer )
{
    src.mTimer  = nullptr;
}

inline Timer* TimerEventData::timer() const
{
    return mTimer;
}

inline TimerEventData & TimerEventData::operator = ( const TimerEventData & src )
{
    mTimer = src.mTimer;
    return (*this);
}

inline TimerEventData & TimerEventData::operator = ( TimerEventData && src ) noexcept
{
    if ( this != &src )
    {
        mTimer = src.mTimer;
        src.mTimer = nullptr;
    }

    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
