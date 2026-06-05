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
#include "areg/base/areg_global.h"
#include "areg/base/RuntimeObject.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
namespace areg {

/************************************************************************
 * List of declared classes
 ************************************************************************/
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
class AREG_API StubEvent  : public Event
{
/************************************************************************/
// Friend classes
/************************************************************************/
    friend class StubEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// Protected, accessed from derived classes
/************************************************************************/

    /**
     * \brief   Constructs from an existing envelope (IPC receive path). Shares the buffer.
     **/
    explicit StubEvent(const EventEnvelope& envelope) noexcept;

    /**
     * \brief   Constructs from a moved envelope (IPC receive path). Takes ownership.
     **/
    explicit StubEvent(EventEnvelope&& envelope) noexcept;

    /**
     * \brief   Initializes the event with the target stub address and event type, allocating the
     *          event buffer with the header plus \a initSize reserved payload bytes.
     *
     * \param   toTarget        The address of the target stub.
     * \param   eventType       The type of event.
     * \param   initSize        Payload bytes to reserve after the header so that serialization does
     *                          not reallocate. 0 keeps the default block size.
     **/
    StubEvent(const StubAddress & toTarget, areg::EventType eventType, uint32_t initSize = 0u );

public:

    StubEvent(const StubEvent& src) noexcept = default;

    StubEvent(StubEvent&& src) noexcept = default;

    ~StubEvent() override = default;

public:

    /**
     * \brief   Returns the address of the target stub, reconstructed from the EventHeader provider endpoint.
     **/
    inline uint32_t target_stub() const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    StubEvent() = delete;
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

    virtual ~StubEventConsumer() = default;

    /**
     * \brief   Returns a pointer to the event currently being processed.
     **/
    [[nodiscard]]
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
     * \brief   Pure virtual; processes a notification that the stub has been registered with the service.
     *
     * \param   stubTarget      Address of the registered service provider.
     * \param   status          Connection status (Connected on success).
     **/
    virtual void process_registered_event( const StubAddress & stubTarget, areg::ServiceConnectionState status ) = 0;

    /**
     * \brief   Pure virtual; processes a notification that a client is requesting connection or disconnection.
     *
     * \param   proxyAddress    Address of the service consumer proxy.
     * \param   status          Service consumer connection status.
     **/
    virtual void process_connect_event( const ProxyAddress & proxyAddress, areg::ServiceConnectionState status ) = 0;

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
    void start_event_processing( Event & eventElem ) final override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Processes a local request event.
     *
     * \param   reqEvent    Local request event to process.
     **/
    void _local_request( RequestEvent & reqEvent );

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
    StubEventConsumer() = delete;
    AREG_NOCOPY_NOMOVE( StubEventConsumer );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StubEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline uint32_t StubEvent::target_stub() const
{
    ASSERT(is_valid());
    return provider().number;
}

inline const Event* StubEventConsumer::current_event() const
{
    return mCurEvent;
}

} // namespace areg
#endif  // AREG_COMPONENT_STUBEVENT_HPP
