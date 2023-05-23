//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWatchdogProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:31 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWatchdog.
 *
 * \file            generated/src/private/HelloWatchdogProxy.hpp
 * \ingroup         HelloWatchdog Service Interface
 * \brief           This is an automatic generated code of HelloWatchdog
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/HelloWatchdogProxy.hpp"
#include "generated/src/private/HelloWatchdogEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogProxy::HelloWatchdogServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(HelloWatchdogProxy::HelloWatchdogServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

HelloWatchdogProxy::HelloWatchdogServiceAvailableEvent::HelloWatchdogServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * HelloWatchdogProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW HelloWatchdogProxy(roleName, ownerThread);
}

HelloWatchdogProxy * HelloWatchdogProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<HelloWatchdogProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloWatchdog::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloWatchdogProxy::_createProxy
                                                                      , ownerThread).get() );
}

HelloWatchdogProxy * HelloWatchdogProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::getEmptyString()*/ )
{
    return static_cast<HelloWatchdogProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloWatchdog::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloWatchdogProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloWatchdogProxy::HelloWatchdogProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEHelloWatchdog::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mServiceState       (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamtimeoutSleep  (  )
{
}

HelloWatchdogProxy::~HelloWatchdogProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * HelloWatchdogProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW HelloWatchdogNotificationEvent(data);
}

ServiceRequestEvent * HelloWatchdogProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW HelloWatchdogRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* HelloWatchdogProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW HelloWatchdogNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * HelloWatchdogProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloWatchdogResponseEvent(stream) );
}

RemoteResponseEvent * HelloWatchdogProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloWatchdogResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * HelloWatchdogProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW HelloWatchdogProxy::HelloWatchdogServiceAvailableEvent(consumer) );
}

void HelloWatchdogProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    HelloWatchdogResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void HelloWatchdogProxy::unregisterServiceListeners( void )
{
    HelloWatchdogResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    HelloWatchdogProxy::HelloWatchdogServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

unsigned int HelloWatchdogProxy::requestStartSleep( IENotificationEventConsumer & caller, unsigned int timeoutSleep )
{
    static const NEHelloWatchdog::eMessageIDs msgId = NEHelloWatchdog::eMessageIDs::MsgId_requestStartSleep;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << timeoutSleep;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void HelloWatchdogProxy::requestStopService( void )
{
    static const NEHelloWatchdog::eMessageIDs msgId = NEHelloWatchdog::eMessageIDs::MsgId_requestStopService;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, nullptr );
}
void HelloWatchdogProxy::requestShutdownService( void )
{
    static const NEHelloWatchdog::eMessageIDs msgId = NEHelloWatchdog::eMessageIDs::MsgId_requestShutdownService;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, nullptr );
}
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void HelloWatchdogProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    HelloWatchdogResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloWatchdogResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void HelloWatchdogProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    HelloWatchdogResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloWatchdogResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_HelloWatchdogProxy_updateData);
void HelloWatchdogProxy::updateData( HelloWatchdogResponseEvent & eventElem, NEHelloWatchdog::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const HelloWatchdogResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep:
        stream >> mParamtimeoutSleep;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEHelloWatchdog::eMessageIDs::MsgId_ServiceState:
        stream >> mServiceState;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_HelloWatchdogProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void HelloWatchdogProxy::processResponse( HelloWatchdogResponseEvent & evenElem )
{
    NEHelloWatchdog::eMessageIDs respId  = static_cast<NEHelloWatchdog::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEHelloWatchdog::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEHelloWatchdog::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/HelloWatchdogProxy.cpp file
//////////////////////////////////////////////////////////////////////////
