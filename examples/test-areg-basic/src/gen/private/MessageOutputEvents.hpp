//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/MessageOutputEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_PRIVATE_MESSAGEOUTPUTEVENTS_HPP
#define  SRC_GEN_PRIVATE_MESSAGEOUTPUTEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/MessageOutputEvents.hpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "src/gen/NEMessageOutput.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class MessageOutputRequestEvent;
class MessageOutputNotifyRequestEvent;
class MessageOutputResponseEvent;
class MessageOutputNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  MessageOutputRequestEvent
 *              2.  MessageOutputNotifyRequestEvent
 *              3.  MessageOutputResponseEvent
 *              4.  MessageOutputNotificationEvent
 *          These are MessageOutput Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The MessageOutputRequestEvent Event is used to send request message to Stub object.
 *          The MessageOutputNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The MessageOutputResponseEvent Event is used to send response or update message to Proxy object.
 *          The MessageOutputNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// MessageOutputRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   MessageOutputRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class MessageOutputRequestEvent   : public LocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( MessageOutputRequestEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Gets message source proxy address, message target address and message request ID.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    MessageOutputRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    MessageOutputRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~MessageOutputRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    MessageOutputRequestEvent( void );
    MessageOutputRequestEvent( const MessageOutputRequestEvent & /* src */ );
    const MessageOutputRequestEvent & operator = ( const MessageOutputRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   MessageOutputNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class MessageOutputNotifyRequestEvent : public LocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( MessageOutputNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Used to send start / stop notification request from Prosy to Stub.
     * \param   fromProxy   The source Proxy address, requesting start / stop notifications.
     * \param   toStub      The target Stub address to start / stop sending notifications.
     * \param   msgId       The ID of message. Normally either Attribute ID, Broadcast ID or Response ID. The Request IDs cannot be notified.
     * \param   reqType     The type of request.
     **/
    MessageOutputNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~MessageOutputNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    MessageOutputNotifyRequestEvent( void );
    MessageOutputNotifyRequestEvent( const MessageOutputNotifyRequestEvent & /* src */ );
    const MessageOutputNotifyRequestEvent & operator = ( const MessageOutputNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// MessageOutputResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   MessageOutputResponseEvent is used to send response events from Stub to Proxy.
 **/
class MessageOutputResponseEvent  : public LocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( MessageOutputResponseEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy. The argument list is empty.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    MessageOutputResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    MessageOutputResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    MessageOutputResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source );
    /**
     * \brief   Destructor
     **/
    virtual ~MessageOutputResponseEvent( void );

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
    virtual ServiceResponseEvent * cloneEventForTarget( const ProxyAddress & target ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    MessageOutputResponseEvent( void );
    MessageOutputResponseEvent( const MessageOutputResponseEvent & /* src */ );
    const MessageOutputResponseEvent & operator = ( const MessageOutputResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   MessageOutputNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class MessageOutputNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( MessageOutputNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    MessageOutputNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~MessageOutputNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    MessageOutputNotificationEvent( void );
    MessageOutputNotificationEvent( const MessageOutputNotificationEvent & /* src */ );
    const MessageOutputNotificationEvent & operator = ( const MessageOutputNotificationEvent & /* src */ );
};

#endif   // SRC_GEN_PRIVATE_MESSAGEOUTPUTEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/MessageOutputEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 