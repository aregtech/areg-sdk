//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/private/PubSubProxy.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/PubSubProxy.hpp"
#include "generated/src/private/PubSubEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PubSubProxy::PubSubServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(PubSubProxy::PubSubServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

PubSubProxy::PubSubServiceAvailableEvent::PubSubServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * PubSubProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW PubSubProxy(roleName, ownerThread);
}

PubSubProxy * PubSubProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<PubSubProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPubSub::getInterfaceData()
                                                                      , connectListener
                                                                      , &PubSubProxy::_createProxy
                                                                      , ownerThread).get() );
}

PubSubProxy * PubSubProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<PubSubProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPubSub::getInterfaceData()
                                                                      , connectListener
                                                                      , &PubSubProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PubSubProxy::PubSubProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEPubSub::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mStringOnChange         (  )
    , mIntegerAlways          (  )
    , mServiceProviderState   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * PubSubProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW PubSubNotificationEvent(data);
}

ServiceRequestEvent * PubSubProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW PubSubRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* PubSubProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW PubSubNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * PubSubProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PubSubResponseEvent(stream) );
}

RemoteResponseEvent * PubSubProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, const SequenceNumber &  seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PubSubResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * PubSubProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW PubSubProxy::PubSubServiceAvailableEvent(consumer) );
}

void PubSubProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    PubSubResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void PubSubProxy::unregisterServiceListeners( void )
{
    PubSubResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    PubSubProxy::PubSubServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void PubSubProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    PubSubResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PubSubResponseEvent );
    if (eventResp != nullptr)
    {
        processResponse( *eventResp );
    }
    else
    {
        processProxyEvent( eventElem );
    }
}

void PubSubProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    PubSubResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PubSubResponseEvent );
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

DEF_TRACE_SCOPE(generated_src_private_PubSubProxy_updateData);
void PubSubProxy::updateData( PubSubResponseEvent & eventElem, NEPubSub::eMessageIDs respId )
{
    const IEInStream & stream { static_cast<const PubSubResponseEvent &>(eventElem).getData().getReadStream() };

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEPubSub::eMessageIDs::MsgId_StringOnChange:
        stream >> mStringOnChange;
        break;

    case NEPubSub::eMessageIDs::MsgId_IntegerAlways:
        stream >> mIntegerAlways;
        break;

    case NEPubSub::eMessageIDs::MsgId_ServiceProviderState:
        stream >> mServiceProviderState;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_PubSubProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

void PubSubProxy::processResponse( PubSubResponseEvent & evenElem )
{
    NEPubSub::eMessageIDs respId  { static_cast<NEPubSub::eMessageIDs>(evenElem.getResponseId()) };
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
        respId      = static_cast<NEPubSub::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEPubSub::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEPubSub::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEPubSub::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/PubSubProxy.cpp file
//////////////////////////////////////////////////////////////////////////
