//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/ConnectionManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     04.07.2021  04:30:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/ConnectionManagerProxy.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/private/ConnectionManagerProxy.hpp"
#include "shared/generated/private/ConnectionManagerEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent::ConnectionManagerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent::~ConnectionManagerServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * ConnectionManagerProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW ConnectionManagerProxy(roleName, ownerThread);
}

ConnectionManagerProxy * ConnectionManagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<ConnectionManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEConnectionManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &ConnectionManagerProxy::_createProxy
                                                                      , ownerThread) );
}

ConnectionManagerProxy * ConnectionManagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<ConnectionManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEConnectionManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &ConnectionManagerProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ConnectionManagerProxy::ConnectionManagerProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NEConnectionManager::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mConnectionList         (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamnickName          (  )
    , mParamcookie            (  )
    , mParamdateTime          (  )
    , mParamresult            (  )
    , mParamconnection        (  )
    , mParamconnectionList    (  )
    , mParamsuccess           (  )
    , mParamupdatedList       (  )
    , mParamclientConnected   (  )
    , mParamclientLeft        (  )
{
    ; // do nothing
}

ConnectionManagerProxy::~ConnectionManagerProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * ConnectionManagerProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW ConnectionManagerNotificationEvent(data);
}

ServiceRequestEvent * ConnectionManagerProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW ConnectionManagerRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* ConnectionManagerProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW ConnectionManagerNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * ConnectionManagerProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW ConnectionManagerResponseEvent(stream) );
}

RemoteResponseEvent * ConnectionManagerProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW ConnectionManagerResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * ConnectionManagerProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent(consumer) );
}

void ConnectionManagerProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    ConnectionManagerResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void ConnectionManagerProxy::unregisterServiceListeners( void )
{
    ConnectionManagerResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ConnectionManagerProxy::ConnectionManagerServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int ConnectionManagerProxy::requestConnet( IENotificationEventConsumer & caller, const String & nickName, const DateTime & dateTime )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_requestConnet;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << dateTime;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
unsigned int ConnectionManagerProxy::requestRegisterConnection( IENotificationEventConsumer & caller, const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_requestRegisterConnection;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << connectCookie;
    stream << dateRegister;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void ConnectionManagerProxy::requestDiconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_requestDiconnect;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << dateTime;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void ConnectionManagerProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    ConnectionManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, ConnectionManagerResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void ConnectionManagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    ConnectionManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, ConnectionManagerResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_private_ConnectionManagerProxy_updateData);
void ConnectionManagerProxy::updateData( ConnectionManagerResponseEvent & eventElem, NEConnectionManager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const ConnectionManagerResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEConnectionManager::MSG_ID_responseConnect:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamdateTime;
        stream >> mParamresult;
        break;

    case NEConnectionManager::MSG_ID_responseRegisterConnection:
        stream >> mParamconnection;
        stream >> mParamconnectionList;
        stream >> mParamsuccess;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEConnectionManager::MSG_ID_broadcastConnectionUpdated:
        stream >> mParamupdatedList;
        break;

    case NEConnectionManager::MSG_ID_broadcastClientConnected:
        stream >> mParamclientConnected;
        break;

    case NEConnectionManager::MSG_ID_broadcastClientDisconnected:
        stream >> mParamclientLeft;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEConnectionManager::MSG_ID_ConnectionList:
        stream >> mConnectionList;
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_private_ConnectionManagerProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void ConnectionManagerProxy::processResponse( ConnectionManagerResponseEvent & evenElem )
{
    NEConnectionManager::eMessageIDs respId  = static_cast<NEConnectionManager::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEConnectionManager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEConnectionManager::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NEConnectionManager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEConnectionManager::MSG_ID_NO_PROCEED;            
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
// End generate shared/generated/private/ConnectionManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 