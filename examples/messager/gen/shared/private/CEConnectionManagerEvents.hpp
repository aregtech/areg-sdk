//////////////////////////////////////////////////////////////////////////
// Begin generate gen/shared/private/CEConnectionManagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GEN_SHARED_PRIVATE_CECONNECTIONMANAGEREVENTS_HPP
#define  GEN_SHARED_PRIVATE_CECONNECTIONMANAGEREVENTS_HPP
/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     24.08.2019  21:12:30 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/shared/private/CEConnectionManagerEvents.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "gen/shared/NEConnectionManager.hpp"
#include "areg/core/component/CERequestEvents.hpp"
#include "areg/core/component/CEResponseEvents.hpp"
#include "areg/core/component/CENotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CEConnectionManagerRequestEvent;
class CEConnectionManagerNotifyRequestEvent;
class CEConnectionManagerResponseEvent;
class CEConnectionManagerNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CEConnectionManagerRequestEvent
 *              2.  CEConnectionManagerNotifyRequestEvent
 *              3.  CEConnectionManagerResponseEvent
 *              4.  CEConnectionManagerNotificationEvent
 *          These are ConnectionManager Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CEConnectionManagerRequestEvent Event is used to send request message to Stub object.
 *          The CEConnectionManagerNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CEConnectionManagerResponseEvent Event is used to send response or update message to Proxy object.
 *          The CEConnectionManagerNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEConnectionManagerRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CEConnectionManagerRequestEvent   : public CERemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEConnectionManagerRequestEvent );

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
    CEConnectionManagerRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, const unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CEConnectionManagerRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, const unsigned int reqId );
    /**
     * \brief   Initialization constructor. Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEConnectionManagerRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEConnectionManagerRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionManagerRequestEvent( void );
    CEConnectionManagerRequestEvent( const CEConnectionManagerRequestEvent & /* src */ );
    const CEConnectionManagerRequestEvent & operator = ( const CEConnectionManagerRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEConnectionManagerNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CEConnectionManagerNotifyRequestEvent : public CERemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEConnectionManagerNotifyRequestEvent );

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
    CEConnectionManagerNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, const unsigned int msgId, const NEService::eRequestType & reqType );
    /**
     * \brief   Initialization constructor. Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEConnectionManagerNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEConnectionManagerNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionManagerNotifyRequestEvent( void );
    CEConnectionManagerNotifyRequestEvent( const CEConnectionManagerNotifyRequestEvent & /* src */ );
    const CEConnectionManagerNotifyRequestEvent & operator = ( const CEConnectionManagerNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEConnectionManagerResponseEvent is used to send response events from Stub to Proxy.
 **/
class CEConnectionManagerResponseEvent  : public CERemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEConnectionManagerResponseEvent );

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
    CEConnectionManagerResponseEvent( const CEProxyAddress & proxyTarget, const NEService::eResultType & result, const unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CEConnectionManagerResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, const NEService::eResultType & result, const unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CEConnectionManagerResponseEvent( const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & source );
    /**
     * \brief   Initialization constructor. Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CEConnectionManagerResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    virtual ~CEConnectionManagerResponseEvent( void );

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
    CEConnectionManagerResponseEvent( void );
    CEConnectionManagerResponseEvent( const CEConnectionManagerResponseEvent & /* src */ );
    const CEConnectionManagerResponseEvent & operator = ( const CEConnectionManagerResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEConnectionManagerNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CEConnectionManagerNotificationEvent  : public CENotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEConnectionManagerNotificationEvent );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CEConnectionManagerNotificationEvent( const CENotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CEConnectionManagerNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionManagerNotificationEvent( void );
    CEConnectionManagerNotificationEvent( const CEConnectionManagerNotificationEvent & /* src */ );
    const CEConnectionManagerNotificationEvent & operator = ( const CEConnectionManagerNotificationEvent & /* src */ );
};

#endif   // GEN_SHARED_PRIVATE_CECONNECTIONMANAGEREVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate gen/shared/private/CEConnectionManagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 