#ifndef AREG_COMPONENT_PRIVATE_CETIMEREVENTDATA_HPP
#define AREG_COMPONENT_PRIVATE_CETIMEREVENTDATA_HPP
/************************************************************************
 * \file        areg/component/private/CETimerEventData.hpp
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
class CETimerEventData;
class CETimerEvent;

/************************************************************************
 * Dependencies
 ************************************************************************/
class CETimer;

//////////////////////////////////////////////////////////////////////////
// CETimerEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CETimerEventData class is containing Timer object, which is
 *          expired and send within the Timer Event object to dispatch.
 **/
class AREG_API CETimerEventData
{
//////////////////////////////////////////////////////////////////////////
// Friend class declaration
//////////////////////////////////////////////////////////////////////////
    friend class CETimerEvent;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    inline CETimerEventData( void );
    /**
     * \brief   Constructor. Initializes Timer object
     * \param   timer   The fired Timer object.
     **/
    inline CETimerEventData( CETimer & timer );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline CETimerEventData( const CETimerEventData & src );
    /**
     * \brief   Destructor
     **/
    inline ~CETimerEventData( void );

public:
//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Assigning operator. Copies data from given source
     * \param   src     The source of Timer Event Data
     **/
    const CETimerEventData & operator = ( const CETimerEventData & src );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the pointer to Timer object.
     **/
    inline CETimer * GetTimer( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to Timer object. Should not be NULL.
     **/
    CETimer *   mTimer;
};

//////////////////////////////////////////////////////////////////////////
// CETimerEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CETimerEvent class contains timer data and create by
 *          Timer Manager when the timer is expired.
 **/
class AREG_API CETimerEvent : public TEEvent<CETimerEventData, const CETimerEventData &>
{
//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(CETimerEvent)

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
    static bool SendEvent( CETimer & timer, ITEM_ID dispatchThreadId );
    /**
     * \brief   Creates and sends Timer event containing information
     *          about expired timer. The event is send to the
     *          specified dispatcher thread
     * \param   timer           The timer object, which is expired.
     * \param   dispatchThread  The dispatcher object to send event
     * \return  Returns true if successfully sent event. Otherwise returns false.
     **/
    static bool SendEvent( CETimer & timer, CEDispatcherThread & dispatchThread );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constructor. Initialize Timer Event object and data
     * \param   data    The data, which is contained in the event object.
     **/
    CETimerEvent( const CETimerEventData & data );
     /**
      * \brief   Constructor. Initialize Timer Event object and data.
      *          The Event Data is initialized by passed Timer object.
      * \param   timer    The Timer object to include in event data.
      **/
   CETimerEvent( CETimer & timer );
   /**
    * \brief   Destructor
    **/
    virtual ~CETimerEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    CETimerEvent( void );
    CETimerEvent( const CETimerEvent & /*src*/ );
    const CETimerEvent & operator = ( const CETimerEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CETimerEventData class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CETimerEventData::CETimerEventData( void )
    : mTimer(NULL)
{   ;   }

inline CETimerEventData::CETimerEventData(CETimer &timer)
    : mTimer(&timer)
{   ;   }

inline CETimerEventData::CETimerEventData(const CETimerEventData & src)
    : mTimer(src.mTimer)
{   ;   }

inline CETimerEventData::~CETimerEventData( void )
{   ;   }

inline CETimer* CETimerEventData::GetTimer( void ) const
{   return mTimer;  }

#endif  // AREG_COMPONENT_PRIVATE_CETIMEREVENTDATA_HPP
