#ifndef AREG_COMPONENT_PRIVATE_IEQUEUELISTENER_HPP
#define AREG_COMPONENT_PRIVATE_IEQUEUELISTENER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/IEQueueListener.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
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
class Event;

//////////////////////////////////////////////////////////////////////////
// IEQueueListener class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event Queue Listener interface. The listener should be assigned
 *          in the Event Queue object and the signalEvent() function is 
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
    IEQueueListener( void ) = default;
    virtual ~IEQueueListener( void ) = default;

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
    virtual void signalEvent(uint32_t eventCount ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEQueueListener );
};

#endif  // AREG_COMPONENT_PRIVATE_IEQUEUELISTENER_HPP
