//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////

#ifndef  GENERATED_PRIVATE_DIRECTMESSAGEREVENTS_HPP
#define  GENERATED_PRIVATE_DIRECTMESSAGEREVENTS_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/private/DirectMessagerEvents.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NEDirectMessager.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class DirectMessagerRequestEvent;
class DirectMessagerNotifyRequestEvent;
class DirectMessagerResponseEvent;
class DirectMessagerNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  DirectMessagerRequestEvent
 *              2.  DirectMessagerNotifyRequestEvent
 *              3.  DirectMessagerResponseEvent
 *              4.  DirectMessagerNotificationEvent
 *          These are DirectMessager Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The DirectMessagerRequestEvent Event is used to send request message to Stub object.
 *          The DirectMessagerNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The DirectMessagerResponseEvent Event is used to send response or update message to Proxy object.
 *          The DirectMessagerNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// DirectMessagerRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   DirectMessagerRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class DirectMessagerRequestEvent   : public RemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( DirectMessagerRequestEvent )

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
    DirectMessagerRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    DirectMessagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    DirectMessagerRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~DirectMessagerRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DirectMessagerRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectMessagerRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   DirectMessagerNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class DirectMessagerNotifyRequestEvent : public RemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( DirectMessagerNotifyRequestEvent )

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
    DirectMessagerNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    DirectMessagerNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~DirectMessagerNotifyRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DirectMessagerNotifyRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectMessagerNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// DirectMessagerResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   DirectMessagerResponseEvent is used to send response events from Stub to Proxy.
 **/
class DirectMessagerResponseEvent  : public RemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( DirectMessagerResponseEvent )

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
    DirectMessagerResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Used to send response messages from Stub to Proxy with arguments.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    DirectMessagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    DirectMessagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source );
    /**
     * \brief   Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    DirectMessagerResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~DirectMessagerResponseEvent( void ) = default;

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
    DirectMessagerResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectMessagerResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   DirectMessagerNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class DirectMessagerNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( DirectMessagerNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies notification event data
     * \param   data    The notification event data to copy
     **/
    explicit DirectMessagerNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Protected destructor.
     **/
    virtual ~DirectMessagerNotificationEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DirectMessagerNotificationEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectMessagerNotificationEvent );
};

#endif  // GENERATED_PRIVATE_DIRECTMESSAGEREVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
