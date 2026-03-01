#ifndef AREG_COMPONENT_PROXYEVENT_HPP
#define AREG_COMPONENT_PROXYEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ProxyEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Event and Proxy Event
 *              Consumer classes.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
namespace areg {

/************************************************************************
 * List of declared classes
 ************************************************************************/
class ProxyEvent;
class ProxyEventConsumer;

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;
class ProxyEvent;
class StubAddress;
class ServiceResponseEvent;
class NotificationConsumer;
class Channel;
class ResponseEvent;
class ProxyConnectEvent;

/**
 * \remark  The following classes are declared in this file:
 *              1. ProxyEvent
 *              2. ProxyEventConsumer
 *          The Proxy Event used to send Event from Stub to Proxy object.
 *          The Proxy Event Consumer is an object, which is processing
 *          such event. All Proxy objects are instances of Proxy Event Consumer.
 **/

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for service response events sent by a stub to the target proxy upon
 *          completing a request.
 **/
class AREG_API ProxyEvent  : public StreamableEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare as a Runtime event class
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(ProxyEvent)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the proxy event and sets the target proxy address.
     *
     * \param   targetProxy     The address of the target proxy to receive the event.
     * \param   eventType       The type of event.
     **/
    ProxyEvent( const ProxyAddress & targetProxy, Event::EventType eventType );

    /**
     * \brief   Initializes the event by deserializing data from the given input stream.
     *
     * \param   stream      The input stream containing serialized event data.
     **/
    ProxyEvent( const InStream & stream );

    /**
     * \brief   Destructor. Protected.
     * \remark  Call Destroy() method to delete object.
     **/
    virtual ~ProxyEvent() = default;

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
     * \brief   Returns the target proxy address.
     **/
    inline const ProxyAddress & target_proxy() const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Deserializes event data from the given input stream.
     *
     * \param   stream      The input stream to read from.
     * \return  The input stream for method chaining.
     **/
    const InStream & read_stream( const InStream & stream ) override;

    /**
     * \brief   Serializes event data to the given output stream.
     *
     * \param   stream      The output stream to write to.
     * \return  The output stream for method chaining.
     **/
    OutStream & write_stream( OutStream & stream ) const override;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The address of target Proxy, which should receive event.
     **/
    ProxyAddress  mTargetProxyAddress;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ProxyEvent() = delete;
    AREG_NOCOPY_NOMOVE( ProxyEvent );
};

//////////////////////////////////////////////////////////////////////////
// ProxyEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for proxy objects to receive and process events. Routes event processing based
 *          on runtime class type.
 **/
class AREG_API ProxyEventConsumer : public EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Sets the Proxy address of event consumer.
     *
     * \param   proxy       The address of proxy object, which is handling consumer
     **/
    explicit ProxyEventConsumer( const ProxyAddress & proxy );

    /**
     * \brief   Destructor.
     **/
    virtual ~ProxyEventConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides. Should be implemented
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ProxyEventConsumer overrides
/************************************************************************/
    
    /**
     * \brief   Processes Response Event sent by Stub.
     *
     * \param   eventElem       The response event to process.
     **/
    virtual void process_response_event( ServiceResponseEvent & eventElem ) = 0;

    /**
     * \brief   Processes Attribute update event sent by Stub.
     *
     * \param   eventElem       The attribute update event to process.
     **/
    virtual void process_attribute_event( ServiceResponseEvent & eventElem ) = 0;

    /**
     * \brief   Processes generic Proxy Event.
     *
     * \param   eventElem       The proxy event to process.
     **/
    virtual void process_proxy_event( ProxyEvent & eventElem ) = 0;

    /**
     * \brief   Processes generic event.
     *
     * \param   eventElem       The generic event to process.
     **/
    virtual void process_generic_event( Event & eventElem ) = 0;

    /**
     * \brief   Handles server connection status change.
     *
     * \param   Server      The address of connected service stub server.
     * \param   Channel     Communication channel object to deliver events.
     * \param   Status      The service connection status. The connection status should be
     *                      areg::Connected To be able to send message to service target from
     *                      Proxy client.
     **/
    virtual void service_connection_updated( const StubAddress & Server, const Channel & Channel, areg::ServiceConnectionState Status ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// EventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Processes generic event. Depending on the runtime type of Event object, the event
     *          will be passed to appropriate processing function.
     *
     * \param   eventElem       The generic Event object to process.
     **/
    void start_event_processing( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Processes response event.
     *
     * \param   eventResponse       Response event to process
     **/
    void _local_response( ResponseEvent & eventResponse);

    /**
     * \brief   Processes connection event. Triggered when client connects or disconnects from
     *          service.
     *
     * \param   eventConnect    The connection event to process.
     **/
    void _local_connect( ProxyConnectEvent & eventConnect );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of proxy object, which is handling consumer
     **/
    const ProxyAddress &  mProxyAddress;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ProxyEventConsumer() = delete;
    AREG_NOCOPY_NOMOVE( ProxyEventConsumer );
};

/************************************************************************
 * Inline function implementations
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// ProxyEvent class inline functions implementations
//////////////////////////////////////////////////////////////////////////

inline const ProxyAddress & ProxyEvent::target_proxy() const
{
    return mTargetProxyAddress;
}

} // namespace areg
#endif  // AREG_COMPONENT_PROXYEVENT_HPP
