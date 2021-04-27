//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CECentralMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     25.04.2021  23:50:42 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CECentralMessagerClientBase.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/CECentralMessagerClientBase.hpp"
#include "areg/component/CEComponentThread.hpp"
#include "areg/component/CEComponent.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NECentralMessager
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CECentralMessagerClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &CECentralMessagerClientBase::RequestSendMessageFailed
        , &CECentralMessagerClientBase::RequestKeyTypingFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CECentralMessagerClientBase::CECentralMessagerClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CECentralMessagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CECentralMessagerClientBase::CECentralMessagerClientBase( const char* roleName, CEDispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CECentralMessagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CECentralMessagerClientBase::CECentralMessagerClientBase( const char* roleName, CEComponent & owner )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CECentralMessagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), owner.GetMasterThread()) )
{
    ; // do nothing
}

CECentralMessagerClientBase::~CECentralMessagerClientBase( void )
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

bool CECentralMessagerClientBase::RecreateProxy( void )
{
    bool result         = false;
    CEString roleName   = mProxy != NULL ? mProxy->GetProxyAddress().GetRoleName() : "";
    CEString threadName = mProxy != NULL ? mProxy->GetProxyAddress().GetThread()   : "";
    if ( roleName.IsEmpty() == false )
    {
        CECentralMessagerProxy * newProxy = CECentralMessagerProxy::CreateProxy(roleName.GetBuffer(), static_cast<IEProxyListener &>(self()), threadName.GetBuffer());
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

CEDispatcherThread * CECentralMessagerClientBase::GetDispatcherThread( void )
{
    return ( mProxy != static_cast<CECentralMessagerProxy *>(NULL) ? &(mProxy->GetProxyDispatcherThread()) : static_cast<CEDispatcherThread *>(NULL) );
}

void CECentralMessagerClientBase::ClearAllNotifications( void )
{
    mProxy->ClearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_ServiceConnected);
bool CECentralMessagerClientBase::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]", proxy.GetProxyAddress().GetServiceName().GetBuffer(), proxy.GetProxyAddress().GetRoleName().GetBuffer(), isConnected ? "CONNECTED" : "DISCONNECTED");
        mIsConnected= isConnected;
        result      = true;
    }
    return result;
}

void CECentralMessagerClientBase::NotifyOn( NECentralMessager::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->SetNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->ClearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_ProcessNotificationEvent);
void CECentralMessagerClientBase::ProcessNotificationEvent( CENotificationEvent & eventElem )
{
    const CENotificationEventData & data= static_cast<const CENotificationEvent &>(eventElem).GetData();
    NEService::eResultType result       = data.GetNotifyType();
    NECentralMessager::eMessageIDs msgId   = static_cast<NECentralMessager::eMessageIDs>(data.GetNotifyId());
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
        break;

    case NEService::RESULT_OK:
        {
            switch (msgId)
            {
        /************************************************************************
         * Trigger response processing
         ************************************************************************/
        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NECentralMessager::MSG_ID_BroadcastSendMessage:
                {
                    const CEString & nickName = mProxy->GetParamnickName();
                    const unsigned int & cookie = mProxy->GetParamcookie();
                    const CEString & newMessage = mProxy->GetParamnewMessage();
                    const CEDateTime & dateTime = mProxy->GetParamdateTime();
                    BroadcastSendMessage( nickName, cookie, newMessage, dateTime );
                }
                break;

            case NECentralMessager::MSG_ID_BroadcastKeyTyping:
                {
                    const CEString & nickName = mProxy->GetParamnickName();
                    const unsigned int & cookie = mProxy->GetParamcookie();
                    const CEString & newMessage = mProxy->GetParamnewMessage();
                    BroadcastKeyTyping( nickName, cookie, newMessage );
                }
                break;

            case NECentralMessager::MSG_ID_BroadcastBroadcastMessage:
                {
                    const CEString & serverMessage = mProxy->GetParamserverMessage();
                    const CEDateTime & dateTime = mProxy->GetParamdateTime();
                    BroadcastBroadcastMessage( serverMessage, dateTime );
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CECentralMessagerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_ProcessNotificationEvent);
            TRACE_ERR("Client object CECentralMessagerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_InvalidResponse);
void CECentralMessagerClientBase::InvalidResponse( NECentralMessager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_InvalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client CECentralMessagerClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NECentralMessager::GetString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    CEClientBase::ResponseInvalidNotImpelemnted("CECentralMessagerClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_InvalidRequest);
void CECentralMessagerClientBase::InvalidRequest( NECentralMessager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_InvalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client CECentralMessagerClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NECentralMessager::GetString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_RequestFailed);
void CECentralMessagerClientBase::RequestFailed( NECentralMessager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shared_generated_CECentralMessagerClientBase_RequestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of CECentralMessagerClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NECentralMessager::GetString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                    , NEService::GetString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NECentralMessager::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::IsResponseId(static_cast<unsigned int>(FailureMsgId)) ? NECentralMessager::GetRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::IsRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NECentralMessager::MSG_ID_INVALID);

    if ( index != static_cast<unsigned int>(NECentralMessager::MSG_ID_INVALID) )
    {
        (this->*NECentralMessager::failureFunctions[index])( FailureReason );
    }
    else
    {
        InvalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void CECentralMessagerClientBase::RequestSendMessageFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CECentralMessagerClientBase", static_cast<unsigned int>(NECentralMessager::MSG_ID_RequestSendMessage) );
}

void CECentralMessagerClientBase::RequestKeyTypingFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CECentralMessagerClientBase", static_cast<unsigned int>(NECentralMessager::MSG_ID_RequestKeyTyping) );
}

void CECentralMessagerClientBase::BroadcastSendMessage( const CEString & /* nickName */, const unsigned int & /* cookie */, const CEString & /* newMessage */, const CEDateTime & /* dateTime */ )
{
    CEClientBase::BroadcastNotImplemented( "CECentralMessagerClientBase", static_cast<unsigned int>(NECentralMessager::MSG_ID_BroadcastSendMessage) );
}

void CECentralMessagerClientBase::BroadcastKeyTyping( const CEString & /* nickName */, const unsigned int & /* cookie */, const CEString & /* newMessage */ )
{
    CEClientBase::BroadcastNotImplemented( "CECentralMessagerClientBase", static_cast<unsigned int>(NECentralMessager::MSG_ID_BroadcastKeyTyping) );
}

void CECentralMessagerClientBase::BroadcastBroadcastMessage( const CEString & /* serverMessage */, const CEDateTime & /* dateTime */ )
{
    CEClientBase::BroadcastNotImplemented( "CECentralMessagerClientBase", static_cast<unsigned int>(NECentralMessager::MSG_ID_BroadcastBroadcastMessage) );
}


//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CECentralMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
