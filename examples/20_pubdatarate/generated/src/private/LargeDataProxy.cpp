//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LargeDataProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:34:01 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LargeData.
 *
 * \file            generated/src/private/LargeDataProxy.hpp
 * \ingroup         LargeData Service Interface
 * \brief           This is an automatic generated code of LargeData
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/LargeDataProxy.hpp"
#include "generated/src/private/LargeDataEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// LargeDataProxy::LargeDataServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(LargeDataProxy::LargeDataServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

LargeDataProxy::LargeDataServiceAvailableEvent::LargeDataServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// LargeDataProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * LargeDataProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW LargeDataProxy(roleName, ownerThread);
}

LargeDataProxy * LargeDataProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<LargeDataProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NELargeData::getInterfaceData()
                                                                      , connectListener
                                                                      , &LargeDataProxy::_createProxy
                                                                      , ownerThread).get() );
}

LargeDataProxy * LargeDataProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<LargeDataProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NELargeData::getInterfaceData()
                                                                      , connectListener
                                                                      , &LargeDataProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
LargeDataProxy::LargeDataProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NELargeData::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamimageBlock    (  )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * LargeDataProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW LargeDataNotificationEvent(data);
}

ServiceRequestEvent * LargeDataProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW LargeDataRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* LargeDataProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW LargeDataNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * LargeDataProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW LargeDataResponseEvent(stream) );
}

RemoteResponseEvent * LargeDataProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, SequenceNumber seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW LargeDataResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * LargeDataProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW LargeDataProxy::LargeDataServiceAvailableEvent(consumer) );
}

void LargeDataProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    LargeDataResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void LargeDataProxy::unregisterServiceListeners( void )
{
    LargeDataResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    LargeDataProxy::LargeDataServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
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

void LargeDataProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    LargeDataResponseEvent * eventResp = RUNTIME_CAST( &eventElem, LargeDataResponseEvent );
    if (eventResp != nullptr)
    {
        processResponse( *eventResp );
    }
    else
    {
        processProxyEvent( eventElem );
    }
}

void LargeDataProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    LargeDataResponseEvent * eventResp = RUNTIME_CAST( &eventElem, LargeDataResponseEvent );
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

DEF_TRACE_SCOPE(generated_src_private_LargeDataProxy_updateData);
void LargeDataProxy::updateData( LargeDataResponseEvent & eventElem, NELargeData::eMessageIDs respId )
{
    const IEInStream & stream { static_cast<const LargeDataResponseEvent &>(eventElem).getData().getReadStream() };

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NELargeData::eMessageIDs::MsgId_broadcastImageBlockAcquired:
        stream >> mParamimageBlock;
        break;

    case NELargeData::eMessageIDs::MsgId_broadcastServiceStopping:
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(generated_src_private_LargeDataProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

void LargeDataProxy::processResponse( LargeDataResponseEvent & evenElem )
{
    NELargeData::eMessageIDs respId  { static_cast<NELargeData::eMessageIDs>(evenElem.getResponseId()) };
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
        respId      = static_cast<NELargeData::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NELargeData::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NELargeData::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NELargeData::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/LargeDataProxy.cpp file
//////////////////////////////////////////////////////////////////////////
