//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CECentralMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_PRIVATE_CECENTRALMESSAGEREVENTS_HPP
#define  SHARED_GENERATED_PRIVATE_CECENTRALMESSAGEREVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     25.04.2021  23:50:42 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CECentralMessagerEvents.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "shared/generated/NECentralMessager.hpp"
#include "areg/src/component/CERequestEvents.hpp"
#include "areg/src/component/CEResponseEvents.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CECentralMessagerRequestEvent;
class CECentralMessagerNotifyRequestEvent;
class CECentralMessagerResponseEvent;
class CECentralMessagerNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CECentralMessagerRequestEvent
 *              2.  CECentralMessagerNotifyRequestEvent
 *              3.  CECentralMessagerResponseEvent
 *              4.  CECentralMessagerNotificationEvent
 *          These are CentralMessager Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CECentralMessagerRequestEvent Event is used to send request message to Stub object.
 *          The CECentralMessagerNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CECentralMessagerResponseEvent Event is used to send response or update message to Proxy object.
 *          The CECentralMessagerNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CECentralMessagerRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CECentralMessagerRequestEvent   : public CERemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CECentralMessagerRequestEvent )

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
    CECentralMessagerRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CECentralMessagerRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CECentralMessagerRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CECentralMessagerRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CECentralMessagerRequestEvent( void );
    CECentralMessagerRequestEvent( const CECentralMessagerRequestEvent & /* src */ );
    const CECentralMessagerRequestEvent & operator = ( const CECentralMessagerRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CECentralMessagerNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CECentralMessagerNotifyRequestEvent : public CERemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CECentralMessagerNotifyRequestEvent )

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
    CECentralMessagerNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initialization constructor. Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CECentralMessagerNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CECentralMessagerNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CECentralMessagerNotifyRequestEvent( void );
    CECentralMessagerNotifyRequestEvent( const CECentralMessagerNotifyRequestEvent & /* src */ );
    const CECentralMessagerNotifyRequestEvent & operator = ( const CECentralMessagerNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CECentralMessagerResponseEvent is used to send response events from Stub to Proxy.
 **/
class CECentralMessagerResponseEvent  : public CERemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CECentralMessagerResponseEvent )

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
    CECentralMessagerResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CECentralMessagerResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CECentralMessagerResponseEvent( const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & source );
    /**
     * \brief   Initialization constructor. Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CECentralMessagerResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    virtual ~CECentralMessagerResponseEvent( void );

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
    CECentralMessagerResponseEvent( void );
    CECentralMessagerResponseEvent( const CECentralMessagerResponseEvent & /* src */ );
    const CECentralMessagerResponseEvent & operator = ( const CECentralMessagerResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CECentralMessagerNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CECentralMessagerNotificationEvent  : public CENotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CECentralMessagerNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CECentralMessagerNotificationEvent( const CENotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CECentralMessagerNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CECentralMessagerNotificationEvent( void );
    CECentralMessagerNotificationEvent( const CECentralMessagerNotificationEvent & /* src */ );
    const CECentralMessagerNotificationEvent & operator = ( const CECentralMessagerNotificationEvent & /* src */ );
};

#endif   // SHARED_GENERATED_PRIVATE_CECENTRALMESSAGEREVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CECentralMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 