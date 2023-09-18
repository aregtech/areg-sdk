//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloServiceEvents.hpp file
//////////////////////////////////////////////////////////////////////////

#ifndef  GENERATED_SRC_PRIVATE_HELLOSERVICEEVENTS_HPP
#define  GENERATED_SRC_PRIVATE_HELLOSERVICEEVENTS_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:33:56 GMT+02:00
 *                  Create by AREG SDK code generator tool from source HelloService.
 *
 * \file            generated/src/private/HelloServiceEvents.hpp
 * \ingroup         HelloService Service Interface
 * \brief           This is an automatic generated code of HelloService
 *                  Service Interface event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloService.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class HelloServiceRequestEvent;
class HelloServiceNotifyRequestEvent;
class HelloServiceResponseEvent;
class HelloServiceNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  HelloServiceRequestEvent
 *              2.  HelloServiceNotifyRequestEvent
 *              3.  HelloServiceResponseEvent
 *              4.  HelloServiceNotificationEvent
 *          These are HelloService Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The HelloServiceRequestEvent Event is used to send request message to Stub object.
 *          The HelloServiceNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The HelloServiceResponseEvent Event is used to send response or update message to Proxy object.
 *          The HelloServiceNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// HelloServiceRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloServiceRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class HelloServiceRequestEvent   : public RemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloServiceRequestEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Gets message source proxy address, message target address and message request ID.
     *
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    HelloServiceRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    HelloServiceRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    HelloServiceRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloServiceRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloServiceRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloServiceRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloServiceNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class HelloServiceNotifyRequestEvent : public RemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloServiceNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Used to send start / stop notification request from Prosy to Stub.
     *
     * \param   fromProxy   The source Proxy address, requesting start / stop notifications.
     * \param   toStub      The target Stub address to start / stop sending notifications.
     * \param   msgId       The ID of message. Normally either Attribute ID, Broadcast ID or Response ID. The Request IDs cannot be notified.
     * \param   reqType     The type of request.
     **/
    HelloServiceNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    HelloServiceNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloServiceNotifyRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloServiceNotifyRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloServiceNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// HelloServiceResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloServiceResponseEvent is used to send response events from Stub to Proxy.
 **/
class HelloServiceResponseEvent  : public RemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloServiceResponseEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Used to send response messages from Stub to Proxy. The argument list is empty.
     *
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    HelloServiceResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, SequenceNumber seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Used to send response messages from Stub to Proxy with arguments.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    HelloServiceResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, SequenceNumber seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    HelloServiceResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source );
    /**
     * \brief   Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    HelloServiceResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloServiceResponseEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ServiceResponseEvent override
/************************************************************************/

    /**
     * \brief   Clones existing service event object.
     *          Copies all data and sets specified target proxy address.
     *          Overwrite this method in every service response specific
     *          class to be able to clone events.
     * \param   target  The target proxy address.
     * \return  Cloned service response event object, which contains specified
     *          target proxy address.
     **/
    virtual ServiceResponseEvent * cloneForTarget( const ProxyAddress & target ) const override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloServiceResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloServiceResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloServiceNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class HelloServiceNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloServiceNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies notification event data
     * \param   data    The notification event data to copy
     **/
    explicit HelloServiceNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Protected destructor.
     **/
    virtual ~HelloServiceNotificationEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloServiceNotificationEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloServiceNotificationEvent );
};

#endif  // GENERATED_SRC_PRIVATE_HELLOSERVICEEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloServiceEvents.hpp file
//////////////////////////////////////////////////////////////////////////
