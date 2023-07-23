//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SystemShutdownEvents.hpp file
//////////////////////////////////////////////////////////////////////////

#ifndef  GENERATED_SRC_PRIVATE_SYSTEMSHUTDOWNEVENTS_HPP
#define  GENERATED_SRC_PRIVATE_SYSTEMSHUTDOWNEVENTS_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     23.07.2023  03:04:29 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/private/SystemShutdownEvents.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESystemShutdown.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class SystemShutdownRequestEvent;
class SystemShutdownNotifyRequestEvent;
class SystemShutdownResponseEvent;
class SystemShutdownNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  SystemShutdownRequestEvent
 *              2.  SystemShutdownNotifyRequestEvent
 *              3.  SystemShutdownResponseEvent
 *              4.  SystemShutdownNotificationEvent
 *          These are SystemShutdown Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The SystemShutdownRequestEvent Event is used to send request message to Stub object.
 *          The SystemShutdownNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The SystemShutdownResponseEvent Event is used to send response or update message to Proxy object.
 *          The SystemShutdownNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// SystemShutdownRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SystemShutdownRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class SystemShutdownRequestEvent   : public RemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SystemShutdownRequestEvent )

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
    SystemShutdownRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    SystemShutdownRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    SystemShutdownRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~SystemShutdownRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SystemShutdownRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SystemShutdownRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SystemShutdownNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class SystemShutdownNotifyRequestEvent : public RemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SystemShutdownNotifyRequestEvent )

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
    SystemShutdownNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    SystemShutdownNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~SystemShutdownNotifyRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SystemShutdownNotifyRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SystemShutdownNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// SystemShutdownResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SystemShutdownResponseEvent is used to send response events from Stub to Proxy.
 **/
class SystemShutdownResponseEvent  : public RemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SystemShutdownResponseEvent )

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
    SystemShutdownResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Used to send response messages from Stub to Proxy with arguments.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    SystemShutdownResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    SystemShutdownResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source );
    /**
     * \brief   Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    SystemShutdownResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~SystemShutdownResponseEvent( void ) = default;

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
    SystemShutdownResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SystemShutdownResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SystemShutdownNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class SystemShutdownNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SystemShutdownNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies notification event data
     * \param   data    The notification event data to copy
     **/
    explicit SystemShutdownNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Protected destructor.
     **/
    virtual ~SystemShutdownNotificationEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SystemShutdownNotificationEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SystemShutdownNotificationEvent );
};

#endif  // GENERATED_SRC_PRIVATE_SYSTEMSHUTDOWNEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SystemShutdownEvents.hpp file
//////////////////////////////////////////////////////////////////////////
