#ifndef AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
#define AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
/************************************************************************
 * \file        areg/component/private/TimerEventData.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer Event Data class.
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
    inline TimerEventData( Timer & timer );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline TimerEventData( const TimerEventData & src );
    /**
     * \brief   Destructor
     **/
    inline ~TimerEventData( void );

public:
//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Assigning operator. Copies data from given source
     * \param   src     The source of Timer Event Data
     **/
    const TimerEventData & operator = ( const TimerEventData & src );

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
     * \brief   Pointer to Timer object. Should not be NULL.
     **/
    Timer *   mTimer;
};

//////////////////////////////////////////////////////////////////////////
// TimerEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TimerEvent class contains timer data and create by
 *          Timer Manager when the timer is expired.
 **/
class AREG_API TimerEvent : public TEEvent<TimerEventData, const TimerEventData &>
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
    static bool sendEvent( Timer & timer, ITEM_ID dispatchThreadId );
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
     * \brief   Constructor. Initialize Timer Event object and data
     * \param   data    The data, which is contained in the event object.
     **/
    TimerEvent( const TimerEventData & data );
     /**
      * \brief   Constructor. Initialize Timer Event object and data.
      *          The Event Data is initialized by passed Timer object.
      * \param   timer    The Timer object to include in event data.
      **/
   TimerEvent( Timer & timer );
   /**
    * \brief   Destructor
    **/
    virtual ~TimerEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TimerEvent( void );
    TimerEvent( const TimerEvent & /*src*/ );
    const TimerEvent & operator = ( const TimerEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// TimerEventData class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline TimerEventData::TimerEventData( void )
    : mTimer(NULL)
{   ;   }

inline TimerEventData::TimerEventData(Timer &timer)
    : mTimer(&timer)
{   ;   }

inline TimerEventData::TimerEventData(const TimerEventData & src)
    : mTimer(src.mTimer)
{   ;   }

inline TimerEventData::~TimerEventData( void )
{   ;   }

inline Timer* TimerEventData::getTimer( void ) const
{
    return mTimer;
}

#endif  // AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
