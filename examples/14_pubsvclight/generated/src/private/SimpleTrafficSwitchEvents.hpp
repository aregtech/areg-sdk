//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficSwitchEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PRIVATE_SIMPLETRAFFICSWITCHEVENTS_HPP
#define  GENERATED_SRC_PRIVATE_SIMPLETRAFFICSWITCHEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 * Generated at     11.08.2021  13:18:21 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/SimpleTrafficSwitchEvents.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NESimpleTrafficSwitch.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class SimpleTrafficSwitchRequestEvent;
class SimpleTrafficSwitchNotifyRequestEvent;
class SimpleTrafficSwitchResponseEvent;
class SimpleTrafficSwitchNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  SimpleTrafficSwitchRequestEvent
 *              2.  SimpleTrafficSwitchNotifyRequestEvent
 *              3.  SimpleTrafficSwitchResponseEvent
 *              4.  SimpleTrafficSwitchNotificationEvent
 *          These are SimpleTrafficSwitch Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The SimpleTrafficSwitchRequestEvent Event is used to send request message to Stub object.
 *          The SimpleTrafficSwitchNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The SimpleTrafficSwitchResponseEvent Event is used to send response or update message to Proxy object.
 *          The SimpleTrafficSwitchNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SimpleTrafficSwitchRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class SimpleTrafficSwitchRequestEvent   : public LocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SimpleTrafficSwitchRequestEvent )

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
    SimpleTrafficSwitchRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    SimpleTrafficSwitchRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~SimpleTrafficSwitchRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SimpleTrafficSwitchRequestEvent( void );
    SimpleTrafficSwitchRequestEvent( const SimpleTrafficSwitchRequestEvent & /* src */ );
    const SimpleTrafficSwitchRequestEvent & operator = ( const SimpleTrafficSwitchRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SimpleTrafficSwitchNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class SimpleTrafficSwitchNotifyRequestEvent : public LocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SimpleTrafficSwitchNotifyRequestEvent )

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
    SimpleTrafficSwitchNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~SimpleTrafficSwitchNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SimpleTrafficSwitchNotifyRequestEvent( void );
    SimpleTrafficSwitchNotifyRequestEvent( const SimpleTrafficSwitchNotifyRequestEvent & /* src */ );
    const SimpleTrafficSwitchNotifyRequestEvent & operator = ( const SimpleTrafficSwitchNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SimpleTrafficSwitchResponseEvent is used to send response events from Stub to Proxy.
 **/
class SimpleTrafficSwitchResponseEvent  : public LocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SimpleTrafficSwitchResponseEvent )

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
    SimpleTrafficSwitchResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    SimpleTrafficSwitchResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    SimpleTrafficSwitchResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source );
    /**
     * \brief   Destructor
     **/
    virtual ~SimpleTrafficSwitchResponseEvent( void );

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
    virtual ServiceResponseEvent * cloneForTarget( const ProxyAddress & target ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SimpleTrafficSwitchResponseEvent( void );
    SimpleTrafficSwitchResponseEvent( const SimpleTrafficSwitchResponseEvent & /* src */ );
    const SimpleTrafficSwitchResponseEvent & operator = ( const SimpleTrafficSwitchResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SimpleTrafficSwitchNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class SimpleTrafficSwitchNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( SimpleTrafficSwitchNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    SimpleTrafficSwitchNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~SimpleTrafficSwitchNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SimpleTrafficSwitchNotificationEvent( void );
    SimpleTrafficSwitchNotificationEvent( const SimpleTrafficSwitchNotificationEvent & /* src */ );
    const SimpleTrafficSwitchNotificationEvent & operator = ( const SimpleTrafficSwitchNotificationEvent & /* src */ );
};

#endif   // GENERATED_SRC_PRIVATE_SIMPLETRAFFICSWITCHEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficSwitchEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 