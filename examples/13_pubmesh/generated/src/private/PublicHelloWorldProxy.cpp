//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PublicHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:19:17 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/private/PublicHelloWorldProxy.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/PublicHelloWorldProxy.hpp"
#include "generated/src/private/PublicHelloWorldEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldProxy::PublicHelloWorldServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(PublicHelloWorldProxy::PublicHelloWorldServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

PublicHelloWorldProxy::PublicHelloWorldServiceAvailableEvent::PublicHelloWorldServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * PublicHelloWorldProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW PublicHelloWorldProxy(roleName, ownerThread);
}

PublicHelloWorldProxy * PublicHelloWorldProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<PublicHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPublicHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &PublicHelloWorldProxy::_createProxy
                                                                      , ownerThread).get() );
}

PublicHelloWorldProxy * PublicHelloWorldProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<PublicHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPublicHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &PublicHelloWorldProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PublicHelloWorldProxy::PublicHelloWorldProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEPublicHelloWorld::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamclient    (  )
    , mParamclientID  (  )
{
}

PublicHelloWorldProxy::~PublicHelloWorldProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * PublicHelloWorldProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW PublicHelloWorldNotificationEvent(data);
}

ServiceRequestEvent * PublicHelloWorldProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW PublicHelloWorldRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* PublicHelloWorldProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW PublicHelloWorldNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * PublicHelloWorldProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PublicHelloWorldResponseEvent(stream) );
}

RemoteResponseEvent * PublicHelloWorldProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PublicHelloWorldResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * PublicHelloWorldProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW PublicHelloWorldProxy::PublicHelloWorldServiceAvailableEvent(consumer) );
}

void PublicHelloWorldProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    PublicHelloWorldResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void PublicHelloWorldProxy::unregisterServiceListeners( void )
{
    PublicHelloWorldResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    PublicHelloWorldProxy::PublicHelloWorldServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

unsigned int PublicHelloWorldProxy::requestRegister( IENotificationEventConsumer & caller, const String & name, const ServiceAddress & service, const String & thread, const String & process )
{
    static const NEPublicHelloWorld::eMessageIDs msgId = NEPublicHelloWorld::eMessageIDs::MsgId_requestRegister;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << name;
    stream << service;
    stream << thread;
    stream << process;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void PublicHelloWorldProxy::requestUnregister( const NEPublicHelloWorld::sClientRegister & client )
{
    static const NEPublicHelloWorld::eMessageIDs msgId = NEPublicHelloWorld::eMessageIDs::MsgId_requestUnregister;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << client;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
unsigned int PublicHelloWorldProxy::requestHelloWorld( IENotificationEventConsumer & caller, unsigned int clientID )
{
    static const NEPublicHelloWorld::eMessageIDs msgId = NEPublicHelloWorld::eMessageIDs::MsgId_requestHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientID;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void PublicHelloWorldProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    PublicHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PublicHelloWorldResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void PublicHelloWorldProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    PublicHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PublicHelloWorldResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_PublicHelloWorldProxy_updateData);
void PublicHelloWorldProxy::updateData( PublicHelloWorldResponseEvent & eventElem, NEPublicHelloWorld::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const PublicHelloWorldResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister:
        stream >> mParamclient;
        break;

    case NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        stream >> mParamclientID;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(generated_src_private_PublicHelloWorldProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void PublicHelloWorldProxy::processResponse( PublicHelloWorldResponseEvent & evenElem )
{
    NEPublicHelloWorld::eMessageIDs respId  = static_cast<NEPublicHelloWorld::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEPublicHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEPublicHelloWorld::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEPublicHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEPublicHelloWorld::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/PublicHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////
