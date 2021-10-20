//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloServiceProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     20.10.2021  21:36:19 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source HelloService.
 *
 * \file            generated/src/private/HelloServiceProxy.hpp
 * \ingroup         HelloService Service Interface
 * \brief           This is an automatic generated code of HelloService
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/HelloServiceProxy.hpp"
#include "generated/src/private/HelloServiceEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloServiceProxy::HelloServiceServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(HelloServiceProxy::HelloServiceServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

HelloServiceProxy::HelloServiceServiceAvailableEvent::HelloServiceServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// HelloServiceProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * HelloServiceProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW HelloServiceProxy(roleName, ownerThread);
}

HelloServiceProxy * HelloServiceProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<HelloServiceProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloService::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloServiceProxy::_createProxy
                                                                      , ownerThread) );
}

HelloServiceProxy * HelloServiceProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= nullptr*/ )
{
    return static_cast<HelloServiceProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEHelloService::getInterfaceData()
                                                                      , connectListener
                                                                      , &HelloServiceProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloServiceProxy::HelloServiceProxy( const char * roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEHelloService::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamsuccess   (  )
{
}

HelloServiceProxy::~HelloServiceProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * HelloServiceProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW HelloServiceNotificationEvent(data);
}

ServiceRequestEvent * HelloServiceProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW HelloServiceRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* HelloServiceProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW HelloServiceNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * HelloServiceProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloServiceResponseEvent(stream) );
}

RemoteResponseEvent * HelloServiceProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW HelloServiceResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * HelloServiceProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW HelloServiceProxy::HelloServiceServiceAvailableEvent(consumer) );
}

void HelloServiceProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    HelloServiceResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void HelloServiceProxy::unregisterServiceListeners( void )
{
    HelloServiceResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    HelloServiceProxy::HelloServiceServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int HelloServiceProxy::requestHelloService( IENotificationEventConsumer & caller, const String & client )
{
    static const NEHelloService::eMessageIDs msgId = NEHelloService::eMessageIDs::MsgId_requestHelloService;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << client;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void HelloServiceProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    HelloServiceResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloServiceResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void HelloServiceProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    HelloServiceResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloServiceResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_HelloServiceProxy_updateData);
void HelloServiceProxy::updateData( HelloServiceResponseEvent & eventElem, NEHelloService::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const HelloServiceResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEHelloService::eMessageIDs::MsgId_responseHelloService:
        stream >> mParamsuccess;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(generated_src_private_HelloServiceProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void HelloServiceProxy::processResponse( HelloServiceResponseEvent & evenElem )
{
    NEHelloService::eMessageIDs respId  = static_cast<NEHelloService::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEHelloService::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEHelloService::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEHelloService::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }
        
        setStates   = respId != NEHelloService::eMessageIDs::MsgId_NotProcessed;            
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
// End generate generated/src/private/HelloServiceProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 