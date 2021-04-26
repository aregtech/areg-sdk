//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CECentralMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     25.04.2021  23:50:42 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CECentralMessagerProxy.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/private/CECentralMessagerProxy.hpp"
#include "shared/generated/private/CECentralMessagerEvents.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent, CEProxyBase::CEServiceAvailableEvent)

CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent::CECentralMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEProxyBase::CEServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent::~CECentralMessagerServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & CECentralMessagerProxy::_createInterfaceData( void )
{
    return NECentralMessager::CreateInterfaceData( );
}

CEProxyBase * CECentralMessagerProxy::_createProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CECentralMessagerProxy(roleName, ownerThread);
}

CECentralMessagerProxy * CECentralMessagerProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread )
{
    return static_cast<CECentralMessagerProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NECentralMessager::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CECentralMessagerProxy::_createProxy
                                                                        , ownerThread) );
}

CECentralMessagerProxy * CECentralMessagerProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CECentralMessagerProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NECentralMessager::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CECentralMessagerProxy::_createProxy
                                                                        , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CECentralMessagerProxy::CECentralMessagerProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
    : CEProxyBase(roleName, CECentralMessagerProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamnickName      (  )
    , mParamcookie        (  )
    , mParamnewMessage    (  )
    , mParamdateTime      (  )
    , mParamserverMessage (  )
{
    ; // do nothing
}

CECentralMessagerProxy::~CECentralMessagerProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

CENotificationEvent * CECentralMessagerProxy::CreateNotificationEvent( const CENotificationEventData & data ) const
{
    return DEBUG_NEW CECentralMessagerNotificationEvent(data);
}

CEServiceRequestEvent * CECentralMessagerProxy::CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW CECentralMessagerRequestEvent(args, GetProxyAddress(), GetStubAddress(), reqId);
}

CEServiceRequestEvent* CECentralMessagerProxy::CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW CECentralMessagerNotifyRequestEvent(GetProxyAddress(), GetStubAddress(), msgId, reqType);
}

CERemoteResponseEvent * CECentralMessagerProxy::CreateRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CECentralMessagerResponseEvent(stream) );
}

CERemoteResponseEvent * CECentralMessagerProxy::CreateRemoteRequestFailedEvent(const CEProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CECentralMessagerResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

CEProxyBase::CEServiceAvailableEvent * CECentralMessagerProxy::CreateServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<CEProxyBase::CEServiceAvailableEvent *>( DEBUG_NEW CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent(consumer) );
}

void CECentralMessagerProxy::RegisterServiceListeners( void )
{
    CEProxyBase::RegisterServiceListeners( );
    CECentralMessagerResponseEvent::AddListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CECentralMessagerProxy::UnregisterServiceListeners( void )
{
    CECentralMessagerResponseEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEProxyBase::UnregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
void CECentralMessagerProxy::RequestSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::MSG_ID_RequestSendMessage;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    stream << dateTime;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
void CECentralMessagerProxy::RequestKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::MSG_ID_RequestKeyTyping;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CECentralMessagerProxy::ProcessResponseEvent( CEServiceResponseEvent & eventElem )
{
    CECentralMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CECentralMessagerResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

void CECentralMessagerProxy::ProcessAttributeEvent( CEServiceResponseEvent & eventElem )
{
    CECentralMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CECentralMessagerResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_private_CECentralMessagerProxy_UpdateData);
void CECentralMessagerProxy::UpdateData( CECentralMessagerResponseEvent & eventElem, NECentralMessager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CECentralMessagerResponseEvent &>(eventElem).GetData().GetReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NECentralMessager::MSG_ID_BroadcastSendMessage:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamnewMessage;
        stream >> mParamdateTime;
        break;

    case NECentralMessager::MSG_ID_BroadcastKeyTyping:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamnewMessage;
        break;

    case NECentralMessager::MSG_ID_BroadcastBroadcastMessage:
        stream >> mParamserverMessage;
        stream >> mParamdateTime;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(shared_generated_private_CECentralMessagerProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer());
            ASSERT(false);
        }
        break;
    }
}
 
void CECentralMessagerProxy::ProcessResponse( CECentralMessagerResponseEvent & evenElem )
{
    NECentralMessager::eMessageIDs respId  = static_cast<NECentralMessager::eMessageIDs>(evenElem.GetResponseId());
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
        respId      = static_cast<NECentralMessager::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NECentralMessager::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::IsRequestId(respId) )
            respId  = static_cast<NECentralMessager::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NECentralMessager::MSG_ID_NO_PROCEED;            
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
// End generate shared/generated/private/CECentralMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 