#ifndef AREG_COMPONENT_PRIVATE_QUEUELISTENER_HPP
#define AREG_COMPONENT_PRIVATE_QUEUELISTENER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/QueueListener.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Queue Listener interface
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
// QueueListener class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for objects that listen to queue state changes and receive notifications when
 *          events are added or the queue becomes empty.
 **/
class AREG_API QueueListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief
     **/
    QueueListener() = default;
    virtual ~QueueListener() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// QueueListener interface overrides
/************************************************************************/
    /**
     * \brief   Triggered whenever an event is pushed into the queue or the queue becomes empty.
     *          Override to implement notification logic.
     *
     * \param   eventCount      The number of events in the queue. Zero means the queue is empty and
     *                          the dispatcher can be suspended.
     **/
    virtual void signal_event(uint32_t eventCount ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( QueueListener );
};

#endif  // AREG_COMPONENT_PRIVATE_QUEUELISTENER_HPP
