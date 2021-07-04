//////////////////////////////////////////////////////////////////////////
// Begin generate gen/private/HelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 * Generated at     04.07.2021  04:19:39 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/private/HelloWorldProxy.cpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "gen/private/HelloWorldProxy.hpp"
#include "gen/private/HelloWorldEvents.hpp"
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
    ; // do nothing
}

HelloWorldProxy::HelloWorldServiceAvailableEvent::~HelloWorldServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * HelloWorldProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
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

HelloWorldProxy * HelloWorldProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
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
HelloWorldProxy::HelloWorldProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NEHelloWorld::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mConnectedClients   (  )
    , mRemainOutput       (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamclientInfo    (  )
    , mParamclientList    (  )
{
    ; // do nothing
}

HelloWorldProxy::~HelloWorldProxy( void )
{
    ; // do nothing
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
 
unsigned int HelloWorldProxy::requestHelloWorld( IENotificationEventConsumer & caller, const String & roleName, const String & addMessage/* = "" */ )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::MSG_ID_requestHelloWorld;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << roleName;
    stream << addMessage;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void HelloWorldProxy::requestClientShutdown( unsigned int clientID, const String & roleName )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::MSG_ID_requestClientShutdown;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientID;
    stream << roleName;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
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
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void HelloWorldProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    HelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, HelloWorldResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(gen_private_HelloWorldProxy_updateData);
void HelloWorldProxy::updateData( HelloWorldResponseEvent & eventElem, NEHelloWorld::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const HelloWorldResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEHelloWorld::MSG_ID_responseHelloWorld:
        stream >> mParamclientInfo;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEHelloWorld::MSG_ID_broadcastHelloClients:
        stream >> mParamclientList;
        break;

    case NEHelloWorld::MSG_ID_broadcastServiceUnavailable:
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEHelloWorld::MSG_ID_ConnectedClients:
        stream >> mConnectedClients;
        break;

    case NEHelloWorld::MSG_ID_RemainOutput:
        stream >> mRemainOutput;
        break;

    default:
        {
            TRACE_SCOPE(gen_private_HelloWorldProxy_updateData);
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
    case NEService::RESULT_DATA_INVALID:
    case NEService::RESULT_INVALID:
        break;  // do nothing

    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_CANCELED:
        respId      = static_cast<NEHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEHelloWorld::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NEHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEHelloWorld::MSG_ID_NO_PROCEED;            
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
// End generate gen/private/HelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 