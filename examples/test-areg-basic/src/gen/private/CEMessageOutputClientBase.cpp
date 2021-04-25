//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CEMessageOutputClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CEMessageOutputClientBase.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "src/gen/CEMessageOutputClientBase.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEMessageOutput
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CEMessageOutputClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
        &CEMessageOutputClientBase::RequestOutputMessageFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CEMessageOutputClientBase::CEMessageOutputClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEMessageOutputProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEMessageOutputClientBase::CEMessageOutputClientBase( const char* roleName, CEDispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEMessageOutputProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEMessageOutputClientBase::CEMessageOutputClientBase( const char* roleName, CEComponent & owner )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEMessageOutputProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), owner.GetMasterThread()) )
{
    ; // do nothing
}

CEMessageOutputClientBase::~CEMessageOutputClientBase( void )
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

bool CEMessageOutputClientBase::RecreateProxy( void )
{
    bool result         = false;
    CEString roleName   = mProxy != NULL ? mProxy->GetProxyAddress().GetRoleName() : "";
    CEString threadName = mProxy != NULL ? mProxy->GetProxyAddress().GetThread()   : "";
    if ( roleName.IsEmpty() == false )
    {
        CEMessageOutputProxy * newProxy = CEMessageOutputProxy::CreateProxy(roleName.GetBuffer(), static_cast<IEProxyListener &>(self()), threadName.GetBuffer());
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

CEDispatcherThread * CEMessageOutputClientBase::GetDispatcherThread( void )
{
    return ( mProxy != static_cast<CEMessageOutputProxy *>(NULL) ? &(mProxy->GetProxyDispatcherThread()) : static_cast<CEDispatcherThread *>(NULL) );
}

void CEMessageOutputClientBase::ClearAllNotifications( void )
{
    mProxy->ClearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(src_gen_CEMessageOutputClientBase_ServiceConnected);
bool CEMessageOutputClientBase::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE(src_gen_CEMessageOutputClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]", proxy.GetProxyAddress().GetServiceName().GetBuffer(), proxy.GetProxyAddress().GetRoleName().GetBuffer(), isConnected ? "CONNECTED" : "DISCONNECTED");
        mIsConnected= isConnected;
        result      = true;
    }
    return result;
}

void CEMessageOutputClientBase::NotifyOn( NEMessageOutput::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->SetNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->ClearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_CEMessageOutputClientBase_ProcessNotificationEvent);
void CEMessageOutputClientBase::ProcessNotificationEvent( CENotificationEvent & eventElem )
{
    const CENotificationEventData & data= static_cast<const CENotificationEvent &>(eventElem).GetData();
    NEService::eResultType result       = data.GetNotifyType();
    NEMessageOutput::eMessageIDs msgId   = static_cast<NEMessageOutput::eMessageIDs>(data.GetNotifyId());
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
            case NEMessageOutput::MSG_ID_LastMessage:
                {
                    const CEString & LastMessage = mProxy->GetLastMessage(dataState);
                    OnLastMessageUpdate(LastMessage, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_CEMessageOutputClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEMessageOutputClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NEMessageOutput::MSG_ID_BroadcastActionRequest:
                {
                    const NEMessageOutput::eRequestedActons & userAction = mProxy->GetParamuserAction();
                    BroadcastActionRequest( userAction );
                }
                break;

            case NEMessageOutput::MSG_ID_BroadcastInputMessage:
                {
                    const CEString & msgInput = mProxy->GetParammsgInput();
                    BroadcastInputMessage( msgInput );
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_CEMessageOutputClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEMessageOutputClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(src_gen_CEMessageOutputClientBase_ProcessNotificationEvent);
            TRACE_ERR("Client object CEMessageOutputClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(src_gen_CEMessageOutputClientBase_InvalidResponse);
void CEMessageOutputClientBase::InvalidResponse( NEMessageOutput::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(src_gen_CEMessageOutputClientBase_InvalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client CEMessageOutputClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEMessageOutput::GetString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    CEClientBase::ResponseInvalidNotImpelemnted("CEMessageOutputClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(src_gen_CEMessageOutputClientBase_InvalidRequest);
void CEMessageOutputClientBase::InvalidRequest( NEMessageOutput::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(src_gen_CEMessageOutputClientBase_InvalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client CEMessageOutputClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEMessageOutput::GetString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(src_gen_CEMessageOutputClientBase_RequestFailed);
void CEMessageOutputClientBase::RequestFailed( NEMessageOutput::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(src_gen_CEMessageOutputClientBase_RequestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of CEMessageOutputClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEMessageOutput::GetString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                    , NEService::GetString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEMessageOutput::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::IsResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEMessageOutput::GetRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::IsRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEMessageOutput::MSG_ID_INVALID);

    if ( index != static_cast<unsigned int>(NEMessageOutput::MSG_ID_INVALID) )
    {
        (this->*NEMessageOutput::failureFunctions[index])( FailureReason );
    }
    else
    {
        InvalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void CEMessageOutputClientBase::OnLastMessageUpdate( const CEString & /* LastMessage */, NEService::eDataStateType /* state */ )
{
    CEClientBase::OnUpdateNotImplemented( "CEMessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_LastMessage) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void CEMessageOutputClientBase::RequestOutputMessageFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEMessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_RequestOutputMessage) );
}

void CEMessageOutputClientBase::BroadcastActionRequest( const NEMessageOutput::eRequestedActons & /* userAction */ )
{
    CEClientBase::BroadcastNotImplemented( "CEMessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_BroadcastActionRequest) );
}

void CEMessageOutputClientBase::BroadcastInputMessage( const CEString & /* msgInput */ )
{
    CEClientBase::BroadcastNotImplemented( "CEMessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_BroadcastInputMessage) );
}


//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CEMessageOutputClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
