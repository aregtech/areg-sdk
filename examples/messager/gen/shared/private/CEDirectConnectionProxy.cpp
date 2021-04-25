//////////////////////////////////////////////////////////////////////////
// Begin generate gen/shared/private/CEDirectConnectionProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     02.09.2019  17:27:34 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/shared/private/CEDirectConnectionProxy.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "gen/shared/private/CEDirectConnectionProxy.hpp"
#include "gen/shared/private/CEDirectConnectionEvents.hpp"
#include "areg/core/component/IEProxyListener.hpp"
#include "areg/core/base/CEThread.hpp"
#include "areg/core/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionProxy::CEDirectConnectionServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CEDirectConnectionProxy::CEDirectConnectionServiceAvailableEvent, CEProxyBase::CEServiceAvailableEvent);

CEDirectConnectionProxy::CEDirectConnectionServiceAvailableEvent::CEDirectConnectionServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEProxyBase::CEServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CEDirectConnectionProxy::CEDirectConnectionServiceAvailableEvent::~CEDirectConnectionServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & CEDirectConnectionProxy::_createInterfaceData( void )
{
    return NEDirectConnection::CreateInterfaceData( );
}

CEProxyBase * CEDirectConnectionProxy::_createProxy( const char * const roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CEDirectConnectionProxy(roleName, ownerThread);
}

CEDirectConnectionProxy * CEDirectConnectionProxy::CreateProxy( const char * const roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread )
{
    return static_cast<CEDirectConnectionProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEDirectConnection::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEDirectConnectionProxy::_createProxy
                                                                        , ownerThread) );
}

CEDirectConnectionProxy * CEDirectConnectionProxy::CreateProxy( const char * const roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CEDirectConnectionProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEDirectConnection::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEDirectConnectionProxy::_createProxy
                                                                        , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEDirectConnectionProxy::CEDirectConnectionProxy( const char * const roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
    : CEProxyBase(roleName, CEDirectConnectionProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mInitiatedConnections   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamsucceeded         (  )
    , mParamtarget            (  )
    , mParaminitiator         (  )
    , mParamlistParticipants  (  )
{
    ; // do nothing
}

CEDirectConnectionProxy::~CEDirectConnectionProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

CENotificationEvent * CEDirectConnectionProxy::CreateNotificationEvent( const CENotificationEventData & data ) const
{
    return DEBUG_NEW CEDirectConnectionNotificationEvent(data);
}

CEServiceRequestEvent * CEDirectConnectionProxy::CreateRequestEvent( const CEEventDataStream & args, const unsigned int reqId )
{
    return DEBUG_NEW CEDirectConnectionRequestEvent(args, GetProxyAddress(), GetStubAddress(), reqId);
}

CEServiceRequestEvent* CEDirectConnectionProxy::CreateNotificationRequestEvent( const unsigned int msgId, const NEService::eRequestType reqType )
{
    return DEBUG_NEW CEDirectConnectionNotifyRequestEvent(GetProxyAddress(), GetStubAddress(), msgId, reqType);
}

CERemoteResponseEvent * CEDirectConnectionProxy::CreateRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CEDirectConnectionResponseEvent(stream) );
}

CERemoteResponseEvent * CEDirectConnectionProxy::CreateRemoteRequestFailedEvent(const CEProxyAddress & addrProxy, unsigned int msgId, const NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CEDirectConnectionResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

CEProxyBase::CEServiceAvailableEvent * CEDirectConnectionProxy::CreateServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<CEProxyBase::CEServiceAvailableEvent *>( DEBUG_NEW CEDirectConnectionProxy::CEDirectConnectionServiceAvailableEvent(consumer) );
}

void CEDirectConnectionProxy::RegisterServiceListeners( void )
{
    CEProxyBase::RegisterServiceListeners( );
    CEDirectConnectionResponseEvent::AddListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CEDirectConnectionProxy::UnregisterServiceListeners( void )
{
    CEDirectConnectionResponseEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEDirectConnectionProxy::CEDirectConnectionServiceAvailableEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEProxyBase::UnregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int CEDirectConnectionProxy::RequestConnectoinSetup( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_RequestConnectoinSetup;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    SendRequestEvent( static_cast<const unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
unsigned int CEDirectConnectionProxy::RequestAddParticipant( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_RequestAddParticipant;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    SendRequestEvent( static_cast<const unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
unsigned int CEDirectConnectionProxy::RequestRemoveParticipant( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_RequestRemoveParticipant;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    SendRequestEvent( static_cast<const unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void CEDirectConnectionProxy::RequestCloseConnection( const NEDirectConnection::sInitiator & initiator )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_RequestCloseConnection;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << initiator;
    SendRequestEvent( static_cast<const unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CEDirectConnectionProxy::ProcessResponseEvent( CEServiceResponseEvent & eventElem )
{
    CEDirectConnectionResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEDirectConnectionResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

void CEDirectConnectionProxy::ProcessAttributeEvent( CEServiceResponseEvent & eventElem )
{
    CEDirectConnectionResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEDirectConnectionResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(gen_shared_private_CEDirectConnectionProxy_UpdateData);
void CEDirectConnectionProxy::UpdateData( CEDirectConnectionResponseEvent & eventElem, const NEDirectConnection::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CEDirectConnectionResponseEvent &>(eventElem).GetData().GetReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEDirectConnection::MSG_ID_ResponseConnectoinSetup:
        stream >> mParamsucceeded;
        stream >> mParamtarget;
        stream >> mParaminitiator;
        stream >> mParamlistParticipants;
        break;

    case NEDirectConnection::MSG_ID_ResponseAddParticipant:
        stream >> mParamsucceeded;
        stream >> mParamlistParticipants;
        break;

    case NEDirectConnection::MSG_ID_ResponseRemoveParticipant:
        stream >> mParamsucceeded;
        stream >> mParamlistParticipants;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEDirectConnection::MSG_ID_InitiatedConnections:
        stream >> mInitiatedConnections;
        break;

    default:
        {
            TRACE_SCOPE(gen_shared_private_CEDirectConnectionProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer());
            ASSERT(false);
        }
        break;
    }
}
 
void CEDirectConnectionProxy::ProcessResponse( CEDirectConnectionResponseEvent & evenElem )
{
    NEDirectConnection::eMessageIDs respId  = static_cast<NEDirectConnection::eMessageIDs>(evenElem.GetResponseId());
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
        respId      = static_cast<NEDirectConnection::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectConnection::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::IsRequestId(respId) )
            respId  = static_cast<NEDirectConnection::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectConnection::MSG_ID_NO_PROCEED;            
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
// End generate gen/shared/private/CEDirectConnectionProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 