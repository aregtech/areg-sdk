//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/TrafficLightEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_PRIVATE_TRAFFICLIGHTEVENTS_HPP
#define  SRC_GEN_PRIVATE_TRAFFICLIGHTEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/TrafficLightEvents.hpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "src/gen/NETrafficLight.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class TrafficLightRequestEvent;
class TrafficLightNotifyRequestEvent;
class TrafficLightResponseEvent;
class TrafficLightNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  TrafficLightRequestEvent
 *              2.  TrafficLightNotifyRequestEvent
 *              3.  TrafficLightResponseEvent
 *              4.  TrafficLightNotificationEvent
 *          These are TrafficLight Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The TrafficLightRequestEvent Event is used to send request message to Stub object.
 *          The TrafficLightNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The TrafficLightResponseEvent Event is used to send response or update message to Proxy object.
 *          The TrafficLightNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// TrafficLightRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TrafficLightRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class TrafficLightRequestEvent   : public LocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( TrafficLightRequestEvent )

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
    TrafficLightRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    TrafficLightRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~TrafficLightRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TrafficLightRequestEvent( void );
    TrafficLightRequestEvent( const TrafficLightRequestEvent & /* src */ );
    const TrafficLightRequestEvent & operator = ( const TrafficLightRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TrafficLightNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class TrafficLightNotifyRequestEvent : public LocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( TrafficLightNotifyRequestEvent )

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
    TrafficLightNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~TrafficLightNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TrafficLightNotifyRequestEvent( void );
    TrafficLightNotifyRequestEvent( const TrafficLightNotifyRequestEvent & /* src */ );
    const TrafficLightNotifyRequestEvent & operator = ( const TrafficLightNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// TrafficLightResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TrafficLightResponseEvent is used to send response events from Stub to Proxy.
 **/
class TrafficLightResponseEvent  : public LocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( TrafficLightResponseEvent )

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
    TrafficLightResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    TrafficLightResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    TrafficLightResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source );
    /**
     * \brief   Destructor
     **/
    virtual ~TrafficLightResponseEvent( void );

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
    TrafficLightResponseEvent( void );
    TrafficLightResponseEvent( const TrafficLightResponseEvent & /* src */ );
    const TrafficLightResponseEvent & operator = ( const TrafficLightResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TrafficLightNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class TrafficLightNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( TrafficLightNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    TrafficLightNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~TrafficLightNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TrafficLightNotificationEvent( void );
    TrafficLightNotificationEvent( const TrafficLightNotificationEvent & /* src */ );
    const TrafficLightNotificationEvent & operator = ( const TrafficLightNotificationEvent & /* src */ );
};

#endif   // SRC_GEN_PRIVATE_TRAFFICLIGHTEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/TrafficLightEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 