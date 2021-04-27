//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     25.04.2021  23:50:46 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectMessagerProxy.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/private/CEDirectMessagerProxy.hpp"
#include "shared/generated/private/CEDirectMessagerEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/CEThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent, CEProxyBase::CEServiceAvailableEvent)

CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent::CEDirectMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEProxyBase::CEServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent::~CEDirectMessagerServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & CEDirectMessagerProxy::_createInterfaceData( void )
{
    return NEDirectMessager::CreateInterfaceData( );
}

CEProxyBase * CEDirectMessagerProxy::_createProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CEDirectMessagerProxy(roleName, ownerThread);
}

CEDirectMessagerProxy * CEDirectMessagerProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread )
{
    return static_cast<CEDirectMessagerProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEDirectMessager::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEDirectMessagerProxy::_createProxy
                                                                        , ownerThread) );
}

CEDirectMessagerProxy * CEDirectMessagerProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CEDirectMessagerProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEDirectMessager::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEDirectMessagerProxy::_createProxy
                                                                        , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEDirectMessagerProxy::CEDirectMessagerProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
    : CEProxyBase(roleName, CEDirectMessagerProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mChatParticipants       (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamsucceed           (  )
    , mParamlistParticipant   (  )
    , mParamtimeConnect       (  )
    , mParamtimeConnected     (  )
    , mParamsender            (  )
    , mParammsgText           (  )
    , mParamtimeSent          (  )
    , mParamparticipant       (  )
    , mParamtimeJoined        (  )
    , mParamtimeLeft          (  )
{
    ; // do nothing
}

CEDirectMessagerProxy::~CEDirectMessagerProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

CENotificationEvent * CEDirectMessagerProxy::CreateNotificationEvent( const CENotificationEventData & data ) const
{
    return DEBUG_NEW CEDirectMessagerNotificationEvent(data);
}

CEServiceRequestEvent * CEDirectMessagerProxy::CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW CEDirectMessagerRequestEvent(args, GetProxyAddress(), GetStubAddress(), reqId);
}

CEServiceRequestEvent* CEDirectMessagerProxy::CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW CEDirectMessagerNotifyRequestEvent(GetProxyAddress(), GetStubAddress(), msgId, reqType);
}

CERemoteResponseEvent * CEDirectMessagerProxy::CreateRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CEDirectMessagerResponseEvent(stream) );
}

CERemoteResponseEvent * CEDirectMessagerProxy::CreateRemoteRequestFailedEvent(const CEProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<CERemoteResponseEvent *>( DEBUG_NEW CEDirectMessagerResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

CEProxyBase::CEServiceAvailableEvent * CEDirectMessagerProxy::CreateServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<CEProxyBase::CEServiceAvailableEvent *>( DEBUG_NEW CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent(consumer) );
}

void CEDirectMessagerProxy::RegisterServiceListeners( void )
{
    CEProxyBase::RegisterServiceListeners( );
    CEDirectMessagerResponseEvent::AddListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CEDirectMessagerProxy::UnregisterServiceListeners( void )
{
    CEDirectMessagerResponseEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEProxyBase::UnregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int CEDirectMessagerProxy::RequestChatJoin( IENotificationEventConsumer & caller, const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_RequestChatJoin;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << participant;
    stream << timeConnect;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void CEDirectMessagerProxy::RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_RequestMessageSend;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << sender;
    stream << msgText;
    stream << timeSent;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
void CEDirectMessagerProxy::RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_RequestMessageType;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << participant;
    stream << msgText;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
void CEDirectMessagerProxy::RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_RequestChatLeave;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << participant;
    stream << timeLeave;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CEDirectMessagerProxy::ProcessResponseEvent( CEServiceResponseEvent & eventElem )
{
    CEDirectMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEDirectMessagerResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

void CEDirectMessagerProxy::ProcessAttributeEvent( CEServiceResponseEvent & eventElem )
{
    CEDirectMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEDirectMessagerResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_private_CEDirectMessagerProxy_UpdateData);
void CEDirectMessagerProxy::UpdateData( CEDirectMessagerResponseEvent & eventElem, NEDirectMessager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CEDirectMessagerResponseEvent &>(eventElem).GetData().GetReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEDirectMessager::MSG_ID_ResponseChatJoin:
        stream >> mParamsucceed;
        stream >> mParamlistParticipant;
        stream >> mParamtimeConnect;
        stream >> mParamtimeConnected;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEDirectMessager::MSG_ID_BroadcastMessageSent:
        stream >> mParamsender;
        stream >> mParammsgText;
        stream >> mParamtimeSent;
        break;

    case NEDirectMessager::MSG_ID_BroadcastMessageTyped:
        stream >> mParamparticipant;
        stream >> mParammsgText;
        break;

    case NEDirectMessager::MSG_ID_BroadcastParticipantJoined:
        stream >> mParamparticipant;
        stream >> mParamtimeJoined;
        break;

    case NEDirectMessager::MSG_ID_BroadcastParticipantLeft:
        stream >> mParamparticipant;
        stream >> mParamtimeLeft;
        break;

    case NEDirectMessager::MSG_ID_BroadcastChatClosed:
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEDirectMessager::MSG_ID_ChatParticipants:
        stream >> mChatParticipants;
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_private_CEDirectMessagerProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer());
            ASSERT(false);
        }
        break;
    }
}
 
void CEDirectMessagerProxy::ProcessResponse( CEDirectMessagerResponseEvent & evenElem )
{
    NEDirectMessager::eMessageIDs respId  = static_cast<NEDirectMessager::eMessageIDs>(evenElem.GetResponseId());
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
        respId      = static_cast<NEDirectMessager::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectMessager::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::IsRequestId(respId) )
            respId  = static_cast<NEDirectMessager::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectMessager::MSG_ID_NO_PROCEED;            
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
// End generate shared/generated/private/CEDirectMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 