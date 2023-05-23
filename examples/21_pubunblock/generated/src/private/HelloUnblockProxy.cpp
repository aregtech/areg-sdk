//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloUnblockProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:56 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnblock.
 *
 * \file            generated/src/private/HelloUnblockProxy.hpp
 * \ingroup         HelloUnblock Service Interface
 * \brief           This is an automatic generated code of HelloUnblock
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/HelloUnblockProxy.hpp"
#include "generated/src/private/HelloUnblockEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloUnblockProxy::HelloUnblockServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(HelloUnblockProxy::HelloUnblockServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

HelloUnblockProxy::HelloUnblockServiceAvailableEvent::HelloUnblockServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// HelloUnblockProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * HelloUnblockProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW HelloUnblockProxy(roleName, ownerThread);
}

HelloUnblockProxy * HelloUnblockProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<HelloUnblockProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloUnblock::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloUnblockProxy::_createProxy
                                                                      , ownerThread).get() );
}

HelloUnblockProxy * HelloUnblockProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::getEmptyString()*/ )
{
    return static_cast<HelloUnblockProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloUnblock::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloUnblockProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloUnblockProxy::HelloUnblockProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEHelloUnblock::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mHelloServiceState  (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamclientId      (  )
    , mParamseqNr         (  )
{
}

HelloUnblockProxy::~HelloUnblockProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * HelloUnblockProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW HelloUnblockNotificationEvent(data);
}

ServiceRequestEvent * HelloUnblockProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW HelloUnblockRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* HelloUnblockProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW HelloUnblockNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * HelloUnblockProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloUnblockResponseEvent(stream) );
}

RemoteResponseEvent * HelloUnblockProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloUnblockResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * HelloUnblockProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW HelloUnblockProxy::HelloUnblockServiceAvailableEvent(consumer) );
}

void HelloUnblockProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    HelloUnblockResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void HelloUnblockProxy::unregisterServiceListeners( void )
{
    HelloUnblockResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    HelloUnblockProxy::HelloUnblockServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

unsigned int HelloUnblockProxy::requestIdentifier( IENotificationEventConsumer & caller )
{
    static const NEHelloUnblock::eMessageIDs msgId = NEHelloUnblock::eMessageIDs::MsgId_requestIdentifier;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, &caller );
    return mSequenceCount;
}
    
unsigned int HelloUnblockProxy::requestHelloUblock( IENotificationEventConsumer & caller, unsigned int clientId, unsigned int seqNr )
{
    static const NEHelloUnblock::eMessageIDs msgId = NEHelloUnblock::eMessageIDs::MsgId_requestHelloUblock;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientId;
    stream << seqNr;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void HelloUnblockProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    HelloUnblockResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloUnblockResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void HelloUnblockProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    HelloUnblockResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloUnblockResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_HelloUnblockProxy_updateData);
void HelloUnblockProxy::updateData( HelloUnblockResponseEvent & eventElem, NEHelloUnblock::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const HelloUnblockResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier:
        stream >> mParamclientId;
        break;

    case NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock:
        stream >> mParamclientId;
        stream >> mParamseqNr;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState:
        stream >> mHelloServiceState;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_HelloUnblockProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void HelloUnblockProxy::processResponse( HelloUnblockResponseEvent & evenElem )
{
    NEHelloUnblock::eMessageIDs respId  = static_cast<NEHelloUnblock::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEHelloUnblock::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEHelloUnblock::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEHelloUnblock::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEHelloUnblock::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/HelloUnblockProxy.cpp file
//////////////////////////////////////////////////////////////////////////
