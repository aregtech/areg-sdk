//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     20.10.2021  13:21:57 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/private/HelloWorldProxy.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/HelloWorldProxy.hpp"
#include "generated/src/private/HelloWorldEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
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

ProxyBase * HelloWorldProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW HelloWorldProxy(roleName, ownerThread);
}

HelloWorldProxy * HelloWorldProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<HelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloWorldProxy::_createProxy
                                                                      , ownerThread) );
}

HelloWorldProxy * HelloWorldProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= nullptr*/ )
{
    return static_cast<HelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloWorldProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloWorldProxy::HelloWorldProxy( const char * roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEHelloWorld::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
{
}

HelloWorldProxy::~HelloWorldProxy( void )
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

RemoteResponseEvent * HelloWorldProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloWorldResponseEvent(stream) );
}

RemoteResponseEvent * HelloWorldProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloWorldResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * HelloWorldProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW HelloWorldProxy::HelloWorldServiceAvailableEvent(consumer) );
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
 
unsigned int HelloWorldProxy::requestHelloWorld( IENotificationEventConsumer & caller )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, &caller );
    return mSequenceCount;
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
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void HelloWorldProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    HelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloWorldResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_HelloWorldProxy_updateData);
void HelloWorldProxy::updateData( HelloWorldResponseEvent & eventElem, NEHelloWorld::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const HelloWorldResponseEvent &>(eventElem).getData().getReadStream();

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
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(generated_src_private_HelloWorldProxy_updateData);
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
    NEHelloWorld::eMessageIDs respId  = static_cast<NEHelloWorld::eMessageIDs>(evenElem.getResponseId());
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
// End generate generated/src/private/HelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 