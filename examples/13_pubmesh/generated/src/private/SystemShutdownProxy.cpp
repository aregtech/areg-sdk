//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SystemShutdownProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     23.07.2023  03:04:29 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/private/SystemShutdownProxy.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface Proxy class implementation.
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
}

//////////////////////////////////////////////////////////////////////////
// SystemShutdownProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * SystemShutdownProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW SystemShutdownProxy(roleName, ownerThread);
}

SystemShutdownProxy * SystemShutdownProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<SystemShutdownProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESystemShutdown::getInterfaceData()
                                                                      , connectListener
                                                                      , &SystemShutdownProxy::_createProxy
                                                                      , ownerThread).get() );
}

SystemShutdownProxy * SystemShutdownProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<SystemShutdownProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESystemShutdown::getInterfaceData()
                                                                      , connectListener
                                                                      , &SystemShutdownProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SystemShutdownProxy::SystemShutdownProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NESystemShutdown::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mServiceState   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
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

void SystemShutdownProxy::requestSystemShutdown( void )
{
    static const NESystemShutdown::eMessageIDs msgId = NESystemShutdown::eMessageIDs::MsgId_requestSystemShutdown;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, nullptr );
}
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void SystemShutdownProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    SystemShutdownResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SystemShutdownResponseEvent );
    if (eventResp != nullptr)
    {
        processResponse( *eventResp );
    }
    else
    {
        processProxyEvent( eventElem );
    }
}

void SystemShutdownProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    SystemShutdownResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SystemShutdownResponseEvent );
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

DEF_TRACE_SCOPE(generated_src_private_SystemShutdownProxy_updateData);
void SystemShutdownProxy::updateData( SystemShutdownResponseEvent & eventElem, NESystemShutdown::eMessageIDs respId )
{
    const IEInStream & stream { static_cast<const SystemShutdownResponseEvent &>(eventElem).getData().getReadStream() };

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
    case NESystemShutdown::eMessageIDs::MsgId_ServiceState:
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
    NESystemShutdown::eMessageIDs respId  { static_cast<NESystemShutdown::eMessageIDs>(evenElem.getResponseId()) };
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
        respId      = static_cast<NESystemShutdown::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NESystemShutdown::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NESystemShutdown::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NESystemShutdown::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/SystemShutdownProxy.cpp file
//////////////////////////////////////////////////////////////////////////
