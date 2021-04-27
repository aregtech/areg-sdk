#ifndef AREG_COMPONENT_PRIVATE_IEQUEUELISTENER_HPP
#define AREG_COMPONENT_PRIVATE_IEQUEUELISTENER_HPP
/************************************************************************
 * \file        areg/component/private/IEQueueListener.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Queue Listener interface
 *              Implement class to react on signal method, which is triggered
 *              from event queue every time when new event is pushed in the
 *              queue or when the queue is empty.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEEvent;

//////////////////////////////////////////////////////////////////////////
// IEQueueListener class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event Queue Listener interface. The listener should be assigned
 *          in the Event Queue object and the SignalEvent() function is 
 *          triggered every time when new event item is pushed in the queue
 *          or when queue is empty.
 **/
class AREG_API IEQueueListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected default constructor.
     **/
    IEQueueListener( void );
    /**
     * \brief   Destructor.
     **/
    virtual ~IEQueueListener( void );

//////////////////////////////////////////////////////////////////////////
// Override operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEQueueListener interface overrides
/************************************************************************/
    /**
     * \brief	Triggered from Event Queue object every time when new event
     *          element is pushed into queue or when queue is empty.
     *          Override method to provide queuing logic.
     * \param	eventCount	The number of event elements currently in the queue.
     *                      If zero, queue is empty, dispatcher can be suspended.
     * \return  
     **/
    virtual void SignalEvent( int eventCount ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    IEQueueListener(const IEQueueListener & /*src*/ );
    const IEQueueListener & operator = (const IEQueueListener & /*src*/ );
};

#endif  // AREG_COMPONENT_PRIVATE_IEQUEUELISTENER_HPP
