//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/ConnectionManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/private/ConnectionManagerProxy.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/private/ConnectionManagerProxy.hpp"
#include "generated/private/ConnectionManagerEvents.hpp"
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
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * ConnectionManagerProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW ConnectionManagerProxy(roleName, ownerThread);
}

ConnectionManagerProxy * ConnectionManagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<ConnectionManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEConnectionManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &ConnectionManagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

ConnectionManagerProxy * ConnectionManagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::getEmptyString()*/ )
{
    return static_cast<ConnectionManagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEConnectionManager::getInterfaceData()
                                                                      , connectListener
                                                                      , &ConnectionManagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ConnectionManagerProxy::ConnectionManagerProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
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
}

ConnectionManagerProxy::~ConnectionManagerProxy( void )
{
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

unsigned int ConnectionManagerProxy::requestConnect( IENotificationEventConsumer & caller, const String & nickName, const DateTime & dateTime )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_requestConnect;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << dateTime;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
unsigned int ConnectionManagerProxy::requestRegisterConnection( IENotificationEventConsumer & caller, const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_requestRegisterConnection;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << connectCookie;
    stream << dateRegister;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void ConnectionManagerProxy::requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_requestDisconnect;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << dateTime;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
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
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void ConnectionManagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    ConnectionManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, ConnectionManagerResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_private_ConnectionManagerProxy_updateData);
void ConnectionManagerProxy::updateData( ConnectionManagerResponseEvent & eventElem, NEConnectionManager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const ConnectionManagerResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEConnectionManager::eMessageIDs::MsgId_responseConnect:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamdateTime;
        stream >> mParamresult;
        break;

    case NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection:
        stream >> mParamconnection;
        stream >> mParamconnectionList;
        stream >> mParamsuccess;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated:
        stream >> mParamupdatedList;
        break;

    case NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected:
        stream >> mParamclientConnected;
        break;

    case NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected:
        stream >> mParamclientLeft;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEConnectionManager::eMessageIDs::MsgId_ConnectionList:
        stream >> mConnectionList;
        break;

    default:
        {
            TRACE_SCOPE(generated_private_ConnectionManagerProxy_updateData);
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
    case NEService::eResultType::DataInvalid:   // fall through
    case NEService::eResultType::RequestInvalid:
        break;  // do nothing

    case NEService::eResultType::RequestError:  // fall through
    case NEService::eResultType::RequestBusy:   // fall through
    case NEService::eResultType::RequestCanceled:
        respId      = static_cast<NEConnectionManager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEConnectionManager::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEConnectionManager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEConnectionManager::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/private/ConnectionManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
