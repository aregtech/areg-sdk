#ifndef AREG_COMPONENT_PROXYEVENT_HPP
#define AREG_COMPONENT_PROXYEVENT_HPP
/************************************************************************
 * \file        areg/component/ProxyEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Event and Proxy Event
 *              Consumer classes.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ProxyAddress.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
class ProxyEvent;
class IEProxyEventConsumer;

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;
class ProxyEvent;
class StubAddress;
class ServiceResponseEvent;
class IENotificationEventConsumer;
class Channel;
class ResponseEvent;
class ProxyConnectEvent;

/**
 * \remark  The following classes are declared in this file:
 *              1. ProxyEvent
 *              2. IEProxyEventConsumer
 *          The Proxy Event used to send Event from Stub to Proxy object.
 *          The Proxy Event Consumer is an object, which is processing
 *          such event. All Proxy objects are instances of Proxy Event Consumer.
 **/

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Proxy Event is a base class to send Service Response
 *              events. The response events are sent by Stub to target 
 *              Proxy, which was requesting to execute a function call.
 *              Proxy event is a runtime class derived from streamable
 *              event and contains Proxy target address.
 * 
 * \details     All Proxy object are receiving Proxy Event instances
 *              to process. These instances are specified as a main
 *              communication between Stub and Proxy and considered
 *              as a Service Internal type events. The events will be
 *              Placed in External event queue of Dispatcher and will
 *              be processed one by one.
 *
 **/
class AREG_API ProxyEvent  : public StreamableEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare as a Runtime event class
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(ProxyEvent)

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
    ProxyEvent( const IEInStream & stream );

    /**
     * \brief   Destructor. Protected.
     * \remark  Call Destroy() method to delete object.
     **/
    virtual ~ProxyEvent( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Event class overrides
/************************************************************************/
    /**
     * \brief   Sends the event to target thread. If target thread
     *          is NULL, it searches target thread, registered in system.
     **/
    virtual void deliverEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns target Proxy Address object.
     **/
    inline const ProxyAddress & getTargetProxy( void ) const;

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
    virtual const IEInStream & readStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const;

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
    ProxyEvent( void );
    ProxyEvent( const ProxyEvent & /*src*/ );
    const ProxyEvent & operator = ( const ProxyEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// IEProxyEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Proxy Event Consumer to receive and process events.
 *              All Proxies are instances of IEProxyEventConsumer interface.
 *              This interface is triggering an appropriate event processing
 *              function depending on event runtime class ID.
 * 
 **/
class AREG_API IEProxyEventConsumer : public ThreadEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor. Sets the Proxy address of event consumer.
     * \param   proxy   The address of proxy object, which is handling consumer
     **/
    IEProxyEventConsumer( const ProxyAddress & proxy );

    /**
     * \brief   Destructor.
     **/
    virtual ~IEProxyEventConsumer( void );

//////////////////////////////////////////////////////////////////////////
// Overrides. Should be implemented
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyEventConsumer overrides
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
     *                      The connection status should be NEService::ServiceConnected
     *                      To be able to send message to service target from Proxy client.
     **/
    virtual void serviceConnectionUpdated( const StubAddress & Server, const Channel & Channel, NEService::eServiceConnection Status ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IEEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered by dispatcher to process generic event.
     *          Derived from generic Event Consumer class.
     *          Depending on the Runtime type of Event object,
     *          the Event object will be passed to appropriate
     *          Event processing function
     * \param   eventElem   The generic Event object to process.
     **/
    virtual void startEventProcessing( Event & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggered, when processing response event
     * \param   eventResponse   Response event to process
     **/
    void localProcessResponseEvent( ResponseEvent & eventResponse);

    /**
     * \brief   Triggered, when processing connection event.
     *          The event is sent when client is connected or disconnected
     *          to service serve component.
     * \param   eventConnect    The connection event to process.
     **/
    void localProcessConnectEvent( ProxyConnectEvent & eventConnect );

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
    IEProxyEventConsumer( void );
    IEProxyEventConsumer( const IEProxyEventConsumer & /*src*/ );
    const IEProxyEventConsumer & operator = ( const IEProxyEventConsumer & /*src*/ );
};

/************************************************************************
 * Inline function implementations
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// ProxyEvent class inline functions implementations
//////////////////////////////////////////////////////////////////////////

inline const ProxyAddress & ProxyEvent::getTargetProxy( void ) const
{
    return mTargetProxyAddress;
}

#endif  // AREG_COMPONENT_PROXYEVENT_HPP
