//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/DirectMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     23.05.2021  00:18:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/DirectMessagerProxy.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/private/DirectMessagerProxy.hpp"
#include "shared/generated/private/DirectMessagerEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// DirectMessagerProxy::DirectMessagerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(DirectMessagerProxy::DirectMessagerServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

DirectMessagerProxy::DirectMessagerServiceAvailableEvent::DirectMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

DirectMessagerProxy::DirectMessagerServiceAvailableEvent::~DirectMessagerServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * DirectMessagerProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW DirectMessagerProxy(roleName, ownerThread);
}

DirectMessagerProxy * DirectMessagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<DirectMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectMessagerProxy::_createProxy
                                                                      , ownerThread) );
}

DirectMessagerProxy * DirectMessagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<DirectMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectMessagerProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectMessagerProxy::DirectMessagerProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NEDirectMessager::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mChatParticipants       (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamsucceed           (  )
    , mParamlistParticipant   (  )
    , mParamtimeConnect       (  )
    , mParamtimeConnected     (  )
    , mParamsender            (  )
    , mParammsgText           (  )
    , mParamtimeSent          (  )
    , mParamparticipant       (  )
    , mParamtimeJoined        (  )
    , mParamtimeLeft          (  )
{
    ; // do nothing
}

DirectMessagerProxy::~DirectMessagerProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * DirectMessagerProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW DirectMessagerNotificationEvent(data);
}

ServiceRequestEvent * DirectMessagerProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW DirectMessagerRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* DirectMessagerProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW DirectMessagerNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * DirectMessagerProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW DirectMessagerResponseEvent(stream) );
}

RemoteResponseEvent * DirectMessagerProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW DirectMessagerResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * DirectMessagerProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW DirectMessagerProxy::DirectMessagerServiceAvailableEvent(consumer) );
}

void DirectMessagerProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    DirectMessagerResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void DirectMessagerProxy::unregisterServiceListeners( void )
{
    DirectMessagerResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    DirectMessagerProxy::DirectMessagerServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int DirectMessagerProxy::requestChatJoin( IENotificationEventConsumer & caller, const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_requestChatJoin;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeConnect;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void DirectMessagerProxy::requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_requestMessageSend;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << sender;
    stream << msgText;
    stream << timeSent;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
void DirectMessagerProxy::requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_requestMessageType;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << msgText;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
void DirectMessagerProxy::requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_requestChatLeave;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeLeave;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void DirectMessagerProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    DirectMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, DirectMessagerResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void DirectMessagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    DirectMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, DirectMessagerResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_private_DirectMessagerProxy_UpdateData);
void DirectMessagerProxy::updateData( DirectMessagerResponseEvent & eventElem, NEDirectMessager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const DirectMessagerResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEDirectMessager::MSG_ID_responseChatJoin:
        stream >> mParamsucceed;
        stream >> mParamlistParticipant;
        stream >> mParamtimeConnect;
        stream >> mParamtimeConnected;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEDirectMessager::MSG_ID_broadcastMessageSent:
        stream >> mParamsender;
        stream >> mParammsgText;
        stream >> mParamtimeSent;
        break;

    case NEDirectMessager::MSG_ID_broadcastMessageTyped:
        stream >> mParamparticipant;
        stream >> mParammsgText;
        break;

    case NEDirectMessager::MSG_ID_broadcastParticipantJoined:
        stream >> mParamparticipant;
        stream >> mParamtimeJoined;
        break;

    case NEDirectMessager::MSG_ID_broadcastParticipantLeft:
        stream >> mParamparticipant;
        stream >> mParamtimeLeft;
        break;

    case NEDirectMessager::MSG_ID_broadcastChatClosed:
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEDirectMessager::MSG_ID_ChatParticipants:
        stream >> mChatParticipants;
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_private_DirectMessagerProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void DirectMessagerProxy::processResponse( DirectMessagerResponseEvent & evenElem )
{
    NEDirectMessager::eMessageIDs respId  = static_cast<NEDirectMessager::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEDirectMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectMessager::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NEDirectMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectMessager::MSG_ID_NO_PROCEED;            
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
// End generate shared/generated/private/DirectMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 