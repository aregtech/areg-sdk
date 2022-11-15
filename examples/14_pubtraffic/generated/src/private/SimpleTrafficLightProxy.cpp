//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficLightProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:32 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 *
 * \file            generated/src/private/SimpleTrafficLightProxy.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/SimpleTrafficLightProxy.hpp"
#include "generated/src/private/SimpleTrafficLightEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightProxy::SimpleTrafficLightServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(SimpleTrafficLightProxy::SimpleTrafficLightServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

SimpleTrafficLightProxy::SimpleTrafficLightServiceAvailableEvent::SimpleTrafficLightServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * SimpleTrafficLightProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW SimpleTrafficLightProxy(roleName, ownerThread);
}

SimpleTrafficLightProxy * SimpleTrafficLightProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<SimpleTrafficLightProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESimpleTrafficLight::getInterfaceData()
                                                                      , connectListener
                                                                      , &SimpleTrafficLightProxy::_createProxy
                                                                      , ownerThread) );
}

SimpleTrafficLightProxy * SimpleTrafficLightProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<SimpleTrafficLightProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESimpleTrafficLight::getInterfaceData()
                                                                      , connectListener
                                                                      , &SimpleTrafficLightProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SimpleTrafficLightProxy::SimpleTrafficLightProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NESimpleTrafficLight::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mSouthNorth         (  )
    , mEastWest           (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamSouthNorth    (  )
    , mParamEastWest      (  )
{
}

SimpleTrafficLightProxy::~SimpleTrafficLightProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * SimpleTrafficLightProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW SimpleTrafficLightNotificationEvent(data);
}

ServiceRequestEvent * SimpleTrafficLightProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW SimpleTrafficLightRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* SimpleTrafficLightProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW SimpleTrafficLightNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * SimpleTrafficLightProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW SimpleTrafficLightResponseEvent(stream) );
}

RemoteResponseEvent * SimpleTrafficLightProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW SimpleTrafficLightResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * SimpleTrafficLightProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW SimpleTrafficLightProxy::SimpleTrafficLightServiceAvailableEvent(consumer) );
}

void SimpleTrafficLightProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    SimpleTrafficLightResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void SimpleTrafficLightProxy::unregisterServiceListeners( void )
{
    SimpleTrafficLightResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    SimpleTrafficLightProxy::SimpleTrafficLightServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
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

void SimpleTrafficLightProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    SimpleTrafficLightResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SimpleTrafficLightResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void SimpleTrafficLightProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    SimpleTrafficLightResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SimpleTrafficLightResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_SimpleTrafficLightProxy_updateData);
void SimpleTrafficLightProxy::updateData( SimpleTrafficLightResponseEvent & eventElem, NESimpleTrafficLight::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const SimpleTrafficLightResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NESimpleTrafficLight::eMessageIDs::MsgId_broadcastLightChanged:
        stream >> mParamSouthNorth;
        stream >> mParamEastWest;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth:
        stream >> mSouthNorth;
        break;

    case NESimpleTrafficLight::eMessageIDs::MsgId_EastWest:
        stream >> mEastWest;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_SimpleTrafficLightProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void SimpleTrafficLightProxy::processResponse( SimpleTrafficLightResponseEvent & evenElem )
{
    NESimpleTrafficLight::eMessageIDs respId  = static_cast<NESimpleTrafficLight::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NESimpleTrafficLight::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NESimpleTrafficLight::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NESimpleTrafficLight::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NESimpleTrafficLight::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/SimpleTrafficLightProxy.cpp file
//////////////////////////////////////////////////////////////////////////
