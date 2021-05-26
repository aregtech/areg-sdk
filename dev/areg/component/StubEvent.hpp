#ifndef AREG_COMPONENT_STUBEVENT_HPP
#define AREG_COMPONENT_STUBEVENT_HPP
/************************************************************************
 * \file        areg/component/StubEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Event and 
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
#include "areg/component/StreamableEvent.hpp"

#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
// class StreamableEvent
    class StubEvent;
// class ThreadEventConsumerBase
    class IEStubEventConsumer;

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
 *              2. IEStubEventConsumer
 *          These objects are defining event object for component communication
 *          and the consumer object to parse and process event.
 *          For more information, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// StubEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Component Event class is a base class for all kind of
 *              Request events and events are processed on Stub / Server
 *              side.
 * 
 * \details     Component Events cannot be used for internal event.
 *              By default it is marked as Service Internal, which means
 *              that the communication is possible between 
 *              Service Interfaces internal within single application.
 *              No other communication mechanism currently supported.
 *
 **/
class AREG_API StubEvent  : public StreamableEvent
{
/************************************************************************/
// Friend classes
/************************************************************************/
    friend class IEStubEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Declare as runtime event object
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(StubEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// Protected, accessed from derived classes
/************************************************************************/

    /**
     * \brief   Initialize component event from streaming object.
     **/
    StubEvent( const IEInStream & stream );
    
    /**
     * \brief   Initializes target Stub addresses and sets event type.
     * \param	toTarget    The address of target Stub
     * \param	eventType   The type of event.
     **/
    StubEvent(const StubAddress & toTarget, Event::eEventType eventType );

    /**
     * \brief   Destructor.
     **/
    virtual ~StubEvent( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Event class overrides
/************************************************************************/
    /**
     * \brief   Sends the event to target thread. If target thread
     *          is NULL, it searches event target thread, registered in system.
     **/
    virtual void sendEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the address of Stub of event target.
     **/
    inline const StubAddress & getTargetStub( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/

    /**
     * \brief   Initialize component address from reading stream
     * \param   stream  The reading stream to read out data
     **/
    virtual const IEInStream & readStream( const IEInStream & stream );

    /**
     * \brief   Write component address to stream.
     * \param   stream  The writing stream to write in data
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const;

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
    StubEvent( void );
    StubEvent( const StubEvent & /*src*/ );
    const StubEvent & operator = ( const StubEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Stub / Server objects, are instances of IEStubEventConsumer
 *              to receive and process component events.
 * 
 * \details     When the dispatcher is dispatching events, it will look for
 *              registered consumer (in this case Stub / Server). All Server
 *              objects are automatically assigned for events, which runtime
 *              class ID is Component Event type. The dispatcher will
 *              forward component event to registered Server to start
 *              event processing. Depending on event message ID, either
 *              request, or attribute, or generic event processing
 *              function will be triggered. All Stub / Server objects
 *              should override processing functions to dispatch message
 *              ID and start processing request. 
 *
 **/
class AREG_API IEStubEventConsumer  : public ThreadEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     * \param   stubAddress The address of stub object, which is handling consumer
     **/
    IEStubEventConsumer( const StubAddress & stubAddress );

    /**
     * \brief   Destructor
     **/
    virtual ~IEStubEventConsumer( void );

//////////////////////////////////////////////////////////////////////////
// Overrides, event processing functions.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	This event processing function is triggered when Stub 
     *          is receiving request to start a function.
     * \param	eventElem	Service Request event object to process,
     *                      which contains request ID and serialized
     *                      parameters.
     **/
    virtual void processRequestEvent( ServiceRequestEvent & eventElem ) = 0;

    /**
     * \brief	This event processing function is triggered when Stub
     *          is receiving request to send attribute data.
     * \param	eventElem	Service Request event object to process,
     *                      which contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem ) = 0;

    /**
     * \brief	This function is triggered when component event should
     *          be processed and this component event is not a 
     *          Service Request type.
     * \param	eventElem	Component Event to process.
     **/
    virtual void processStubEvent( StubEvent & eventElem ) = 0;

    /**
     * \brief	This function is triggered when generic event 
     *          should be processed
     * \param	eventElem	Generic Event object to process.
     **/
    virtual void processGenericEvent( Event & eventElem ) = 0;

    /**
     * \brief   Send by system when client is requested connect / disconnect
     * \param   proxyAddress        The address of source proxy
     * \param   connectionStatus    Connection status of specified client
     **/
    virtual void processClientConnectEvent( const ProxyAddress & proxyAddress, NEService::eServiceConnection connectionStatus ) = 0;

    /**
     * \brief   Triggered by system when stub is registered in service. The connection status indicated
     *          registration status. If succeeded, the value is NEService::ServiceConnected
     * \param   stubTarget          The address of registered Stub
     * \param   connectionStatus    Stub registration status.
     **/
    virtual void processStubRegisteredEvent( const StubAddress & stubTarget, NEService::eServiceConnection connectionStatus ) = 0;

    /**
     * \brief   This function is triggered when object is
     *          adding listener and triggered by dispatcher
     *          to indicate whether consumer registered or
     *          not.
     * \param isRegistered  On adding listener, this parameter
     *                      should be true. On removing -- false.
     *                      Otherwise there was an error adding listener.
     **/
    virtual void consumerRegistered( bool isRegistered );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IEEventConsumer interface overrides
/************************************************************************/
    /**
     * \brief   Override derived from Event Consumer class.
     *          the type of event and appropriate processing function
     *          call is processed here. This method is triggered by
     *          dispatcher.
     * \param   eventElem   Event object to start processing.
     **/
    virtual void startEventProcessing( Event & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Processes request event
     * \param   requestEvent    The request event to process
     **/
    void localProcessRequestEvent( RequestEvent & requestEvent );

    /**
     * \brief   Processes notification request event.
     * \param   notifyRequest   The notification request event to process
     **/
    void localProcessNotifyRequestEvent( NotifyRequestEvent & notifyRequest );

    /**
     * \brief   Processes connection update notification.
     *          Processes when service registered and when client connection changed.
     * \param   notifyConnect   The connection notification event.
     **/
    void localProcessConnectEvent( StubConnectEvent & notifyConnect );

private:
    /**
     * \brief   The address of stub object, which is handling consumer
     **/
    const StubAddress & mStubAddress;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IEStubEventConsumer( void );
    IEStubEventConsumer( const IEStubEventConsumer & /*src*/ );
    const IEStubEventConsumer & operator = ( const IEStubEventConsumer & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StubEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const StubAddress & StubEvent::getTargetStub( void ) const
{
    return mTargetStubAddress;
}

#endif  // AREG_COMPONENT_STUBEVENT_HPP
