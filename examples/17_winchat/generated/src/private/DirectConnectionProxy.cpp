//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/DirectConnectionProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/src/private/DirectConnectionProxy.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/DirectConnectionProxy.hpp"
#include "generated/src/private/DirectConnectionEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// DirectConnectionProxy::DirectConnectionServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(DirectConnectionProxy::DirectConnectionServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

DirectConnectionProxy::DirectConnectionServiceAvailableEvent::DirectConnectionServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * DirectConnectionProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW DirectConnectionProxy(roleName, ownerThread);
}

DirectConnectionProxy * DirectConnectionProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<DirectConnectionProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectConnection::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectConnectionProxy::_createProxy
                                                                      , ownerThread).get() );
}

DirectConnectionProxy * DirectConnectionProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<DirectConnectionProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectConnection::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectConnectionProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectConnectionProxy::DirectConnectionProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEDirectConnection::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mInitiatedConnections   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamsucceeded         (  )
    , mParamtarget            (  )
    , mParaminitiator         (  )
    , mParamlistParticipants  (  )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * DirectConnectionProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW DirectConnectionNotificationEvent(data);
}

ServiceRequestEvent * DirectConnectionProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW DirectConnectionRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* DirectConnectionProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW DirectConnectionNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * DirectConnectionProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW DirectConnectionResponseEvent(stream) );
}

RemoteResponseEvent * DirectConnectionProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, const SequenceNumber &  seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW DirectConnectionResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * DirectConnectionProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW DirectConnectionProxy::DirectConnectionServiceAvailableEvent(consumer) );
}

void DirectConnectionProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    DirectConnectionResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void DirectConnectionProxy::unregisterServiceListeners( void )
{
    DirectConnectionResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    DirectConnectionProxy::DirectConnectionServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

const SequenceNumber &  DirectConnectionProxy::requestConnectoinSetup( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::eMessageIDs::MsgId_requestConnectoinSetup;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}

const SequenceNumber &  DirectConnectionProxy::requestAddParticipant( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::eMessageIDs::MsgId_requestAddParticipant;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}

const SequenceNumber &  DirectConnectionProxy::requestRemoveParticipant( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::eMessageIDs::MsgId_requestRemoveParticipant;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}

void DirectConnectionProxy::requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::eMessageIDs::MsgId_requestCloseConnection;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void DirectConnectionProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    DirectConnectionResponseEvent * eventResp = RUNTIME_CAST( &eventElem, DirectConnectionResponseEvent );
    if (eventResp != nullptr)
    {
        processResponse( *eventResp );
    }
    else
    {
        processProxyEvent( eventElem );
    }
}

void DirectConnectionProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    DirectConnectionResponseEvent * eventResp = RUNTIME_CAST( &eventElem, DirectConnectionResponseEvent );
    if (eventResp != nullptr)
    {
        processResponse( *eventResp );
    }
    else
    {
        processProxyEvent( eventElem );
    }
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_DirectConnectionProxy_updateData);
void DirectConnectionProxy::updateData( DirectConnectionResponseEvent & eventElem, NEDirectConnection::eMessageIDs respId )
{
    const IEInStream & stream { static_cast<const DirectConnectionResponseEvent &>(eventElem).getData().getReadStream() };

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup:
        stream >> mParamsucceeded;
        stream >> mParamtarget;
        stream >> mParaminitiator;
        stream >> mParamlistParticipants;
        break;

    case NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant:
        stream >> mParamsucceeded;
        stream >> mParamlistParticipants;
        break;

    case NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant:
        stream >> mParamsucceeded;
        stream >> mParamlistParticipants;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections:
        stream >> mInitiatedConnections;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_DirectConnectionProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

void DirectConnectionProxy::processResponse( DirectConnectionResponseEvent & evenElem )
{
    NEDirectConnection::eMessageIDs respId  { static_cast<NEDirectConnection::eMessageIDs>(evenElem.getResponseId()) };
    NEService::eResultType resultType   { evenElem.getResult() };

    bool dataValid { false };
    bool setStates { true  };

    switch (resultType)
    {
    case NEService::eResultType::DataInvalid:   // fall through
    case NEService::eResultType::RequestInvalid:
        break;  // do nothing

    case NEService::eResultType::RequestError:  // fall through
    case NEService::eResultType::RequestBusy:   // fall through
    case NEService::eResultType::RequestCanceled:
        respId      = static_cast<NEDirectConnection::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEDirectConnection::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEDirectConnection::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEDirectConnection::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::RequestOK:     // fall through
    case NEService::eResultType::DataOK:
        dataValid   = true;
        break;

    default:
        setStates   = false;
        break;
    }

    if ( dataValid )
    {
        updateData(evenElem, respId);
    }

    if ( setStates )
    {
        setState(static_cast<msg_id>(respId), dataValid ? NEService::eDataStateType::DataIsOK : NEService::eDataStateType::DataIsInvalid);
    }

    notifyListeners(static_cast<msg_id>(respId), resultType, evenElem.getSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/DirectConnectionProxy.cpp file
//////////////////////////////////////////////////////////////////////////
