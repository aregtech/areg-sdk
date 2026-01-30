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
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"

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
 * \brief   TimerEventData class is containing Timer object, which is
 *          expired and send within the Timer Event object to dispatch.
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
    /**
     * \brief   Default constructor
     **/
    inline TimerEventData( void );
    /**
     * \brief   Constructor. Initializes Timer object
     * \param   timer   The fired Timer object.
     **/
    inline explicit TimerEventData( Timer & timer );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline explicit TimerEventData( const TimerEventData & src );
    /**
     * \brief   Move constructor.
     * \param   src     The source of data to move.
     **/
    inline TimerEventData( TimerEventData && src ) noexcept;
    /**
     * \brief   Destructor
     **/
    inline ~TimerEventData( void ) = default;

public:
//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Copies data from given source
     * \param   src     The source of Timer Event Data
     **/
    inline TimerEventData & operator = ( const TimerEventData & src );

    /**
     * \brief   Moves data from given source
     * \param   src     The source of Timer Event Data
     **/
    inline TimerEventData & operator = ( TimerEventData && src ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the pointer to Timer object.
     **/
    inline Timer * getTimer( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to Timer object. Should not be nullptr.
     **/
    Timer *   mTimer;
};

DECLARE_EVENT(TimerEventData, TimerEventBase, IETimerEventConsumerBase)

//////////////////////////////////////////////////////////////////////////
// TimerEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TimerEvent class contains timer data and create by
 *          Timer Manager when the timer is expired.
 **/
class AREG_API TimerEvent : public TimerEventBase
{
//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(TimerEvent)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and sends Timer event containing information
     *          about expired timer. The event is send to the
     *          specified dispatcher thread
     * \param   timer               The timer object, which is expired.
     * \param   dispatchThreadId    The ID of dispatcher object to send event
     * \return  Returns true if successfully sent event. Otherwise returns false.
     **/
    static bool sendEvent( Timer & timer, id_type dispatchThreadId );
    /**
     * \brief   Creates and sends Timer event containing information
     *          about expired timer. The event is send to the
     *          specified dispatcher thread
     * \param   timer           The timer object, which is expired.
     * \param   dispatchThread  The dispatcher object to send event
     * \return  Returns true if successfully sent event. Otherwise returns false.
     **/
    static bool sendEvent( Timer & timer, DispatcherThread & dispatchThread );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initialize Timer Event object and data.
     * \param   data    The data, which is contained in the event object.
     **/
    explicit TimerEvent( const TimerEventData & data );
     /**
      * \brief   Initialize Timer Event object and data.
      *          The Event Data is initialized by passed Timer object.
      * \param   timer    The Timer object to include in event data.
      **/
   explicit TimerEvent( Timer & timer );
   /**
    * \brief    Initializes Timer Event object and data, sets timer consumer
    *           and registers for specified target thread. The target thread
    *           should be valid. It does not make additional checkup whether
    *           the target thread is valid or not. After calling this method,
    *           the event is ready to be sent and processed.
    * \param    timer   The Timer object to set in event data.
    * \param    target  The target dispatching thread to process event.
    **/
   TimerEvent( Timer & timer, DispatcherThread & target );
   /**
    * \brief   Destructor
    **/
    virtual ~TimerEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TimerEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TimerEvent );
};

//////////////////////////////////////////////////////////////////////////
// TimerEventData class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline TimerEventData::TimerEventData( void )
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

inline Timer* TimerEventData::getTimer( void ) const
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

#endif  // AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
