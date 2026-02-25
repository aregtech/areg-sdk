#ifndef AREG_COMPONENT_STUBEVENT_HPP
#define AREG_COMPONENT_STUBEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/StubEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Event and 
 *              Component Event Consumer class.
 *              Component Event is a base class for all kind of Request and
 *              Response events. All objects, which are interested to receive
 *              such events, should be instance of Component Event Consumer.
 *              For more details see description bellow.
 *
 *              Component Events are defined as a type of event to communicate
 *              between components and its service interfaces. 
 *              This is base class for all type of Request and Response Events.
 *              All Service Instance, which need to receive such events,
 *              should
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/RuntimeObject.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
// class StreamableEvent
    class StubEvent;
// class EventConsumer
    class StubEventConsumer;

/************************************************************************
 * Dependencies
 ************************************************************************/
class ServiceRequestEvent;
class RequestEvent;
class NotifyRequestEvent;
class StubConnectEvent;

/************************************************************************
 * \brief   This file contains declarations of following classes:
 *              1. StubEvent
 *              2. StubEventConsumer
 *          These objects are defining event object for component communication
 *          and the consumer object to parse and process event.
 *          For more information, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// StubEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for request events processed on the stub side. Cannot be used directly;
 *          marked as internal for service manager communication.
 **/
class AREG_API StubEvent  : public StreamableEvent
{
/************************************************************************/
// Friend classes
/************************************************************************/
    friend class StubEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Declare as runtime event object
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(StubEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// Protected, accessed from derived classes
/************************************************************************/

    /**
     * \brief   Initializes the event by deserializing data from the given input stream.
     *
     * \param   stream      The input stream containing serialized event data.
     **/
    StubEvent( const InStream & stream );
    
    /**
     * \brief   Initializes the event with the target stub address and event type.
     *
     * \param   toTarget        The address of the target stub.
     * \param   eventType       The type of event.
     **/
    StubEvent(const StubAddress & toTarget, Event::EventType eventType );

    /**
     * \brief   Destructor.
     **/
    virtual ~StubEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Event class overrides
/************************************************************************/
    /**
     * \brief   Sends the event to the target thread. If the target thread is null, searches for the
     *          target thread in the system.
     **/
    void deliver_event() override;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the address of the target stub.
     **/
    inline const StubAddress & target_stub() const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/

    /**
     * \brief   Deserializes the stub address from the input stream.
     *
     * \param   stream      The input stream to read from.
     * \return  The input stream for method chaining.
     **/
    const InStream & read_stream( const InStream & stream ) override;

    /**
     * \brief   Serializes the stub address to the output stream.
     *
     * \param   stream      The output stream to write to.
     * \return  The output stream for method chaining.
     **/
    OutStream & write_stream( OutStream & stream ) const override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Event target Stub address
     **/
    StubAddress   mTargetStubAddress;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     **/
    StubEvent() = delete;
    AREG_NOCOPY_NOMOVE( StubEvent );
};

//////////////////////////////////////////////////////////////////////////
// StubEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for service provider (stub) event handlers; receives and processes
 *          stub-specific events registered with the component thread.
 **/
class AREG_API StubEventConsumer  : public EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the consumer with a stub address.
     *
     * \param   stubAddress     Address of the stub object this consumer handles.
     **/
    explicit StubEventConsumer( const StubAddress & stubAddress );

    /**
     * \brief   Destructor
     **/
    virtual ~StubEventConsumer() = default;

    /**
     * \brief   Returns a pointer to the event currently being processed.
     **/
    inline const Event* current_event() const;

//////////////////////////////////////////////////////////////////////////
// Overrides, event processing functions.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Pure virtual; processes a request to invoke a service function.
     *
     * \param   eventElem       Service request event containing the request ID and serialized
     *                          parameters.
     **/
    virtual void process_request_event( ServiceRequestEvent & eventElem ) = 0;

    /**
     * \brief   Pure virtual; processes a request to get attribute data.
     *
     * \param   eventElem       Service request event containing the attribute ID.
     **/
    virtual void process_attribute_event( ServiceRequestEvent & eventElem ) = 0;

    /**
     * \brief   Pure virtual; processes a component event that is not a service request.
     *
     * \param   eventElem       Component event to process.
     **/
    virtual void process_stub_event( StubEvent & eventElem ) = 0;

    /**
     * \brief   Pure virtual; processes a generic event.
     *
     * \param   eventElem       Generic event to process.
     **/
    virtual void process_generic_event( Event & eventElem ) = 0;

    /**
     * \brief   Pure virtual; processes a notification that the stub has been registered with the
     *          service.
     *
     * \param   stubTarget      Address of the registered service provider.
     * \param   status          Connection status (Connected on success).
     **/
    virtual void process_registered_event( const StubAddress & stubTarget, NEService::ServiceConnectionState status ) = 0;

    /**
     * \brief   Pure virtual; processes a notification that a client is requesting connection or
     *          disconnection.
     *
     * \param   proxyAddress    Address of the service consumer proxy.
     * \param   status          Service consumer connection status.
     **/
    virtual void process_connect_event( const ProxyAddress & proxyAddress, NEService::ServiceConnectionState status ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// EventConsumer interface overrides
/************************************************************************/
    /**
     * \brief   Dispatches event to the appropriate handler based on event type.
     *
     * \param   eventElem       Event to process.
     **/
    void start_event_processing( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Processes a local request event.
     *
     * \param   requestEvent    Local request event to process.
     **/
    void _local_request( RequestEvent & requestEvent );

    /**
     * \brief   Processes a local notification request event.
     *
     * \param   notifyRequest       Notification request event to process.
     **/
    void _local_notify_request( NotifyRequestEvent & notifyRequest );

    /**
     * \brief   Processes a connection update notification (registration or client connection
     *          change).
     *
     * \param   notifyConnect       Connection notification event.
     **/
    void _local_connect( StubConnectEvent & notifyConnect );

private:
    //!< The address of stub object, which is handling consumer.
    const StubAddress & mStubAddress;
    //!< The pointer to the currently processing event object.
    Event *             mCurEvent;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     **/
    StubEventConsumer() = delete;
    AREG_NOCOPY_NOMOVE( StubEventConsumer );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StubEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const StubAddress & StubEvent::target_stub() const
{
    return mTargetStubAddress;
}

inline const Event* StubEventConsumer::current_event() const
{
    return mCurEvent;
}

#endif  // AREG_COMPONENT_STUBEVENT_HPP
