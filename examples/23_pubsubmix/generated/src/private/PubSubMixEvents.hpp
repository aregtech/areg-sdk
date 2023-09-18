//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubMixEvents.hpp file
//////////////////////////////////////////////////////////////////////////

#ifndef  GENERATED_SRC_PRIVATE_PUBSUBMIXEVENTS_HPP
#define  GENERATED_SRC_PRIVATE_PUBSUBMIXEVENTS_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:34:02 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/private/PubSubMixEvents.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPubSubMix.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class PubSubMixRequestEvent;
class PubSubMixNotifyRequestEvent;
class PubSubMixResponseEvent;
class PubSubMixNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  PubSubMixRequestEvent
 *              2.  PubSubMixNotifyRequestEvent
 *              3.  PubSubMixResponseEvent
 *              4.  PubSubMixNotificationEvent
 *          These are PubSubMix Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The PubSubMixRequestEvent Event is used to send request message to Stub object.
 *          The PubSubMixNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The PubSubMixResponseEvent Event is used to send response or update message to Proxy object.
 *          The PubSubMixNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// PubSubMixRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PubSubMixRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class PubSubMixRequestEvent   : public RemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PubSubMixRequestEvent )

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
    PubSubMixRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    PubSubMixRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    PubSubMixRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubMixRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubMixRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubMixRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PubSubMixNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class PubSubMixNotifyRequestEvent : public RemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PubSubMixNotifyRequestEvent )

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
    PubSubMixNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    PubSubMixNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubMixNotifyRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubMixNotifyRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubMixNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// PubSubMixResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PubSubMixResponseEvent is used to send response events from Stub to Proxy.
 **/
class PubSubMixResponseEvent  : public RemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PubSubMixResponseEvent )

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
    PubSubMixResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, SequenceNumber seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Used to send response messages from Stub to Proxy with arguments.
     *
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    PubSubMixResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, SequenceNumber seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    PubSubMixResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source );
    /**
     * \brief   Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     *
     * \param   stream      The streaming object, where event data is serialized.
     **/
    PubSubMixResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubMixResponseEvent( void ) = default;

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
    PubSubMixResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubMixResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PubSubMixNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class PubSubMixNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( PubSubMixNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies notification event data
     * \param   data    The notification event data to copy
     **/
    explicit PubSubMixNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Protected destructor.
     **/
    virtual ~PubSubMixNotificationEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubMixNotificationEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PubSubMixNotificationEvent );
};

#endif  // GENERATED_SRC_PRIVATE_PUBSUBMIXEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubMixEvents.hpp file
//////////////////////////////////////////////////////////////////////////
