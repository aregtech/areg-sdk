//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PowerManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PowerManager.
 * Generated at     15.08.2021  00:03:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/PowerManagerProxy.cpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager Service Interface Proxy class implementation.
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
    ; // do nothing
}

PowerManagerProxy::PowerManagerServiceAvailableEvent::~PowerManagerServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * PowerManagerProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW PowerManagerProxy(roleName, ownerThread);
}

PowerManagerProxy * PowerManagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<PowerManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPowerManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &PowerManagerProxy::_createProxy
                                                                      , ownerThread) );
}

PowerManagerProxy * PowerManagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<PowerManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPowerManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &PowerManagerProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PowerManagerProxy::PowerManagerProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
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
    ; // do nothing
}

PowerManagerProxy::~PowerManagerProxy( void )
{
    ; // do nothing
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
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::MSG_ID_requestPowerOn;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, NULL );
}
void PowerManagerProxy::requestPowerOff( void )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::MSG_ID_requestPowerOff;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, NULL );
}
unsigned int PowerManagerProxy::requestStartTrafficLight( IENotificationEventConsumer & caller )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::MSG_ID_requestStartTrafficLight;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, &caller );
    return mSequenceCount;
}
    
unsigned int PowerManagerProxy::requestStopTrafficLight( IENotificationEventConsumer & caller )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::MSG_ID_requestStopTrafficLight;
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
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void PowerManagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    PowerManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PowerManagerResponseEvent );
    if (eventResp != NULL)
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
    case NEPowerManager::MSG_ID_responseStartTrafficLight:
        stream >> mParamSuccess;
        break;

    case NEPowerManager::MSG_ID_responseStopTrafficLight:
        stream >> mParamSuccess;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEPowerManager::MSG_ID_LightsPowerState:
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
    case NEService::RESULT_DATA_INVALID:
    case NEService::RESULT_INVALID:
        break;  // do nothing

    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_CANCELED:
        respId      = static_cast<NEPowerManager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEPowerManager::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NEPowerManager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEPowerManager::MSG_ID_NO_PROCEED;            
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
// End generate generated/src/private/PowerManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 