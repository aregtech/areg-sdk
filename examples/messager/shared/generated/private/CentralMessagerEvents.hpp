//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CentralMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_PRIVATE_CENTRALMESSAGEREVENTS_HPP
#define  SHARED_GENERATED_PRIVATE_CENTRALMESSAGEREVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 * Generated at     04.07.2021  04:29:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CentralMessagerEvents.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shared/generated/NECentralMessager.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CentralMessagerRequestEvent;
class CentralMessagerNotifyRequestEvent;
class CentralMessagerResponseEvent;
class CentralMessagerNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CentralMessagerRequestEvent
 *              2.  CentralMessagerNotifyRequestEvent
 *              3.  CentralMessagerResponseEvent
 *              4.  CentralMessagerNotificationEvent
 *          These are CentralMessager Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CentralMessagerRequestEvent Event is used to send request message to Stub object.
 *          The CentralMessagerNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CentralMessagerResponseEvent Event is used to send response or update message to Proxy object.
 *          The CentralMessagerNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CentralMessagerRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CentralMessagerRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CentralMessagerRequestEvent   : public RemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CentralMessagerRequestEvent )

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
    CentralMessagerRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CentralMessagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CentralMessagerRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CentralMessagerRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CentralMessagerRequestEvent( void );
    CentralMessagerRequestEvent( const CentralMessagerRequestEvent & /* src */ );
    const CentralMessagerRequestEvent & operator = ( const CentralMessagerRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CentralMessagerNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CentralMessagerNotifyRequestEvent : public RemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CentralMessagerNotifyRequestEvent )

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
    CentralMessagerNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initialization constructor. Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CentralMessagerNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~CentralMessagerNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CentralMessagerNotifyRequestEvent( void );
    CentralMessagerNotifyRequestEvent( const CentralMessagerNotifyRequestEvent & /* src */ );
    const CentralMessagerNotifyRequestEvent & operator = ( const CentralMessagerNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CentralMessagerResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CentralMessagerResponseEvent is used to send response events from Stub to Proxy.
 **/
class CentralMessagerResponseEvent  : public RemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CentralMessagerResponseEvent )

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
    CentralMessagerResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CentralMessagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CentralMessagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source );
    /**
     * \brief   Initialization constructor. Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    CentralMessagerResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    virtual ~CentralMessagerResponseEvent( void );

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
    CentralMessagerResponseEvent( void );
    CentralMessagerResponseEvent( const CentralMessagerResponseEvent & /* src */ );
    const CentralMessagerResponseEvent & operator = ( const CentralMessagerResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CentralMessagerNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CentralMessagerNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CentralMessagerNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CentralMessagerNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CentralMessagerNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CentralMessagerNotificationEvent( void );
    CentralMessagerNotificationEvent( const CentralMessagerNotificationEvent & /* src */ );
    const CentralMessagerNotificationEvent & operator = ( const CentralMessagerNotificationEvent & /* src */ );
};

#endif   // SHARED_GENERATED_PRIVATE_CENTRALMESSAGEREVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CentralMessagerEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 