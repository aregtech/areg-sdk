#ifndef AREG_COMPONENT_REMOTEEVENTCONSUMER_HPP
#define AREG_COMPONENT_REMOTEEVENTCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/RemoteEventConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote Event consumer to dispatch remote events.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/Event.hpp"
namespace areg {

/************************************************************************
 * Dependencies.
 ************************************************************************/
class ServiceRequestEvent;
class NotifyRequestEvent;
class ServiceResponseEvent;

//////////////////////////////////////////////////////////////////////////
// RemoteEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Interface for remote event consumers (stub objects) to process remote request,
 *          notification, and response events.
 **/
class AREG_API RemoteEventConsumer   : public EventConsumer
{
protected:
    RemoteEventConsumer() = default;
    virtual ~RemoteEventConsumer() = default;

public:
/************************************************************************/
// RemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Processes a remote request event received by the stub.
     *
     * \param   reqEvent    The remote request event to process.
     **/
    virtual void process_request_event(ServiceRequestEvent& reqEvent ) = 0;

    /**
     * \brief   Processes a remote notification request event received by the stub (e.g., to start
     *          or stop attribute notifications).
     *
     * \param   reqNotifyEvent      The remote notification request event to process.
     **/
    virtual void process_notify_request(NotifyRequestEvent& reqNotifyEvent ) = 0;

    /**
     * \brief   Processes a remote response event received by the stub (e.g., to subscribe or
     *          unsubscribe from information).
     *
     * \param   respEvent       The remote response event to process.
     **/
    virtual void process_response_event(ServiceResponseEvent& respEvent ) = 0;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// EventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered by the dispatcher when starting to process an event. Overwrite to handle
     *          event processing.
     *
     * \param   eventElem       The event object being processed by the dispatcher.
     **/
    void start_event_processing( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RemoteEventConsumer );
};

} // namespace areg
#endif  // AREG_COMPONENT_REMOTEEVENTCONSUMER_HPP
