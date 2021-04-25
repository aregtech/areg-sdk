//////////////////////////////////////////////////////////////////////////
// Begin generate gen/shared/private/CEDirectMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GEN_SHARED_PRIVATE_CEDIRECTMESSAGEREVENTS_HPP
#define  GEN_SHARED_PRIVATE_CEDIRECTMESSAGEREVENTS_HPP
/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     24.08.2019  21:12:33 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/shared/private/CEDirectMessagerEvents.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "gen/shared/NEDirectMessager.hpp"
#include "areg/core/component/CERequestEvents.hpp"
#include "areg/core/component/CEResponseEvents.hpp"
#include "areg/core/component/CENotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CEDirectMessagerRequestEvent;
class CEDirectMessagerNotifyRequestEvent;
class CEDirectMessagerResponseEvent;
class CEDirectMessagerNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CEDirectMessagerRequestEvent
 *              2.  CEDirectMessagerNotifyRequestEvent
 *              3.  CEDirectMessagerResponseEvent
 *              4.  CEDirectMessagerNotificationEvent
 *          These are DirectMessager Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CEDirectMessagerRequestEvent Event is used to send request message to Stub object.
 *          The CEDirectMessagerNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CEDirectMessagerResponseEvent Event is used to send response or update message to Proxy object.
 *          The CEDirectMessagerNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectMessagerRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CEDirectMessagerRequestEvent   : public CERemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectMessagerRequestEvent );

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
    CEDirectMessagerRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, const unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CEDirectMessagerRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, const unsigned int reqId );
    /**
     * \brief   Initialization constructor. Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEDirectMessagerRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectMessagerRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectMessagerRequestEvent( void );
    CEDirectMessagerRequestEvent( const CEDirectMessagerRequestEvent & /* src */ );
    const CEDirectMessagerRequestEvent & operator = ( const CEDirectMessagerRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectMessagerNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CEDirectMessagerNotifyRequestEvent : public CERemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectMessagerNotifyRequestEvent );

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
    CEDirectMessagerNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, const unsigned int msgId, const NEService::eRequestType & reqType );
    /**
     * \brief   Initialization constructor. Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEDirectMessagerNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectMessagerNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectMessagerNotifyRequestEvent( void );
    CEDirectMessagerNotifyRequestEvent( const CEDirectMessagerNotifyRequestEvent & /* src */ );
    const CEDirectMessagerNotifyRequestEvent & operator = ( const CEDirectMessagerNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectMessagerResponseEvent is used to send response events from Stub to Proxy.
 **/
class CEDirectMessagerResponseEvent  : public CERemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectMessagerResponseEvent );

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
    CEDirectMessagerResponseEvent( const CEProxyAddress & proxyTarget, const NEService::eResultType & result, const unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CEDirectMessagerResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, const NEService::eResultType & result, const unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CEDirectMessagerResponseEvent( const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & source );
    /**
     * \brief   Initialization constructor. Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEDirectMessagerResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    virtual ~CEDirectMessagerResponseEvent( void );

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
    CEDirectMessagerResponseEvent( void );
    CEDirectMessagerResponseEvent( const CEDirectMessagerResponseEvent & /* src */ );
    const CEDirectMessagerResponseEvent & operator = ( const CEDirectMessagerResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectMessagerNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CEDirectMessagerNotificationEvent  : public CENotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEDirectMessagerNotificationEvent );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CEDirectMessagerNotificationEvent( const CENotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CEDirectMessagerNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectMessagerNotificationEvent( void );
    CEDirectMessagerNotificationEvent( const CEDirectMessagerNotificationEvent & /* src */ );
    const CEDirectMessagerNotificationEvent & operator = ( const CEDirectMessagerNotificationEvent & /* src */ );
};

#endif   // GEN_SHARED_PRIVATE_CEDIRECTMESSAGEREVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate gen/shared/private/CEDirectMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 