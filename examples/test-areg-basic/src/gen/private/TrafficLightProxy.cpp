//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/TrafficLightProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/TrafficLightProxy.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/private/TrafficLightProxy.hpp"
#include "src/gen/private/TrafficLightEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// TrafficLightProxy::TrafficLightServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(TrafficLightProxy::TrafficLightServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

TrafficLightProxy::TrafficLightServiceAvailableEvent::TrafficLightServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

TrafficLightProxy::TrafficLightServiceAvailableEvent::~TrafficLightServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & TrafficLightProxy::_createInterfaceData( void )
{
    return NETrafficLight::createInterfaceData( );
}

ProxyBase * TrafficLightProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW TrafficLightProxy(roleName, ownerThread);
}

TrafficLightProxy * TrafficLightProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<TrafficLightProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NETrafficLight::createInterfaceData()
                                                                      , connectListener
                                                                      , &TrafficLightProxy::_createProxy
                                                                      , ownerThread) );
}

TrafficLightProxy * TrafficLightProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<TrafficLightProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NETrafficLight::createInterfaceData()
                                                                      , connectListener
                                                                      , &TrafficLightProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
TrafficLightProxy::TrafficLightProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, TrafficLightProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mTrafficLight       (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamlightColor    (  )
{
    ; // do nothing
}

TrafficLightProxy::~TrafficLightProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * TrafficLightProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW TrafficLightNotificationEvent(data);
}

ServiceRequestEvent * TrafficLightProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW TrafficLightRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* TrafficLightProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW TrafficLightNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * TrafficLightProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW TrafficLightProxy::TrafficLightServiceAvailableEvent(consumer) );
}

void TrafficLightProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    TrafficLightResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void TrafficLightProxy::unregisterServiceListeners( void )
{
    TrafficLightResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    TrafficLightProxy::TrafficLightServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int TrafficLightProxy::requestChangeLight( IENotificationEventConsumer & caller, NETrafficLight::eLight lightColor, bool holdon )
{
    static const NETrafficLight::eMessageIDs msgId = NETrafficLight::MSG_ID_requestChangeLight;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << lightColor;
    stream << holdon;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void TrafficLightProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    TrafficLightResponseEvent * eventResp = RUNTIME_CAST( &eventElem, TrafficLightResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void TrafficLightProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    TrafficLightResponseEvent * eventResp = RUNTIME_CAST( &eventElem, TrafficLightResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_private_TrafficLightProxy_UpdateData);
void TrafficLightProxy::updateData( TrafficLightResponseEvent & eventElem, NETrafficLight::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const TrafficLightResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NETrafficLight::MSG_ID_responseChangeLight:
        stream >> mParamlightColor;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NETrafficLight::MSG_ID_TrafficLight:
        stream >> mTrafficLight;
        break;

    default:
        {
            TRACE_SCOPE(src_gen_private_TrafficLightProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void TrafficLightProxy::processResponse( TrafficLightResponseEvent & evenElem )
{
    NETrafficLight::eMessageIDs respId  = static_cast<NETrafficLight::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NETrafficLight::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NETrafficLight::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NETrafficLight::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NETrafficLight::MSG_ID_NO_PROCEED;            
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
// End generate src/gen/private/TrafficLightProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 