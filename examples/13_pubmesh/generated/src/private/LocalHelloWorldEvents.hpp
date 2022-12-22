//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LocalHelloWorldEvents.hpp file
//////////////////////////////////////////////////////////////////////////

#ifndef  GENERATED_SRC_PRIVATE_LOCALHELLOWORLDEVENTS_HPP
#define  GENERATED_SRC_PRIVATE_LOCALHELLOWORLDEVENTS_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:19:16 GMT+01:00
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/private/LocalHelloWorldEvents.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NELocalHelloWorld.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class LocalHelloWorldRequestEvent;
class LocalHelloWorldNotifyRequestEvent;
class LocalHelloWorldResponseEvent;
class LocalHelloWorldNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  LocalHelloWorldRequestEvent
 *              2.  LocalHelloWorldNotifyRequestEvent
 *              3.  LocalHelloWorldResponseEvent
 *              4.  LocalHelloWorldNotificationEvent
 *          These are LocalHelloWorld Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The LocalHelloWorldRequestEvent Event is used to send request message to Stub object.
 *          The LocalHelloWorldNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The LocalHelloWorldResponseEvent Event is used to send response or update message to Proxy object.
 *          The LocalHelloWorldNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LocalHelloWorldRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class LocalHelloWorldRequestEvent   : public LocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( LocalHelloWorldRequestEvent )

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
    LocalHelloWorldRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    LocalHelloWorldRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~LocalHelloWorldRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalHelloWorldRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalHelloWorldRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LocalHelloWorldNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class LocalHelloWorldNotifyRequestEvent : public LocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( LocalHelloWorldNotifyRequestEvent )

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
    LocalHelloWorldNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~LocalHelloWorldNotifyRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalHelloWorldNotifyRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalHelloWorldNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LocalHelloWorldResponseEvent is used to send response events from Stub to Proxy.
 **/
class LocalHelloWorldResponseEvent  : public LocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( LocalHelloWorldResponseEvent )

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
    LocalHelloWorldResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Used to send response messages from Stub to Proxy with arguments.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    LocalHelloWorldResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    LocalHelloWorldResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source );
    /**
     * \brief   Destructor.
     **/
    virtual ~LocalHelloWorldResponseEvent( void ) = default;

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
    LocalHelloWorldResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalHelloWorldResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LocalHelloWorldNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class LocalHelloWorldNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( LocalHelloWorldNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies notification event data
     * \param   data    The notification event data to copy
     **/
    explicit LocalHelloWorldNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Protected destructor.
     **/
    virtual ~LocalHelloWorldNotificationEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalHelloWorldNotificationEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LocalHelloWorldNotificationEvent );
};

#endif  // GENERATED_SRC_PRIVATE_LOCALHELLOWORLDEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LocalHelloWorldEvents.hpp file
//////////////////////////////////////////////////////////////////////////
