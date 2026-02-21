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
#include "areg/base/GEGlobal.h"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ProxyAddress.hpp"

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
 * \brief   Proxy Event is a base class to send Service Response events. 
 *          The response events are sent by Stub to target Proxy when 
 *          finish request to execute a certain function call. Proxy event
 *          is a runtime class and contains Proxy target address.
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
     * \brief   Creates Proxy Event object and sets target  Proxy Address
     * \param   targetProxy The address of target Proxy, which should
     *                      receive and process event.
     **/
    ProxyEvent( const ProxyAddress & targetProxy, Event::eEventType eventType );

    /**
     * \brief   Initializes event data from streaming object.
     * \param   stream      Streaming object, containing event data.
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
     * \brief   Sends the event to target thread. If target thread
     *          is nullptr, it searches target thread, registered in system.
     **/
    virtual void deliverEvent() override;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns target Proxy Address object.
     **/
    inline const ProxyAddress & getTargetProxy() const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const InStream & readStream( const InStream & stream ) override;

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual OutStream & writeStream( OutStream & stream ) const override;

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
 * \brief       Proxy Event Consumer to receive and process events.
 *              All Proxies are instances of ProxyEventConsumer interface.
 *              This interface is triggering an appropriate event processing
 *              function depending on event runtime class ID.
 * 
 **/
class AREG_API ProxyEventConsumer : public EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor. Sets the Proxy address of event consumer.
     * \param   proxy   The address of proxy object, which is handling consumer
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
     * \brief   Triggered to process Response Event sent by Stub.
     **/
    virtual void processResponseEvent( ServiceResponseEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process Attribute update event sent by Stub.
     **/
    virtual void processAttributeEvent( ServiceResponseEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process generic Proxy Event.
     **/
    virtual void processProxyEvent( ProxyEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process generic event
     **/
    virtual void processGenericEvent( Event & eventElem ) = 0;

    /**
     * \brief   Triggered, when received server connection status changed.
     * \param   Server      The address of connected service stub server.
     * \param   Channel     Communication channel object to deliver events.
     * \param   Status      The service connection status. 
     *                      The connection status should be areg::ServiceConnected
     *                      To be able to send message to service target from Proxy client.
     **/
    virtual void serviceConnectionUpdated( const StubAddress & Server, const Channel & Channel, areg::eServiceConnection Status ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// EventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered by dispatcher to process generic event.
     *          Derived from generic Event Consumer class.
     *          Depending on the Runtime type of Event object,
     *          the Event object will be passed to appropriate
     *          Event processing function
     * \param   eventElem   The generic Event object to process.
     **/
    virtual void startEventProcessing( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggered, when processing response event
     * \param   eventResponse   Response event to process
     **/
    void _localProcessResponseEvent( ResponseEvent & eventResponse);

    /**
     * \brief   Triggered, when processing connection event.
     *          The event is sent when client is connected or disconnected
     *          to service serve component.
     * \param   eventConnect    The connection event to process.
     **/
    void _localProcessConnectEvent( ProxyConnectEvent & eventConnect );

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

inline const ProxyAddress & ProxyEvent::getTargetProxy() const
{
    return mTargetProxyAddress;
}

#endif  // AREG_COMPONENT_PROXYEVENT_HPP
