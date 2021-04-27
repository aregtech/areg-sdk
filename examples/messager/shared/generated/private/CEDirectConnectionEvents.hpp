//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectConnectionEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_PRIVATE_CEDIRECTCONNECTIONEVENTS_HPP
#define  SHARED_GENERATED_PRIVATE_CEDIRECTCONNECTIONEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     25.04.2021  23:50:45 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectConnectionEvents.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shared/generated/NEDirectConnection.hpp"
#include "areg/component/CERequestEvents.hpp"
#include "areg/component/CEResponseEvents.hpp"
#include "areg/component/CENotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CEDirectConnectionRequestEvent;
class CEDirectConnectionNotifyRequestEvent;
class CEDirectConnectionResponseEvent;
class CEDirectConnectionNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CEDirectConnectionRequestEvent
 *              2.  CEDirectConnectionNotifyRequestEvent
 *              3.  CEDirectConnectionResponseEvent
 *              4.  CEDirectConnectionNotificationEvent
 *          These are DirectConnection Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CEDirectConnectionRequestEvent Event is used to send request message to Stub object.
 *          The CEDirectConnectionNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CEDirectConnectionResponseEvent Event is used to send response or update message to Proxy object.
 *          The CEDirectConnectionNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectConnectionRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CEDirectConnectionRequestEvent   : public CERemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectConnectionRequestEvent )

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
    CEDirectConnectionRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CEDirectConnectionRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEDirectConnectionRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectConnectionRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectConnectionRequestEvent( void );
    CEDirectConnectionRequestEvent( const CEDirectConnectionRequestEvent & /* src */ );
    const CEDirectConnectionRequestEvent & operator = ( const CEDirectConnectionRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectConnectionNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CEDirectConnectionNotifyRequestEvent : public CERemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectConnectionNotifyRequestEvent )

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
    CEDirectConnectionNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initialization constructor. Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEDirectConnectionNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectConnectionNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectConnectionNotifyRequestEvent( void );
    CEDirectConnectionNotifyRequestEvent( const CEDirectConnectionNotifyRequestEvent & /* src */ );
    const CEDirectConnectionNotifyRequestEvent & operator = ( const CEDirectConnectionNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectConnectionResponseEvent is used to send response events from Stub to Proxy.
 **/
class CEDirectConnectionResponseEvent  : public CERemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectConnectionResponseEvent )

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
    CEDirectConnectionResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CEDirectConnectionResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CEDirectConnectionResponseEvent( const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & source );
    /**
     * \brief   Initialization constructor. Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEDirectConnectionResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    virtual ~CEDirectConnectionResponseEvent( void );

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
    CEDirectConnectionResponseEvent( void );
    CEDirectConnectionResponseEvent( const CEDirectConnectionResponseEvent & /* src */ );
    const CEDirectConnectionResponseEvent & operator = ( const CEDirectConnectionResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectConnectionNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CEDirectConnectionNotificationEvent  : public CENotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectConnectionNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CEDirectConnectionNotificationEvent( const CENotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CEDirectConnectionNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectConnectionNotificationEvent( void );
    CEDirectConnectionNotificationEvent( const CEDirectConnectionNotificationEvent & /* src */ );
    const CEDirectConnectionNotificationEvent & operator = ( const CEDirectConnectionNotificationEvent & /* src */ );
};

#endif   // SHARED_GENERATED_PRIVATE_CEDIRECTCONNECTIONEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectConnectionEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 