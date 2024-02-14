//////////////////////////////////////////////////////////////////////////
// Begin generate examples/10_locsvc/private/HelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2024
 *
 * Generated at     06.02.2024  01:00:34 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            examples/10_locsvc/private/HelloWorldProxy.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "examples/10_locsvc/private/HelloWorldProxy.hpp"
#include "examples/10_locsvc/private/HelloWorldEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloWorldProxy::HelloWorldServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(HelloWorldProxy::HelloWorldServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

HelloWorldProxy::HelloWorldServiceAvailableEvent::HelloWorldServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * HelloWorldProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW HelloWorldProxy(roleName, ownerThread);
}

HelloWorldProxy * HelloWorldProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<HelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloWorldProxy::_createProxy
                                                                      , ownerThread).get() );
}

HelloWorldProxy * HelloWorldProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<HelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloWorldProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloWorldProxy::HelloWorldProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEHelloWorld::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParammaxNumber (  )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * HelloWorldProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW HelloWorldNotificationEvent(data);
}

ServiceRequestEvent * HelloWorldProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW HelloWorldRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* HelloWorldProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW HelloWorldNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * HelloWorldProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    ProxyBase::ServiceAvailableEvent* event = static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW HelloWorldProxy::HelloWorldServiceAvailableEvent(consumer) );
    if (event != nullptr)
    {
        if (mDispatcherThread.getId() != Thread::getCurrentThreadId())
        {
            event->setEventDelay(ProxyBase::MINIMAL_DELAY_TIME_MS);
        }
    }

    return event;
}

void HelloWorldProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    HelloWorldResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void HelloWorldProxy::unregisterServiceListeners( void )
{
    HelloWorldResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    HelloWorldProxy::HelloWorldServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

const SequenceNumber &  HelloWorldProxy::requestHelloWorld( IENotificationEventConsumer & caller, const String & roleName )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << roleName;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}

void HelloWorldProxy::requestShutdownService( void )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_requestShutdownService;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, nullptr );
}
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void HelloWorldProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    HelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloWorldResponseEvent );
    if (eventResp != nullptr)
    {
        processResponse( *eventResp );
    }
    else
    {
        processProxyEvent( eventElem );
    }
}

void HelloWorldProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    HelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloWorldResponseEvent );
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

DEF_TRACE_SCOPE(examples_10_locsvc_private_HelloWorldProxy_updateData);
void HelloWorldProxy::updateData( HelloWorldResponseEvent & eventElem, NEHelloWorld::eMessageIDs respId )
{
    const IEInStream & stream { static_cast<const HelloWorldResponseEvent &>(eventElem).getData().getReadStream() };

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_broadcastReachedMaximum:
        stream >> mParammaxNumber;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(examples_10_locsvc_private_HelloWorldProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

void HelloWorldProxy::processResponse( HelloWorldResponseEvent & evenElem )
{
    NEHelloWorld::eMessageIDs respId  { static_cast<NEHelloWorld::eMessageIDs>(evenElem.getResponseId()) };
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
        respId      = static_cast<NEHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEHelloWorld::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEHelloWorld::eMessageIDs::MsgId_NotProcessed;
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
// End generate examples/10_locsvc/private/HelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////
