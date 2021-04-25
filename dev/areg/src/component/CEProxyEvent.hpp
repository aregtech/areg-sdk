#ifndef AREG_COMPONENT_CEPROXYEVENT_HPP
#define AREG_COMPONENT_CEPROXYEVENT_HPP
/************************************************************************
 * \file        areg/src/component/CEProxyEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Event and Proxy Event
 *              Consumer classes.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEStreamableEvent.hpp"
#include "areg/src/component/CEProxyAddress.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
class CEProxyEvent;
class IEProxyEventConsumer;

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEEvent;
class CEProxyEvent;
class CEStubAddress;
class CEServiceResponseEvent;
class IENotificationEventConsumer;
class CEChannel;
class CEResponseEvent;
class CEProxyConnectEvent;

/**
 * \remark  The following classes are declared in this file:
 *              1. CEProxyEvent
 *              2. IEProxyEventConsumer
 *          The Proxy Event used to send Event from Stub to Proxy object.
 *          The Proxy Event Consumer is an object, which is processing
 *          such event. All Proxy objects are instances of Proxy Event Consumer.
 **/

//////////////////////////////////////////////////////////////////////////
// CEProxyEvent class declaration
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
class AREG_API CEProxyEvent  : public CEStreamableEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare as a Runtime event class
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEProxyEvent)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates Proxy Event object and sets target  Proxy Address
     * \param   targetProxy The address of target Proxy, which should
     *                      receive and process event.
     **/
    CEProxyEvent( const CEProxyAddress & targetProxy, CEEvent::eEventType eventType );

    /**
     * \brief   Initializes event data from streaming object.
     * \param   stream      Streaming object, containing event data.
     **/
    CEProxyEvent( const IEInStream & stream );

    /**
     * \brief   Destructor. Protected.
     * \remark  Call Destroy() method to delete object.
     **/
    virtual ~CEProxyEvent( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
public:
/************************************************************************/
// CEEvent class overrides
/************************************************************************/
    /**
     * \brief   Sends the event to target thread. If target thread
     *          is NULL, it searches target thread, registered in system.
     **/
    virtual void SendEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns target Proxy Address object.
     **/
    inline const CEProxyAddress & GetTargetProxy( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// CEStreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & ReadFromStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & WriteToStream( IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The address of target Proxy, which should receive event.
     **/
    CEProxyAddress  mTargetProxyAddress;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEProxyEvent( void );
    CEProxyEvent( const CEProxyEvent & /*src*/ );
    const CEProxyEvent & operator = ( const CEProxyEvent & /*src*/ );
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
class AREG_API IEProxyEventConsumer : public CEThreadEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor. Sets the Proxy address of event consumer.
     * \param   proxy   The address of proxy object, which is handling consumer
     **/
    IEProxyEventConsumer( const CEProxyAddress & proxy );

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
    virtual void ProcessResponseEvent( CEServiceResponseEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process Attribute update event sent by Stub.
     **/
    virtual void ProcessAttributeEvent( CEServiceResponseEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process generic Proxy Event.
     **/
    virtual void ProcessProxyEvent( CEProxyEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process generic event
     **/
    virtual void ProcessGenericEvent( CEEvent & eventElem ) = 0;

    /**
     * \brief   Triggered, when received server connection status changed.
     * \param   Server      The address of connected service stub server.
     * \param   Channel     Communication channel object to deliver events.
     * \param   Status      The service connection status. 
     *                      The connection status should be NEService::ServiceConnected
     *                      To be able to send message to service target from Proxy client.
     **/
    virtual void ServiceConnectionUpdated( const CEStubAddress & Server, const CEChannel & Channel, NEService::eServiceConnection Status ) = 0;

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
    virtual void StartEventProcessing( CEEvent & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggered, when processing response event
     * \param   eventResponse   Response event to process
     **/
    void processResponseEvent( CEResponseEvent & eventResponse);

    /**
     * \brief   Triggered, when processing connection event.
     *          The event is sent when client is connected or disconnected
     *          to service serve component.
     * \param   eventConnect    The connection event to process.
     **/
    void processConnectEvent( CEProxyConnectEvent & eventConnect );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of proxy object, which is handling consumer
     **/
    const CEProxyAddress &  mProxyAddress;

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
// CEProxyEvent class inline functions implementations
//////////////////////////////////////////////////////////////////////////

inline const CEProxyAddress & CEProxyEvent::GetTargetProxy( void ) const
{   return mTargetProxyAddress; }

#endif  // AREG_COMPONENT_CEPROXYEVENT_HPP
