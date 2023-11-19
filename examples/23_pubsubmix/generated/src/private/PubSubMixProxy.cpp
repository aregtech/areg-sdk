//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubMixProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:52:21 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/private/PubSubMixProxy.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/PubSubMixProxy.hpp"
#include "generated/src/private/PubSubMixEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PubSubMixProxy::PubSubMixServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(PubSubMixProxy::PubSubMixServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

PubSubMixProxy::PubSubMixServiceAvailableEvent::PubSubMixServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubMixProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * PubSubMixProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW PubSubMixProxy(roleName, ownerThread);
}

PubSubMixProxy * PubSubMixProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<PubSubMixProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPubSubMix::getInterfaceData()
                                                                      , connectListener
                                                                      , &PubSubMixProxy::_createProxy
                                                                      , ownerThread).get() );
}

PubSubMixProxy * PubSubMixProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<PubSubMixProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPubSubMix::getInterfaceData()
                                                                      , connectListener
                                                                      , &PubSubMixProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PubSubMixProxy::PubSubMixProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEPubSubMix::getInterfaceData(), ownerThread)

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

NotificationEvent * PubSubMixProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW PubSubMixNotificationEvent(data);
}

ServiceRequestEvent * PubSubMixProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW PubSubMixRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* PubSubMixProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW PubSubMixNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * PubSubMixProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PubSubMixResponseEvent(stream) );
}

RemoteResponseEvent * PubSubMixProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, const SequenceNumber &  seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PubSubMixResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * PubSubMixProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    ProxyBase::ServiceAvailableEvent* event = static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW PubSubMixProxy::PubSubMixServiceAvailableEvent(consumer) );
    if (event != nullptr)
    {
        if (mDispatcherThread.getId() != Thread::getCurrentThreadId())
        {
            event->setEventDelay(ProxyBase::MINIMAL_DELAY_TIME_MS);
        }
    }

    return event;
}

void PubSubMixProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    PubSubMixResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void PubSubMixProxy::unregisterServiceListeners( void )
{
    PubSubMixResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    PubSubMixProxy::PubSubMixServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
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

void PubSubMixProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    PubSubMixResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PubSubMixResponseEvent );
    if (eventResp != nullptr)
    {
        processResponse( *eventResp );
    }
    else
    {
        processProxyEvent( eventElem );
    }
}

void PubSubMixProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    PubSubMixResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PubSubMixResponseEvent );
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

DEF_TRACE_SCOPE(generated_src_private_PubSubMixProxy_updateData);
void PubSubMixProxy::updateData( PubSubMixResponseEvent & eventElem, NEPubSubMix::eMessageIDs respId )
{
    const IEInStream & stream { static_cast<const PubSubMixResponseEvent &>(eventElem).getData().getReadStream() };

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
    case NEPubSubMix::eMessageIDs::MsgId_StringOnChange:
        stream >> mStringOnChange;
        break;

    case NEPubSubMix::eMessageIDs::MsgId_IntegerAlways:
        stream >> mIntegerAlways;
        break;

    case NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState:
        stream >> mServiceProviderState;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_PubSubMixProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

void PubSubMixProxy::processResponse( PubSubMixResponseEvent & evenElem )
{
    NEPubSubMix::eMessageIDs respId  { static_cast<NEPubSubMix::eMessageIDs>(evenElem.getResponseId()) };
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
        respId      = static_cast<NEPubSubMix::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEPubSubMix::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEPubSubMix::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEPubSubMix::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/PubSubMixProxy.cpp file
//////////////////////////////////////////////////////////////////////////
