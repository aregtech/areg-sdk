//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteRegistryEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHAREIPCMIX_SRC_PRIVATE_REMOTEREGISTRYEVENTS_HPP
#define  SHAREIPCMIX_SRC_PRIVATE_REMOTEREGISTRYEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     11.06.2021  21:11:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteRegistryEvents.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shareipcmix/src/NERemoteRegistry.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/NotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class RemoteRegistryRequestEvent;
class RemoteRegistryNotifyRequestEvent;
class RemoteRegistryResponseEvent;
class RemoteRegistryNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  RemoteRegistryRequestEvent
 *              2.  RemoteRegistryNotifyRequestEvent
 *              3.  RemoteRegistryResponseEvent
 *              4.  RemoteRegistryNotificationEvent
 *          These are RemoteRegistry Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The RemoteRegistryRequestEvent Event is used to send request message to Stub object.
 *          The RemoteRegistryNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The RemoteRegistryResponseEvent Event is used to send response or update message to Proxy object.
 *          The RemoteRegistryNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RemoteRegistryRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class RemoteRegistryRequestEvent   : public RemoteRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( RemoteRegistryRequestEvent )

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
    RemoteRegistryRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    RemoteRegistryRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Initializes request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    RemoteRegistryRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteRegistryRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRegistryRequestEvent( void );
    RemoteRegistryRequestEvent( const RemoteRegistryRequestEvent & /* src */ );
    const RemoteRegistryRequestEvent & operator = ( const RemoteRegistryRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RemoteRegistryNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class RemoteRegistryNotifyRequestEvent : public RemoteNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( RemoteRegistryNotifyRequestEvent )

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
    RemoteRegistryNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Initialization constructor. Initializes notification request event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    RemoteRegistryNotifyRequestEvent( const IEInStream & stream );
    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteRegistryNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRegistryNotifyRequestEvent( void );
    RemoteRegistryNotifyRequestEvent( const RemoteRegistryNotifyRequestEvent & /* src */ );
    const RemoteRegistryNotifyRequestEvent & operator = ( const RemoteRegistryNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RemoteRegistryResponseEvent is used to send response events from Stub to Proxy.
 **/
class RemoteRegistryResponseEvent  : public RemoteResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( RemoteRegistryResponseEvent )

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
    RemoteRegistryResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    RemoteRegistryResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    RemoteRegistryResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source );
    /**
     * \brief   Initialization constructor. Initializes response event from streaming object, where data is serialized. Used in remote messaging.
     * \param   stream      The streaming object, where event data is serialized.
     **/
    RemoteRegistryResponseEvent( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    virtual ~RemoteRegistryResponseEvent( void );

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
    RemoteRegistryResponseEvent( void );
    RemoteRegistryResponseEvent( const RemoteRegistryResponseEvent & /* src */ );
    const RemoteRegistryResponseEvent & operator = ( const RemoteRegistryResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RemoteRegistryNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class RemoteRegistryNotificationEvent  : public NotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( RemoteRegistryNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    RemoteRegistryNotificationEvent( const NotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~RemoteRegistryNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRegistryNotificationEvent( void );
    RemoteRegistryNotificationEvent( const RemoteRegistryNotificationEvent & /* src */ );
    const RemoteRegistryNotificationEvent & operator = ( const RemoteRegistryNotificationEvent & /* src */ );
};

#endif   // SHAREIPCMIX_SRC_PRIVATE_REMOTEREGISTRYEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteRegistryEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 