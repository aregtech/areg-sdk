//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     25.04.2021  23:50:46 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectMessagerClientBase.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/CEDirectMessagerClientBase.hpp"
#include "areg/component/CEComponentThread.hpp"
#include "areg/component/CEComponent.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEDirectMessager
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CEDirectMessagerClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &CEDirectMessagerClientBase::RequestChatJoinFailed
        , &CEDirectMessagerClientBase::RequestMessageSendFailed
        , &CEDirectMessagerClientBase::RequestMessageTypeFailed
        , &CEDirectMessagerClientBase::RequestChatLeaveFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CEDirectMessagerClientBase::CEDirectMessagerClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEDirectMessagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEDirectMessagerClientBase::CEDirectMessagerClientBase( const char* roleName, CEDispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEDirectMessagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEDirectMessagerClientBase::CEDirectMessagerClientBase( const char* roleName, CEComponent & owner )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEDirectMessagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), owner.GetMasterThread()) )
{
    ; // do nothing
}

CEDirectMessagerClientBase::~CEDirectMessagerClientBase( void )
{
    if (mProxy != NULL)
    {
        mProxy->ClearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
        mProxy->FreeProxy( static_cast<IEProxyListener &>(self()) );
        mProxy      = NULL;
    }
    mIsConnected= false;
}

/************************************************************************
 * Operations
 ************************************************************************/

bool CEDirectMessagerClientBase::RecreateProxy( void )
{
    bool result         = false;
    CEString roleName   = mProxy != NULL ? mProxy->GetProxyAddress().GetRoleName() : "";
    CEString threadName = mProxy != NULL ? mProxy->GetProxyAddress().GetThread()   : "";
    if ( roleName.IsEmpty() == false )
    {
        CEDirectMessagerProxy * newProxy = CEDirectMessagerProxy::CreateProxy(roleName.GetBuffer(), static_cast<IEProxyListener &>(self()), threadName.GetBuffer());
        if (newProxy != NULL)
        {
            mProxy->ClearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
            mProxy->FreeProxy (static_cast<IEProxyListener &>(self()) );
            mProxy = newProxy;
            result = true;
        }
        else
        {
            ; // do nothing
        }
    }
    else
    {
        ; // do nothing, no role name is assigned
    }
    return result;
}

CEDispatcherThread * CEDirectMessagerClientBase::GetDispatcherThread( void )
{
    return ( mProxy != static_cast<CEDirectMessagerProxy *>(NULL) ? &(mProxy->GetProxyDispatcherThread()) : static_cast<CEDispatcherThread *>(NULL) );
}

void CEDirectMessagerClientBase::ClearAllNotifications( void )
{
    mProxy->ClearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_ServiceConnected);
bool CEDirectMessagerClientBase::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]", proxy.GetProxyAddress().GetServiceName().GetBuffer(), proxy.GetProxyAddress().GetRoleName().GetBuffer(), isConnected ? "CONNECTED" : "DISCONNECTED");
        mIsConnected= isConnected;
        result      = true;
    }
    return result;
}

void CEDirectMessagerClientBase::NotifyOn( NEDirectMessager::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->SetNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->ClearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_ProcessNotificationEvent);
void CEDirectMessagerClientBase::ProcessNotificationEvent( CENotificationEvent & eventElem )
{
    const CENotificationEventData & data= static_cast<const CENotificationEvent &>(eventElem).GetData();
    NEService::eResultType result       = data.GetNotifyType();
    NEDirectMessager::eMessageIDs msgId   = static_cast<NEDirectMessager::eMessageIDs>(data.GetNotifyId());
    mCurrSequenceNr = data.GetSequenceNr();

    switch (result)
    {
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_CANCELED:
        {
        /************************************************************************
         * Trigger request error handling if request failed.
         ************************************************************************/
            RequestFailed(msgId, result);
        }
        break;

    case NEService::RESULT_INVALID:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification 
         ************************************************************************/
            InvalidResponse(msgId);
        }
        break;

    case NEService::RESULT_DATA_OK:
    case NEService::RESULT_DATA_INVALID:
        {
            NEService::eDataStateType dataState;
            switch (msgId)
            {
        /************************************************************************
         * Trigger attribute update processing
         ************************************************************************/
            case NEDirectMessager::MSG_ID_ChatParticipants:
                {
                    const NEDirectMessager::ListParticipants & ChatParticipants = mProxy->GetChatParticipants(dataState);
                    OnChatParticipantsUpdate(ChatParticipants, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEDirectMessagerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::RESULT_OK:
        {
            switch (msgId)
            {
        /************************************************************************
         * Trigger response processing
         ************************************************************************/
            case NEDirectMessager::MSG_ID_ResponseChatJoin:
                {
                    const bool & succeed = mProxy->GetParamsucceed();
                    const NEDirectMessager::ListParticipants & listParticipant = mProxy->GetParamlistParticipant();
                    const CEDateTime & timeConnect = mProxy->GetParamtimeConnect();
                    const CEDateTime & timeConnected = mProxy->GetParamtimeConnected();
                    ResponseChatJoin( succeed, listParticipant, timeConnect, timeConnected );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NEDirectMessager::MSG_ID_BroadcastMessageSent:
                {
                    const NEDirectMessager::sParticipant & sender = mProxy->GetParamsender();
                    const CEString & msgText = mProxy->GetParammsgText();
                    const CEDateTime & timeSent = mProxy->GetParamtimeSent();
                    BroadcastMessageSent( sender, msgText, timeSent );
                }
                break;

            case NEDirectMessager::MSG_ID_BroadcastMessageTyped:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->GetParamparticipant();
                    const CEString & msgText = mProxy->GetParammsgText();
                    BroadcastMessageTyped( participant, msgText );
                }
                break;

            case NEDirectMessager::MSG_ID_BroadcastParticipantJoined:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->GetParamparticipant();
                    const CEDateTime & timeJoined = mProxy->GetParamtimeJoined();
                    BroadcastParticipantJoined( participant, timeJoined );
                }
                break;

            case NEDirectMessager::MSG_ID_BroadcastParticipantLeft:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->GetParamparticipant();
                    const CEDateTime & timeLeft = mProxy->GetParamtimeLeft();
                    BroadcastParticipantLeft( participant, timeLeft );
                }
                break;

            case NEDirectMessager::MSG_ID_BroadcastChatClosed:
                {
                    BroadcastChatClosed(  );
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEDirectMessagerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
                                , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }        
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_ProcessNotificationEvent);
            TRACE_ERR("Client object CEDirectMessagerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
                        , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                        , static_cast<int>(result));
            ASSERT(false);
        }
        break;
    }
}

/************************************************************************
 * Error handling calls
 ************************************************************************/
DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_InvalidResponse);
void CEDirectMessagerClientBase::InvalidResponse( NEDirectMessager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_InvalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client CEDirectMessagerClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEDirectMessager::GetString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    CEClientBase::ResponseInvalidNotImpelemnted("CEDirectMessagerClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_InvalidRequest);
void CEDirectMessagerClientBase::InvalidRequest( NEDirectMessager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_InvalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client CEDirectMessagerClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEDirectMessager::GetString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_RequestFailed);
void CEDirectMessagerClientBase::RequestFailed( NEDirectMessager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shared_generated_CEDirectMessagerClientBase_RequestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of CEDirectMessagerClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEDirectMessager::GetString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                    , NEService::GetString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEDirectMessager::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::IsResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEDirectMessager::GetRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::IsRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEDirectMessager::MSG_ID_INVALID);

    if ( index != static_cast<unsigned int>(NEDirectMessager::MSG_ID_INVALID) )
    {
        (this->*NEDirectMessager::failureFunctions[index])( FailureReason );
    }
    else
    {
        InvalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void CEDirectMessagerClientBase::OnChatParticipantsUpdate( const NEDirectMessager::ListParticipants & /* ChatParticipants */, NEService::eDataStateType /* state */ )
{
    CEClientBase::OnUpdateNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_ChatParticipants) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void CEDirectMessagerClientBase::RequestChatJoinFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_RequestChatJoin) );
}

void CEDirectMessagerClientBase::RequestMessageSendFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_RequestMessageSend) );
}

void CEDirectMessagerClientBase::RequestMessageTypeFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_RequestMessageType) );
}

void CEDirectMessagerClientBase::RequestChatLeaveFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_RequestChatLeave) );
}

void CEDirectMessagerClientBase::ResponseChatJoin( const bool & /* succeed */, const NEDirectMessager::ListParticipants & /* listParticipant */, const CEDateTime & /* timeConnect */, const CEDateTime & /* timeConnected */ )
{
    CEClientBase::ResponseNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_ResponseChatJoin) );
}

void CEDirectMessagerClientBase::BroadcastMessageSent( const NEDirectMessager::sParticipant & /* sender */, const CEString & /* msgText */, const CEDateTime & /* timeSent */ )
{
    CEClientBase::BroadcastNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_BroadcastMessageSent) );
}

void CEDirectMessagerClientBase::BroadcastMessageTyped( const NEDirectMessager::sParticipant & /* participant */, const CEString & /* msgText */ )
{
    CEClientBase::BroadcastNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_BroadcastMessageTyped) );
}

void CEDirectMessagerClientBase::BroadcastParticipantJoined( const NEDirectMessager::sParticipant & /* participant */, const CEDateTime & /* timeJoined */ )
{
    CEClientBase::BroadcastNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_BroadcastParticipantJoined) );
}

void CEDirectMessagerClientBase::BroadcastParticipantLeft( const NEDirectMessager::sParticipant & /* participant */, const CEDateTime & /* timeLeft */ )
{
    CEClientBase::BroadcastNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_BroadcastParticipantLeft) );
}

void CEDirectMessagerClientBase::BroadcastChatClosed( void )
{
    CEClientBase::BroadcastNotImplemented( "CEDirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_BroadcastChatClosed) );
}


//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
