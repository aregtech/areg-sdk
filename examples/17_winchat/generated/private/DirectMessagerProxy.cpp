//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/private/DirectMessagerProxy.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/private/DirectMessagerProxy.hpp"
#include "generated/private/DirectMessagerEvents.hpp"
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
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * DirectMessagerProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW DirectMessagerProxy(roleName, ownerThread);
}

DirectMessagerProxy * DirectMessagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<DirectMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectMessagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

DirectMessagerProxy * DirectMessagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::getEmptyString()*/ )
{
    return static_cast<DirectMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectMessagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectMessagerProxy::DirectMessagerProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
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
}

DirectMessagerProxy::~DirectMessagerProxy( void )
{
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
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_requestChatJoin;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeConnect;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void DirectMessagerProxy::requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_requestMessageSend;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << sender;
    stream << msgText;
    stream << timeSent;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
void DirectMessagerProxy::requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_requestMessageType;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << msgText;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
void DirectMessagerProxy::requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_requestChatLeave;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeLeave;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
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
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void DirectMessagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    DirectMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, DirectMessagerResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_private_DirectMessagerProxy_updateData);
void DirectMessagerProxy::updateData( DirectMessagerResponseEvent & eventElem, NEDirectMessager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const DirectMessagerResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEDirectMessager::eMessageIDs::MsgId_responseChatJoin:
        stream >> mParamsucceed;
        stream >> mParamlistParticipant;
        stream >> mParamtimeConnect;
        stream >> mParamtimeConnected;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent:
        stream >> mParamsender;
        stream >> mParammsgText;
        stream >> mParamtimeSent;
        break;

    case NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped:
        stream >> mParamparticipant;
        stream >> mParammsgText;
        break;

    case NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined:
        stream >> mParamparticipant;
        stream >> mParamtimeJoined;
        break;

    case NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft:
        stream >> mParamparticipant;
        stream >> mParamtimeLeft;
        break;

    case NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed:
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEDirectMessager::eMessageIDs::MsgId_ChatParticipants:
        stream >> mChatParticipants;
        break;

    default:
        {
            TRACE_SCOPE(generated_private_DirectMessagerProxy_updateData);
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
    case NEService::eResultType::DataInvalid:   // fall through
    case NEService::eResultType::RequestInvalid:
        break;  // do nothing

    case NEService::eResultType::RequestError:  // fall through
    case NEService::eResultType::RequestBusy:   // fall through
    case NEService::eResultType::RequestCanceled:
        respId      = static_cast<NEDirectMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEDirectMessager::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEDirectMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEDirectMessager::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::RequestOK:     // fall through
    case NEService::eResultType::DataOK:
        dataValid   = true;
        break;

    default:
        setStates   = false;
        break;
    }

    if (dataValid == true)
    {
        updateData(evenElem, respId);
    }

    if (setStates == true)
    {
        setState(static_cast<msg_id>(respId), dataValid ? NEService::eDataStateType::DataIsOK : NEService::eDataStateType::DataIsInvalid);
    }

    notifyListeners(static_cast<msg_id>(respId), resultType, evenElem.getSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
