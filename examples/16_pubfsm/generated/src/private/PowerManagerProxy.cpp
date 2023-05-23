//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PowerManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PowerManager.
 *
 * \file            generated/src/private/PowerManagerProxy.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/PowerManagerProxy.hpp"
#include "generated/src/private/PowerManagerEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PowerManagerProxy::PowerManagerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(PowerManagerProxy::PowerManagerServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

PowerManagerProxy::PowerManagerServiceAvailableEvent::PowerManagerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * PowerManagerProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW PowerManagerProxy(roleName, ownerThread);
}

PowerManagerProxy * PowerManagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<PowerManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPowerManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &PowerManagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

PowerManagerProxy * PowerManagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::getEmptyString()*/ )
{
    return static_cast<PowerManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPowerManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &PowerManagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PowerManagerProxy::PowerManagerProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEPowerManager::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mLightsPowerState   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamSuccess       (  )
{
}

PowerManagerProxy::~PowerManagerProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * PowerManagerProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW PowerManagerNotificationEvent(data);
}

ServiceRequestEvent * PowerManagerProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW PowerManagerRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* PowerManagerProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW PowerManagerNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * PowerManagerProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW PowerManagerProxy::PowerManagerServiceAvailableEvent(consumer) );
}

void PowerManagerProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    PowerManagerResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void PowerManagerProxy::unregisterServiceListeners( void )
{
    PowerManagerResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    PowerManagerProxy::PowerManagerServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

void PowerManagerProxy::requestPowerOn( void )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::eMessageIDs::MsgId_requestPowerOn;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, nullptr );
}
void PowerManagerProxy::requestPowerOff( void )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::eMessageIDs::MsgId_requestPowerOff;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, nullptr );
}
unsigned int PowerManagerProxy::requestStartTrafficLight( IENotificationEventConsumer & caller )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::eMessageIDs::MsgId_requestStartTrafficLight;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, &caller );
    return mSequenceCount;
}
    
unsigned int PowerManagerProxy::requestStopTrafficLight( IENotificationEventConsumer & caller )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::eMessageIDs::MsgId_requestStopTrafficLight;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, &caller );
    return mSequenceCount;
}
    
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void PowerManagerProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    PowerManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PowerManagerResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void PowerManagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    PowerManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PowerManagerResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_PowerManagerProxy_updateData);
void PowerManagerProxy::updateData( PowerManagerResponseEvent & eventElem, NEPowerManager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const PowerManagerResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight:
        stream >> mParamSuccess;
        break;

    case NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight:
        stream >> mParamSuccess;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEPowerManager::eMessageIDs::MsgId_LightsPowerState:
        stream >> mLightsPowerState;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_PowerManagerProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void PowerManagerProxy::processResponse( PowerManagerResponseEvent & evenElem )
{
    NEPowerManager::eMessageIDs respId  = static_cast<NEPowerManager::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEPowerManager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEPowerManager::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEPowerManager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEPowerManager::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/PowerManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
