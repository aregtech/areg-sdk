//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CEMessageOutputEvents.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_PRIVATE_CEMESSAGEOUTPUTEVENTS_HPP
#define  SRC_GEN_PRIVATE_CEMESSAGEOUTPUTEVENTS_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CEMessageOutputEvents.hpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Event classes declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NEMessageOutput.hpp"
#include "areg/src/component/CERequestEvents.hpp"
#include "areg/src/component/CEResponseEvents.hpp"
#include "areg/src/component/CENotificationEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

/************************************************************************
 * Declared classes
 ************************************************************************/
class CEMessageOutputRequestEvent;
class CEMessageOutputNotifyRequestEvent;
class CEMessageOutputResponseEvent;
class CEMessageOutputNotificationEvent;

/**
 * \brief   In this file following classes are declared:
 *              1.  CEMessageOutputRequestEvent
 *              2.  CEMessageOutputNotifyRequestEvent
 *              3.  CEMessageOutputResponseEvent
 *              4.  CEMessageOutputNotificationEvent
 *          These are MessageOutput Service Interface event objects.
 *          They are used to send Communication Events to Proxy,
 *          Stub and Client objects.
 *          The CEMessageOutputRequestEvent Event is used to send request message to Stub object.
 *          The CEMessageOutputNotifyRequestEvent Event is used to assign for Notifications of Attributes or Responses / Broadcasts.
 *          The CEMessageOutputResponseEvent Event is used to send response or update message to Proxy object.
 *          The CEMessageOutputNotificationEvent Event is used to trigger functions on Client object.
 *          For more details see descriptions bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEMessageOutputRequestEvent is used to send request events from Proxy objects to Stub
 *          Every request is generating Request Event and forwarded to Stub for processing.
 **/
class CEMessageOutputRequestEvent   : public CELocalRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEMessageOutputRequestEvent )

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
    CEMessageOutputRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Initialization constructor. Gets streamable data of arguments, message source proxy address, message target address  and message request ID.
     * \param   args        The streamable object containing message arguments.
     * \param   fromSource  The Proxy message source address.
     * \param   toTarget    The Stub message target address to deliver event.
     * \param   reqId       The request message ID.
     **/
    CEMessageOutputRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEMessageOutputRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEMessageOutputRequestEvent( void );
    CEMessageOutputRequestEvent( const CEMessageOutputRequestEvent & /* src */ );
    const CEMessageOutputRequestEvent & operator = ( const CEMessageOutputRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEMessageOutputNotifyRequestEvent is used to send notification request events.
 *          Used when Proxy is requesting to start / stop notifications of attribute update, Broadcast, or response.
 **/
class CEMessageOutputNotifyRequestEvent : public CELocalNotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEMessageOutputNotifyRequestEvent )

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
    CEMessageOutputNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEMessageOutputNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEMessageOutputNotifyRequestEvent( void );
    CEMessageOutputNotifyRequestEvent( const CEMessageOutputNotifyRequestEvent & /* src */ );
    const CEMessageOutputNotifyRequestEvent & operator = ( const CEMessageOutputNotifyRequestEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEMessageOutputResponseEvent is used to send response events from Stub to Proxy.
 **/
class CEMessageOutputResponseEvent  : public CELocalResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEMessageOutputResponseEvent )

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
    CEMessageOutputResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to send response messages from Stub to Proxy with arguments.
     * \param   args        The streamable object containing message arguments.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   result      The result code of response message
     * \param   responseId  The response message ID. Normally either attribute, broadcast or response ID.
     * \param   seqNr       The sequence number of response call. By default it is NEService::SEQUENCE_NUMBER_NOTIFY.
     **/
    CEMessageOutputResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );
    /**
     * \brief   Initialization constructor. Used to clone same message for different Proxy objects.
     * \param   proxyTarget The target Proxy address to deliver message
     * \param   source      The source event object containing message ID, result code and parameters.
     **/
    CEMessageOutputResponseEvent( const CEProxyAddress & proxyTarget, const CELocalResponseEvent & source );
    /**
     * \brief   Destructor
     **/
    virtual ~CEMessageOutputResponseEvent( void );

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
    CEMessageOutputResponseEvent( void );
    CEMessageOutputResponseEvent( const CEMessageOutputResponseEvent & /* src */ );
    const CEMessageOutputResponseEvent & operator = ( const CEMessageOutputResponseEvent & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotificationEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEMessageOutputNotificationEvent is used by Proxy to send notifications to client objects
 *          and trigger appropriate functions
 **/
class CEMessageOutputNotificationEvent  : public CENotificationEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT( CEMessageOutputNotificationEvent )

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Copies notification event data
     * \param   data    The notification event data to copy
     **/
    CEMessageOutputNotificationEvent( const CENotificationEventData & data );

protected:
    /**
     * \brief   Destructor. Protected
     **/
    virtual ~CEMessageOutputNotificationEvent( void );
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEMessageOutputNotificationEvent( void );
    CEMessageOutputNotificationEvent( const CEMessageOutputNotificationEvent & /* src */ );
    const CEMessageOutputNotificationEvent & operator = ( const CEMessageOutputNotificationEvent & /* src */ );
};

#endif   // SRC_GEN_PRIVATE_CEMESSAGEOUTPUTEVENTS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CEMessageOutputEvents.hpp file
//////////////////////////////////////////////////////////////////////////
 