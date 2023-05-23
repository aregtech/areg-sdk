//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/TrafficControllerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/private/TrafficControllerProxy.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/TrafficControllerProxy.hpp"
#include "generated/src/private/TrafficControllerEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// TrafficControllerProxy::TrafficControllerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(TrafficControllerProxy::TrafficControllerServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

TrafficControllerProxy::TrafficControllerServiceAvailableEvent::TrafficControllerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// TrafficControllerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * TrafficControllerProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW TrafficControllerProxy(roleName, ownerThread);
}

TrafficControllerProxy * TrafficControllerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<TrafficControllerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NETrafficController::getInterfaceData()
                                                                      , connectListener
                                                                      , &TrafficControllerProxy::_createProxy
                                                                      , ownerThread).get() );
}

TrafficControllerProxy * TrafficControllerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::getEmptyString()*/ )
{
    return static_cast<TrafficControllerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NETrafficController::getInterfaceData()
                                                                      , connectListener
                                                                      , &TrafficControllerProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
TrafficControllerProxy::TrafficControllerProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NETrafficController::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mTrafficSouthNorth      (  )
    , mTrafficEastWest        (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamLightVehicle      (  )
    , mParamLightPedestrian   (  )
{
}

TrafficControllerProxy::~TrafficControllerProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * TrafficControllerProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW TrafficControllerNotificationEvent(data);
}

ServiceRequestEvent * TrafficControllerProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW TrafficControllerRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* TrafficControllerProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW TrafficControllerNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * TrafficControllerProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW TrafficControllerResponseEvent(stream) );
}

RemoteResponseEvent * TrafficControllerProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW TrafficControllerResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * TrafficControllerProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW TrafficControllerProxy::TrafficControllerServiceAvailableEvent(consumer) );
}

void TrafficControllerProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    TrafficControllerResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void TrafficControllerProxy::unregisterServiceListeners( void )
{
    TrafficControllerResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    TrafficControllerProxy::TrafficControllerServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
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

void TrafficControllerProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    TrafficControllerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, TrafficControllerResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void TrafficControllerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    TrafficControllerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, TrafficControllerResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_TrafficControllerProxy_updateData);
void TrafficControllerProxy::updateData( TrafficControllerResponseEvent & eventElem, NETrafficController::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const TrafficControllerResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth:
        stream >> mParamLightVehicle;
        stream >> mParamLightPedestrian;
        break;

    case NETrafficController::eMessageIDs::MsgId_broadcastEastWest:
        stream >> mParamLightVehicle;
        stream >> mParamLightPedestrian;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth:
        stream >> mTrafficSouthNorth;
        break;

    case NETrafficController::eMessageIDs::MsgId_TrafficEastWest:
        stream >> mTrafficEastWest;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_TrafficControllerProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void TrafficControllerProxy::processResponse( TrafficControllerResponseEvent & evenElem )
{
    NETrafficController::eMessageIDs respId  = static_cast<NETrafficController::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NETrafficController::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NETrafficController::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NETrafficController::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NETrafficController::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/TrafficControllerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
