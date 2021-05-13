//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/SystemProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     12.05.2021  16:41:13 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/SystemProxy.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/private/SystemProxy.hpp"
#include "src/gen/private/SystemEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SystemProxy::SystemServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(SystemProxy::SystemServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

SystemProxy::SystemServiceAvailableEvent::SystemServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

SystemProxy::SystemServiceAvailableEvent::~SystemServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & SystemProxy::_createInterfaceData( void )
{
    return NESystem::createInterfaceData( );
}

ProxyBase * SystemProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW SystemProxy(roleName, ownerThread);
}

SystemProxy * SystemProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<SystemProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESystem::createInterfaceData()
                                                                      , connectListener
                                                                      , &SystemProxy::_createProxy
                                                                      , ownerThread) );
}

SystemProxy * SystemProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<SystemProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESystem::createInterfaceData()
                                                                      , connectListener
                                                                      , &SystemProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SystemProxy::SystemProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, SystemProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mSystemState    (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
    ; // do nothing
}

SystemProxy::~SystemProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * SystemProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW SystemNotificationEvent(data);
}

ServiceRequestEvent * SystemProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW SystemRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* SystemProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW SystemNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * SystemProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW SystemProxy::SystemServiceAvailableEvent(consumer) );
}

void SystemProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    SystemResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void SystemProxy::unregisterServiceListeners( void )
{
    SystemResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    SystemProxy::SystemServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
void SystemProxy::requestPowerOff( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_requestPowerOff;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, NULL );
}
void SystemProxy::requestPowerOn( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_requestPowerOn;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, NULL );
}
void SystemProxy::requestShutdown( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_requestShutdown;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, NULL );
}
void SystemProxy::requestStart( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_requestStart;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void SystemProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    SystemResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SystemResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void SystemProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    SystemResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SystemResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_private_SystemProxy_UpdateData);
void SystemProxy::updateData( SystemResponseEvent & eventElem, NESystem::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const SystemResponseEvent &>(eventElem).getData().getReadStream();

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
    case NESystem::MSG_ID_SystemState:
        stream >> mSystemState;
        break;

    default:
        {
            TRACE_SCOPE(src_gen_private_SystemProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void SystemProxy::processResponse( SystemResponseEvent & evenElem )
{
    NESystem::eMessageIDs respId  = static_cast<NESystem::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NESystem::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NESystem::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NESystem::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NESystem::MSG_ID_NO_PROCEED;            
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
// End generate src/gen/private/SystemProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 