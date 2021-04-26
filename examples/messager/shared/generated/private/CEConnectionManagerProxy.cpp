//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEConnectionManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     25.04.2021  23:50:44 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEConnectionManagerProxy.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/private/CEConnectionManagerProxy.hpp"
#include "shared/generated/private/CEConnectionManagerEvents.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerProxy::CEConnectionManagerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CEConnectionManagerProxy::CEConnectionManagerServiceAvailableEvent, CEProxyBase::CEServiceAvailableEvent)

CEConnectionManagerProxy::CEConnectionManagerServiceAvailableEvent::CEConnectionManagerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEProxyBase::CEServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CEConnectionManagerProxy::CEConnectionManagerServiceAvailableEvent::~CEConnectionManagerServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & CEConnectionManagerProxy::_createInterfaceData( void )
{
    return NEConnectionManager::CreateInterfaceData( );
}

CEProxyBase * CEConnectionManagerProxy::_createProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CEConnectionManagerProxy(roleName, ownerThread);
}

CEConnectionManagerProxy * CEConnectionManagerProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread )
{
    return static_cast<CEConnectionManagerProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEConnectionManager::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEConnectionManagerProxy::_createProxy
                                                                        , ownerThread) );
}

CEConnectionManagerProxy * CEConnectionManagerProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CEConnectionManagerProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEConnectionManager::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEConnectionManagerProxy::_createProxy
                                                                        , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEConnectionManagerProxy::CEConnectionManagerProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
    : CEProxyBase(roleName, CEConnectionManagerProxy::_createInterfaceData(), ownerThread)

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

CEConnectionManagerProxy::~CEConnectionManagerProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

CENotificationEvent * CEConnectionManagerProxy::CreateNotificationEvent( const CENotificationEventData & data ) const
{
    return DEBUG_NEW CEConnectionManagerNotificationEvent(data);
}

CEServiceRequestEvent * CEConnectionManagerProxy::CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW CEConnectionManagerRequestEvent(args, GetProxyAddress(), GetStubAddress(), reqId);
}

CEServiceRequestEvent* CEConnectionManagerProxy::CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW CEConnectionManagerNotifyRequestEvent(GetProxyAddress(), GetStubAddress(), msgId, reqType);
}

CERemoteResponseEvent * CEConnectionManagerProxy::CreateRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CEConnectionManagerResponseEvent(stream) );
}

CERemoteResponseEvent * CEConnectionManagerProxy::CreateRemoteRequestFailedEvent(const CEProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CEConnectionManagerResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

CEProxyBase::CEServiceAvailableEvent * CEConnectionManagerProxy::CreateServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<CEProxyBase::CEServiceAvailableEvent *>( DEBUG_NEW CEConnectionManagerProxy::CEConnectionManagerServiceAvailableEvent(consumer) );
}

void CEConnectionManagerProxy::RegisterServiceListeners( void )
{
    CEProxyBase::RegisterServiceListeners( );
    CEConnectionManagerResponseEvent::AddListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CEConnectionManagerProxy::UnregisterServiceListeners( void )
{
    CEConnectionManagerResponseEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEConnectionManagerProxy::CEConnectionManagerServiceAvailableEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEProxyBase::UnregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int CEConnectionManagerProxy::RequestConnet( IENotificationEventConsumer & caller, const CEString & nickName, const CEDateTime & dateTime )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_RequestConnet;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << dateTime;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
unsigned int CEConnectionManagerProxy::RequestRegisterConnection( IENotificationEventConsumer & caller, const CEString & nickName, const unsigned int & cookie, const unsigned int & connectCookie, const CEDateTime & dateRegister )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_RequestRegisterConnection;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << connectCookie;
    stream << dateRegister;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void CEConnectionManagerProxy::RequestDiconnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_RequestDiconnect;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << dateTime;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CEConnectionManagerProxy::ProcessResponseEvent( CEServiceResponseEvent & eventElem )
{
    CEConnectionManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEConnectionManagerResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

void CEConnectionManagerProxy::ProcessAttributeEvent( CEServiceResponseEvent & eventElem )
{
    CEConnectionManagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEConnectionManagerResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_private_CEConnectionManagerProxy_UpdateData);
void CEConnectionManagerProxy::UpdateData( CEConnectionManagerResponseEvent & eventElem, NEConnectionManager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CEConnectionManagerResponseEvent &>(eventElem).GetData().GetReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEConnectionManager::MSG_ID_ResponseConnect:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamdateTime;
        stream >> mParamresult;
        break;

    case NEConnectionManager::MSG_ID_ResponseRegisterConnection:
        stream >> mParamconnection;
        stream >> mParamconnectionList;
        stream >> mParamsuccess;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEConnectionManager::MSG_ID_BroadcastConnectionUpdated:
        stream >> mParamupdatedList;
        break;

    case NEConnectionManager::MSG_ID_BroadcastClientConnected:
        stream >> mParamclientConnected;
        break;

    case NEConnectionManager::MSG_ID_BroadcastClientDisconnected:
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
            TRACE_SCOPE(shared_generated_private_CEConnectionManagerProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer());
            ASSERT(false);
        }
        break;
    }
}
 
void CEConnectionManagerProxy::ProcessResponse( CEConnectionManagerResponseEvent & evenElem )
{
    NEConnectionManager::eMessageIDs respId  = static_cast<NEConnectionManager::eMessageIDs>(evenElem.GetResponseId());
    NEService::eResultType resultType  = evenElem.GetResult();

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
        respId      = static_cast<NEConnectionManager::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEConnectionManager::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::IsRequestId(respId) )
            respId  = static_cast<NEConnectionManager::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
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
        UpdateData(evenElem, respId);
    if (setStates == true)
        SetState(static_cast<unsigned int>(respId), dataValid ? NEService::DATA_OK : NEService::DATA_INVALID);

    NotifyListeners(static_cast<unsigned int>(respId), resultType, evenElem.GetSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEConnectionManagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 