//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SystemShutdownProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 * Generated at     11.08.2021  13:17:34 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/SystemShutdownProxy.cpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/SystemShutdownProxy.hpp"
#include "generated/src/private/SystemShutdownEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SystemShutdownProxy::SystemShutdownServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(SystemShutdownProxy::SystemShutdownServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

SystemShutdownProxy::SystemShutdownServiceAvailableEvent::SystemShutdownServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

SystemShutdownProxy::SystemShutdownServiceAvailableEvent::~SystemShutdownServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemShutdownProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * SystemShutdownProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW SystemShutdownProxy(roleName, ownerThread);
}

SystemShutdownProxy * SystemShutdownProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<SystemShutdownProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESystemShutdown::getInterfaceData()
                                                                      , connectListener
                                                                      , &SystemShutdownProxy::_createProxy
                                                                      , ownerThread) );
}

SystemShutdownProxy * SystemShutdownProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<SystemShutdownProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESystemShutdown::getInterfaceData()
                                                                      , connectListener
                                                                      , &SystemShutdownProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SystemShutdownProxy::SystemShutdownProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NESystemShutdown::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mServiceState   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
    ; // do nothing
}

SystemShutdownProxy::~SystemShutdownProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * SystemShutdownProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW SystemShutdownNotificationEvent(data);
}

ServiceRequestEvent * SystemShutdownProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW SystemShutdownRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* SystemShutdownProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW SystemShutdownNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * SystemShutdownProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW SystemShutdownResponseEvent(stream) );
}

RemoteResponseEvent * SystemShutdownProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW SystemShutdownResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * SystemShutdownProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW SystemShutdownProxy::SystemShutdownServiceAvailableEvent(consumer) );
}

void SystemShutdownProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    SystemShutdownResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void SystemShutdownProxy::unregisterServiceListeners( void )
{
    SystemShutdownResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    SystemShutdownProxy::SystemShutdownServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
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

void SystemShutdownProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    SystemShutdownResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SystemShutdownResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void SystemShutdownProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    SystemShutdownResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SystemShutdownResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_SystemShutdownProxy_updateData);
void SystemShutdownProxy::updateData( SystemShutdownResponseEvent & eventElem, NESystemShutdown::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const SystemShutdownResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NESystemShutdown::MSG_ID_broadcastServiceUnavailable:
        break;

    case NESystemShutdown::MSG_ID_broadcastServiceShutdown:
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NESystemShutdown::MSG_ID_ServiceState:
        stream >> mServiceState;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_SystemShutdownProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void SystemShutdownProxy::processResponse( SystemShutdownResponseEvent & evenElem )
{
    NESystemShutdown::eMessageIDs respId  = static_cast<NESystemShutdown::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NESystemShutdown::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NESystemShutdown::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NESystemShutdown::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NESystemShutdown::MSG_ID_NO_PROCEED;            
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
// End generate generated/src/private/SystemShutdownProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 