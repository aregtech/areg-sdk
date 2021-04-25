//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CETrafficLightEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_PRIVATE_CETRAFFICLIGHTEVENTS_HPP
#define  SRC_GEN_PRIVATE_CETRAFFICLIGHTEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source TrafficLight.
 * Generated at     21.04.2021  18:33:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CETrafficLightEvents.hpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NETrafficLight.hpp"
#include "areg/src/component/CERequestEvents.hpp"
#include "areg/src/component/CEResponseEvents.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CETrafficLightRequestEvent;
class CETrafficLightNotifyRequestEvent;
class CETrafficLightResponseEvent;
class CETrafficLightNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CETrafficLightRequestEvent
 *              2.  CETrafficLightNotifyRequestEvent
 *              3.  CETrafficLightResponseEvent
 *              4.  CETrafficLightNotificationEvent
 *          These are TrafficLight Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CETrafficLightRequestEvent Event is used to send request message to Stub object.
 *          The CETrafficLightNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CETrafficLightResponseEvent Event is used to send response or update message to Proxy object.
 *          The CETrafficLightNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CETrafficLightRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CETrafficLightRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CETrafficLightRequestEvent   : public CELocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CETrafficLightRequestEvent )

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
    CETrafficLightRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CETrafficLightRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~CETrafficLightRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CETrafficLightRequestEvent( void );
    CETrafficLightRequestEvent( const CETrafficLightRequestEvent & /* src */ );
    const CETrafficLightRequestEvent & operator = ( const CETrafficLightRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CETrafficLightNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CETrafficLightNotifyRequestEvent : public CELocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CETrafficLightNotifyRequestEvent )

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
    CETrafficLightNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~CETrafficLightNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CETrafficLightNotifyRequestEvent( void );
    CETrafficLightNotifyRequestEvent( const CETrafficLightNotifyRequestEvent & /* src */ );
    const CETrafficLightNotifyRequestEvent & operator = ( const CETrafficLightNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CETrafficLightResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CETrafficLightResponseEvent is used to send response events from Stub to Proxy.
 **/
class CETrafficLightResponseEvent  : public CELocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CETrafficLightResponseEvent )

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
    CETrafficLightResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CETrafficLightResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CETrafficLightResponseEvent( const CEProxyAddress & proxyTarget, const CELocalResponseEvent & source );
    /**
     * \brief   Destructor
     **/
    virtual ~CETrafficLightResponseEvent( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEServiceResponseEvent override
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
    virtual CEServiceResponseEvent * CloneEventForTarget( const CEProxyAddress & target ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CETrafficLightResponseEvent( void );
    CETrafficLightResponseEvent( const CETrafficLightResponseEvent & /* src */ );
    const CETrafficLightResponseEvent & operator = ( const CETrafficLightResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CETrafficLightNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CETrafficLightNotificationEvent  : public CENotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CETrafficLightNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CETrafficLightNotificationEvent( const CENotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CETrafficLightNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CETrafficLightNotificationEvent( void );
    CETrafficLightNotificationEvent( const CETrafficLightNotificationEvent & /* src */ );
    const CETrafficLightNotificationEvent & operator = ( const CETrafficLightNotificationEvent & /* src */ );
};

#endif   // SRC_GEN_PRIVATE_CETRAFFICLIGHTEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CETrafficLightEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 