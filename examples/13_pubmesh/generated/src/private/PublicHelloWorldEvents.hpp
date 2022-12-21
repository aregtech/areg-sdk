//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PublicHelloWorldEvents.hpp file
//////////////////////////////////////////////////////////////////////////

#ifndef  GENERATED_SRC_PRIVATE_PUBLICHELLOWORLDEVENTS_HPP
#define  GENERATED_SRC_PRIVATE_PUBLICHELLOWORLDEVENTS_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:19:17 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/private/PublicHelloWorldEvents.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
 *                  Service Interface event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPublicHelloWorld.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class PublicHelloWorldRequestEvent;
class PublicHelloWorldNotifyRequestEvent;
class PublicHelloWorldResponseEvent;
class PublicHelloWorldNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  PublicHelloWorldRequestEvent
 *              2.  PublicHelloWorldNotifyRequestEvent
 *              3.  PublicHelloWorldResponseEvent
 *              4.  PublicHelloWorldNotificationEvent
 *          These are PublicHelloWorld Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The PublicHelloWorldRequestEvent Event is used to send request message to Stub object.
 *          The PublicHelloWorldNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The PublicHelloWorldResponseEvent Event is used to send response or update message to Proxy object.
 *          The PublicHelloWorldNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PublicHelloWorldRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class PublicHelloWorldRequestEvent   : public RemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PublicHelloWorldRequestEvent )

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
    PublicHelloWorldRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    PublicHelloWorldRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    PublicHelloWorldRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~PublicHelloWorldRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PublicHelloWorldRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PublicHelloWorldRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PublicHelloWorldNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class PublicHelloWorldNotifyRequestEvent : public RemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PublicHelloWorldNotifyRequestEvent )

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
    PublicHelloWorldNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    PublicHelloWorldNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~PublicHelloWorldNotifyRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PublicHelloWorldNotifyRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PublicHelloWorldNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PublicHelloWorldResponseEvent is used to send response events from Stub to Proxy.
 **/
class PublicHelloWorldResponseEvent  : public RemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PublicHelloWorldResponseEvent )

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
    PublicHelloWorldResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Used to send response messages from Stub to Proxy with arguments.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    PublicHelloWorldResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    PublicHelloWorldResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source );
    /**
     * \brief   Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    PublicHelloWorldResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~PublicHelloWorldResponseEvent( void ) = default;

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
    PublicHelloWorldResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PublicHelloWorldResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PublicHelloWorldNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class PublicHelloWorldNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PublicHelloWorldNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies notification event data
     * \param   data    The notification event data to copy
     **/
    explicit PublicHelloWorldNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Protected destructor.
     **/
    virtual ~PublicHelloWorldNotificationEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PublicHelloWorldNotificationEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PublicHelloWorldNotificationEvent );
};

#endif  // GENERATED_SRC_PRIVATE_PUBLICHELLOWORLDEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PublicHelloWorldEvents.hpp file
//////////////////////////////////////////////////////////////////////////
