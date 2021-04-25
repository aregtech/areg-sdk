//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CESystemEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_PRIVATE_CESYSTEMEVENTS_HPP
#define  SRC_GEN_PRIVATE_CESYSTEMEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source System.
 * Generated at     21.04.2021  18:33:57 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CESystemEvents.hpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NESystem.hpp"
#include "areg/src/component/CERequestEvents.hpp"
#include "areg/src/component/CEResponseEvents.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CESystemRequestEvent;
class CESystemNotifyRequestEvent;
class CESystemResponseEvent;
class CESystemNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CESystemRequestEvent
 *              2.  CESystemNotifyRequestEvent
 *              3.  CESystemResponseEvent
 *              4.  CESystemNotificationEvent
 *          These are System Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CESystemRequestEvent Event is used to send request message to Stub object.
 *          The CESystemNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CESystemResponseEvent Event is used to send response or update message to Proxy object.
 *          The CESystemNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CESystemRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CESystemRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CESystemRequestEvent   : public CELocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CESystemRequestEvent )

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
    CESystemRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CESystemRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~CESystemRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESystemRequestEvent( void );
    CESystemRequestEvent( const CESystemRequestEvent & /* src */ );
    const CESystemRequestEvent & operator = ( const CESystemRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CESystemNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CESystemNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CESystemNotifyRequestEvent : public CELocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CESystemNotifyRequestEvent )

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
    CESystemNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~CESystemNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESystemNotifyRequestEvent( void );
    CESystemNotifyRequestEvent( const CESystemNotifyRequestEvent & /* src */ );
    const CESystemNotifyRequestEvent & operator = ( const CESystemNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CESystemResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CESystemResponseEvent is used to send response events from Stub to Proxy.
 **/
class CESystemResponseEvent  : public CELocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CESystemResponseEvent )

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
    CESystemResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CESystemResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CESystemResponseEvent( const CEProxyAddress & proxyTarget, const CELocalResponseEvent & source );
    /**
     * \brief   Destructor
     **/
    virtual ~CESystemResponseEvent( void );

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
    CESystemResponseEvent( void );
    CESystemResponseEvent( const CESystemResponseEvent & /* src */ );
    const CESystemResponseEvent & operator = ( const CESystemResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CESystemNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CESystemNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CESystemNotificationEvent  : public CENotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CESystemNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CESystemNotificationEvent( const CENotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CESystemNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESystemNotificationEvent( void );
    CESystemNotificationEvent( const CESystemNotificationEvent & /* src */ );
    const CESystemNotificationEvent & operator = ( const CESystemNotificationEvent & /* src */ );
};

#endif   // SRC_GEN_PRIVATE_CESYSTEMEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CESystemEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 