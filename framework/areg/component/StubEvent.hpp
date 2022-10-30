#ifndef AREG_COMPONENT_STUBEVENT_HPP
#define AREG_COMPONENT_STUBEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/StubEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
 * \brief   StubEvent class is a base class for all kind of Request events
 *          processed on Stub side. StubEvent cannot be used for internal event.
 *          By default it is marked as Service Internal to communicate with
 *          ServiceManager.
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
    virtual ~StubEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Event class overrides
/************************************************************************/
    /**
     * \brief   Sends the event to target thread. If target thread
     *          is nullptr, it searches event target thread, registered in system.
     **/
    virtual void deliverEvent( void ) override;

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
    virtual const IEInStream & readStream( const IEInStream & stream ) override;

    /**
     * \brief   Write component address to stream.
     * \param   stream  The writing stream to write in data
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const override;

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
    StubEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( StubEvent );
};

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   All Stub (service provider) objects are instances of 
 *          IEStubEventConsumer to receive and process component events.
 *          IEStubEventConsumer is registered at component thread as a
 *          consumer of Stub specific events. It is extended in StubBase
 *          class, which is a base class for all Stub objects.
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
    explicit IEStubEventConsumer( const StubAddress & stubAddress );

    /**
     * \brief   Destructor
     **/
    virtual ~IEStubEventConsumer( void ) = default;

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
     * \brief   This function is triggered when object is adding listener
     *          and triggered by dispatcher to indicate whether consumer
     *          registered or not.
     * \param isRegistered  On adding listener, this parameter
     *                      should be true. On removing -- false.
     *                      Otherwise there was an error adding listener.
     **/
    virtual void consumerRegistered( bool isRegistered ) override;

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
    virtual void startEventProcessing( Event & eventElem ) override;

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
    IEStubEventConsumer( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IEStubEventConsumer );
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
