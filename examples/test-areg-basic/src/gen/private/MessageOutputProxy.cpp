//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/MessageOutputProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/MessageOutputProxy.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/private/MessageOutputProxy.hpp"
#include "src/gen/private/MessageOutputEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// MessageOutputProxy::MessageOutputServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(MessageOutputProxy::MessageOutputServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

MessageOutputProxy::MessageOutputServiceAvailableEvent::MessageOutputServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

MessageOutputProxy::MessageOutputServiceAvailableEvent::~MessageOutputServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// MessageOutputProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & MessageOutputProxy::_createInterfaceData( void )
{
    return NEMessageOutput::createInterfaceData( );
}

ProxyBase * MessageOutputProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW MessageOutputProxy(roleName, ownerThread);
}

MessageOutputProxy * MessageOutputProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<MessageOutputProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEMessageOutput::createInterfaceData()
                                                                      , connectListener
                                                                      , &MessageOutputProxy::_createProxy
                                                                      , ownerThread) );
}

MessageOutputProxy * MessageOutputProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<MessageOutputProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEMessageOutput::createInterfaceData()
                                                                      , connectListener
                                                                      , &MessageOutputProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
MessageOutputProxy::MessageOutputProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, MessageOutputProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mLastMessage        (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamuserAction    (  )
    , mParammsgInput      (  )
{
    ; // do nothing
}

MessageOutputProxy::~MessageOutputProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * MessageOutputProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW MessageOutputNotificationEvent(data);
}

ServiceRequestEvent * MessageOutputProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW MessageOutputRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* MessageOutputProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW MessageOutputNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * MessageOutputProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW MessageOutputProxy::MessageOutputServiceAvailableEvent(consumer) );
}

void MessageOutputProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    MessageOutputResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void MessageOutputProxy::unregisterServiceListeners( void )
{
    MessageOutputResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    MessageOutputProxy::MessageOutputServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
void MessageOutputProxy::requestOutputMessage( const String & msgOutput, const DateTime & dateTime )
{
    static const NEMessageOutput::eMessageIDs msgId = NEMessageOutput::MSG_ID_requestOutputMessage;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << msgOutput;
    stream << dateTime;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void MessageOutputProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    MessageOutputResponseEvent * eventResp = RUNTIME_CAST( &eventElem, MessageOutputResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void MessageOutputProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    MessageOutputResponseEvent * eventResp = RUNTIME_CAST( &eventElem, MessageOutputResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_private_MessageOutputProxy_UpdateData);
void MessageOutputProxy::updateData( MessageOutputResponseEvent & eventElem, NEMessageOutput::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const MessageOutputResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEMessageOutput::MSG_ID_broadcastActionRequest:
        stream >> mParamuserAction;
        break;

    case NEMessageOutput::MSG_ID_broadcastInputMessage:
        stream >> mParammsgInput;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEMessageOutput::MSG_ID_LastMessage:
        stream >> mLastMessage;
        break;

    default:
        {
            TRACE_SCOPE(src_gen_private_MessageOutputProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void MessageOutputProxy::processResponse( MessageOutputResponseEvent & evenElem )
{
    NEMessageOutput::eMessageIDs respId  = static_cast<NEMessageOutput::eMessageIDs>(evenElem.getResponseId());
    NEService::eResultType resultType  = evenElem.getResult();

    bool dataValid  = false;
    bool setStates  = true;

    switch (resultType)
    {
    case NEService::RESULT_DATA_INVALID:
    case NEService::RESULT_INVALID:
        break;  // do nothing

    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_CANCELED:
        respId      = static_cast<NEMessageOutput::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEMessageOutput::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NEMessageOutput::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEMessageOutput::MSG_ID_NO_PROCEED;            
        break;

    case NEService::RESULT_OK:
    case NEService::RESULT_DATA_OK:
        dataValid   = true;
        break;

    default:
        setStates   = false;
        break;
    }

    if (dataValid == true)
        updateData(evenElem, respId);
    if (setStates == true)
        setState(static_cast<unsigned int>(respId), dataValid ? NEService::DATA_OK : NEService::DATA_INVALID);

    notifyListeners(static_cast<unsigned int>(respId), resultType, evenElem.getSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/MessageOutputProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 