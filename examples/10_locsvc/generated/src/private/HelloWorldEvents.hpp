//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWorldEvents.hpp file
//////////////////////////////////////////////////////////////////////////

#ifndef  GENERATED_SRC_PRIVATE_HELLOWORLDEVENTS_HPP
#define  GENERATED_SRC_PRIVATE_HELLOWORLDEVENTS_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:07:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/private/HelloWorldEvents.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloWorld.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class HelloWorldRequestEvent;
class HelloWorldNotifyRequestEvent;
class HelloWorldResponseEvent;
class HelloWorldNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  HelloWorldRequestEvent
 *              2.  HelloWorldNotifyRequestEvent
 *              3.  HelloWorldResponseEvent
 *              4.  HelloWorldNotificationEvent
 *          These are HelloWorld Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The HelloWorldRequestEvent Event is used to send request message to Stub object.
 *          The HelloWorldNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The HelloWorldResponseEvent Event is used to send response or update message to Proxy object.
 *          The HelloWorldNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// HelloWorldRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloWorldRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class HelloWorldRequestEvent   : public LocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloWorldRequestEvent )

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
    HelloWorldRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    HelloWorldRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWorldRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloWorldRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloWorldNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class HelloWorldNotifyRequestEvent : public LocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloWorldNotifyRequestEvent )

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
    HelloWorldNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWorldNotifyRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloWorldNotifyRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// HelloWorldResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloWorldResponseEvent is used to send response events from Stub to Proxy.
 **/
class HelloWorldResponseEvent  : public LocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloWorldResponseEvent )

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
    HelloWorldResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Used to send response messages from Stub to Proxy with arguments.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    HelloWorldResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    HelloWorldResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWorldResponseEvent( void ) = default;

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
    HelloWorldResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloWorldNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class HelloWorldNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( HelloWorldNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies notification event data
     * \param   data    The notification event data to copy
     **/
    explicit HelloWorldNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Protected destructor.
     **/
    virtual ~HelloWorldNotificationEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloWorldNotificationEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldNotificationEvent );
};

#endif  // GENERATED_SRC_PRIVATE_HELLOWORLDEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWorldEvents.hpp file
//////////////////////////////////////////////////////////////////////////
