//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteHelloWorld.
 * Generated at     29.05.2021  12:43:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteHelloWorldProxy.cpp
 * \ingroup         RemoteHelloWorld Service Interface
 * \brief           This is an automatic generated code of RemoteHelloWorld Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shareipcmix/src/private/RemoteHelloWorldProxy.hpp"
#include "shareipcmix/src/private/RemoteHelloWorldEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldProxy::RemoteHelloWorldServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(RemoteHelloWorldProxy::RemoteHelloWorldServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

RemoteHelloWorldProxy::RemoteHelloWorldServiceAvailableEvent::RemoteHelloWorldServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

RemoteHelloWorldProxy::RemoteHelloWorldServiceAvailableEvent::~RemoteHelloWorldServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * RemoteHelloWorldProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW RemoteHelloWorldProxy(roleName, ownerThread);
}

RemoteHelloWorldProxy * RemoteHelloWorldProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<RemoteHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NERemoteHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &RemoteHelloWorldProxy::_createProxy
                                                                      , ownerThread) );
}

RemoteHelloWorldProxy * RemoteHelloWorldProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<RemoteHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NERemoteHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &RemoteHelloWorldProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteHelloWorldProxy::RemoteHelloWorldProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NERemoteHelloWorld::getInterfaceData(), ownerThread)

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

RemoteHelloWorldProxy::~RemoteHelloWorldProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * RemoteHelloWorldProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW RemoteHelloWorldNotificationEvent(data);
}

ServiceRequestEvent * RemoteHelloWorldProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW RemoteHelloWorldRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* RemoteHelloWorldProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW RemoteHelloWorldNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * RemoteHelloWorldProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW RemoteHelloWorldResponseEvent(stream) );
}

RemoteResponseEvent * RemoteHelloWorldProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW RemoteHelloWorldResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * RemoteHelloWorldProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW RemoteHelloWorldProxy::RemoteHelloWorldServiceAvailableEvent(consumer) );
}

void RemoteHelloWorldProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    RemoteHelloWorldResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void RemoteHelloWorldProxy::unregisterServiceListeners( void )
{
    RemoteHelloWorldResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    RemoteHelloWorldProxy::RemoteHelloWorldServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int RemoteHelloWorldProxy::requestHelloWorld( IENotificationEventConsumer & caller, const String & roleName, const String & addMessage/* = "" */ )
{
    static const NERemoteHelloWorld::eMessageIDs msgId = NERemoteHelloWorld::MSG_ID_requestHelloWorld;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << roleName;
    stream << addMessage;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void RemoteHelloWorldProxy::requestClientShutdown( unsigned int clientID, const String & roleName )
{
    static const NERemoteHelloWorld::eMessageIDs msgId = NERemoteHelloWorld::MSG_ID_requestClientShutdown;
    EventDataStream args(EventDataStream::EventDataExternal);
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

void RemoteHelloWorldProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    RemoteHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, RemoteHelloWorldResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void RemoteHelloWorldProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    RemoteHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, RemoteHelloWorldResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shareipcmix_src_private_RemoteHelloWorldProxy_updateData);
void RemoteHelloWorldProxy::updateData( RemoteHelloWorldResponseEvent & eventElem, NERemoteHelloWorld::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const RemoteHelloWorldResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NERemoteHelloWorld::MSG_ID_responseHelloWorld:
        stream >> mParamclientInfo;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NERemoteHelloWorld::MSG_ID_broadcastHelloClients:
        stream >> mParamclientList;
        break;

    case NERemoteHelloWorld::MSG_ID_broadcastServiceUnavailable:
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NERemoteHelloWorld::MSG_ID_ConnectedClients:
        stream >> mConnectedClients;
        break;

    case NERemoteHelloWorld::MSG_ID_RemainOutput:
        stream >> mRemainOutput;
        break;

    default:
        {
            TRACE_SCOPE(shareipcmix_src_private_RemoteHelloWorldProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void RemoteHelloWorldProxy::processResponse( RemoteHelloWorldResponseEvent & evenElem )
{
    NERemoteHelloWorld::eMessageIDs respId  = static_cast<NERemoteHelloWorld::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NERemoteHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NERemoteHelloWorld::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NERemoteHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NERemoteHelloWorld::MSG_ID_NO_PROCEED;            
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
// End generate shareipcmix/src/private/RemoteHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 