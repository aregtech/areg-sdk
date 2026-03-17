#ifndef AREG_COMPONENT_EVENTCONSUMER_HPP
#define AREG_COMPONENT_EVENTCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/EventConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, the main Event Consumer Interface.
 *              All Event Consumers should be instances of this interface
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Event;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all event consumers that receive and process events from the dispatcher.
 *          Consumers must be registered with a dispatcher.
 **/
class AREG_API EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    EventConsumer() = default;
    virtual ~EventConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// EventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered by the dispatcher when starting to process an event. Overwrite to handle
     *          event processing.
     *
     * \param   eventElem       The event object being processed by the dispatcher.
     **/
    virtual void start_event_processing( Event & eventElem ) = 0;

    /**
     * \brief   Triggered when the consumer is registered with or unregistered from a dispatcher.
     *
     * \param   isRegistered    True if the consumer was registered; false if unregistered.
     **/
    virtual void consumer_registered( bool isRegistered );

    /**
     * \brief   Called before the event is processed. Return true to proceed with processing; false
     *          to drop the event.
     *
     * \param   eventElem       The event object about to be processed.
     * \return  Return true to allow event processing; false to drop the event.
     **/
    virtual bool preprocess_event( Event & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( EventConsumer );
};

} // namespace areg
#endif  // AREG_COMPONENT_EVENTCONSUMER_HPP
