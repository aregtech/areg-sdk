//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectConnectionClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     25.04.2021  23:50:45 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectConnectionClientBase.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/CEDirectConnectionClientBase.hpp"
#include "areg/component/CEComponentThread.hpp"
#include "areg/component/CEComponent.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEDirectConnection
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CEDirectConnectionClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &CEDirectConnectionClientBase::RequestConnectoinSetupFailed
        , &CEDirectConnectionClientBase::RequestAddParticipantFailed
        , &CEDirectConnectionClientBase::RequestRemoveParticipantFailed
        , &CEDirectConnectionClientBase::RequestCloseConnectionFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CEDirectConnectionClientBase::CEDirectConnectionClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEDirectConnectionProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEDirectConnectionClientBase::CEDirectConnectionClientBase( const char* roleName, CEDispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEDirectConnectionProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEDirectConnectionClientBase::CEDirectConnectionClientBase( const char* roleName, CEComponent & owner )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEDirectConnectionProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), owner.GetMasterThread()) )
{
    ; // do nothing
}

CEDirectConnectionClientBase::~CEDirectConnectionClientBase( void )
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

bool CEDirectConnectionClientBase::RecreateProxy( void )
{
    bool result         = false;
    CEString roleName   = mProxy != NULL ? mProxy->GetProxyAddress().GetRoleName() : "";
    CEString threadName = mProxy != NULL ? mProxy->GetProxyAddress().GetThread()   : "";
    if ( roleName.IsEmpty() == false )
    {
        CEDirectConnectionProxy * newProxy = CEDirectConnectionProxy::CreateProxy(roleName.GetBuffer(), static_cast<IEProxyListener &>(self()), threadName.GetBuffer());
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

CEDispatcherThread * CEDirectConnectionClientBase::GetDispatcherThread( void )
{
    return ( mProxy != static_cast<CEDirectConnectionProxy *>(NULL) ? &(mProxy->GetProxyDispatcherThread()) : static_cast<CEDispatcherThread *>(NULL) );
}

void CEDirectConnectionClientBase::ClearAllNotifications( void )
{
    mProxy->ClearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_ServiceConnected);
bool CEDirectConnectionClientBase::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]", proxy.GetProxyAddress().GetServiceName().GetBuffer(), proxy.GetProxyAddress().GetRoleName().GetBuffer(), isConnected ? "CONNECTED" : "DISCONNECTED");
        mIsConnected= isConnected;
        result      = true;
    }
    return result;
}

void CEDirectConnectionClientBase::NotifyOn( NEDirectConnection::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->SetNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->ClearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_ProcessNotificationEvent);
void CEDirectConnectionClientBase::ProcessNotificationEvent( CENotificationEvent & eventElem )
{
    const CENotificationEventData & data= static_cast<const CENotificationEvent &>(eventElem).GetData();
    NEService::eResultType result       = data.GetNotifyType();
    NEDirectConnection::eMessageIDs msgId   = static_cast<NEDirectConnection::eMessageIDs>(data.GetNotifyId());
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
            case NEDirectConnection::MSG_ID_InitiatedConnections:
                {
                    const NEDirectConnection::MapParticipants & InitiatedConnections = mProxy->GetInitiatedConnections(dataState);
                    OnInitiatedConnectionsUpdate(InitiatedConnections, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEDirectConnectionClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEDirectConnection::MSG_ID_ResponseConnectoinSetup:
                {
                    const bool & succeeded = mProxy->GetParamsucceeded();
                    const NEDirectConnection::sParticipant & target = mProxy->GetParamtarget();
                    const NEDirectConnection::sInitiator & initiator = mProxy->GetParaminitiator();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->GetParamlistParticipants();
                    ResponseConnectoinSetup( succeeded, target, initiator, listParticipants );
                }
                break;

            case NEDirectConnection::MSG_ID_ResponseAddParticipant:
                {
                    const bool & succeeded = mProxy->GetParamsucceeded();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->GetParamlistParticipants();
                    ResponseAddParticipant( succeeded, listParticipants );
                }
                break;

            case NEDirectConnection::MSG_ID_ResponseRemoveParticipant:
                {
                    const bool & succeeded = mProxy->GetParamsucceeded();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->GetParamlistParticipants();
                    ResponseRemoveParticipant( succeeded, listParticipants );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEDirectConnectionClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_ProcessNotificationEvent);
            TRACE_ERR("Client object CEDirectConnectionClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_InvalidResponse);
void CEDirectConnectionClientBase::InvalidResponse( NEDirectConnection::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_InvalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client CEDirectConnectionClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEDirectConnection::GetString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    CEClientBase::ResponseInvalidNotImpelemnted("CEDirectConnectionClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_InvalidRequest);
void CEDirectConnectionClientBase::InvalidRequest( NEDirectConnection::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_InvalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client CEDirectConnectionClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEDirectConnection::GetString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_RequestFailed);
void CEDirectConnectionClientBase::RequestFailed( NEDirectConnection::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shared_generated_CEDirectConnectionClientBase_RequestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of CEDirectConnectionClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEDirectConnection::GetString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                    , NEService::GetString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEDirectConnection::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::IsResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEDirectConnection::GetRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::IsRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEDirectConnection::MSG_ID_INVALID);

    if ( index != static_cast<unsigned int>(NEDirectConnection::MSG_ID_INVALID) )
    {
        (this->*NEDirectConnection::failureFunctions[index])( FailureReason );
    }
    else
    {
        InvalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void CEDirectConnectionClientBase::OnInitiatedConnectionsUpdate( const NEDirectConnection::MapParticipants & /* InitiatedConnections */, NEService::eDataStateType /* state */ )
{
    CEClientBase::OnUpdateNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_InitiatedConnections) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void CEDirectConnectionClientBase::RequestConnectoinSetupFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_RequestConnectoinSetup) );
}

void CEDirectConnectionClientBase::RequestAddParticipantFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_RequestAddParticipant) );
}

void CEDirectConnectionClientBase::RequestRemoveParticipantFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_RequestRemoveParticipant) );
}

void CEDirectConnectionClientBase::RequestCloseConnectionFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_RequestCloseConnection) );
}

void CEDirectConnectionClientBase::ResponseConnectoinSetup( const bool & /* succeeded */, const NEDirectConnection::sParticipant & /* target */, const NEDirectConnection::sInitiator & /* initiator */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    CEClientBase::ResponseNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_ResponseConnectoinSetup) );
}

void CEDirectConnectionClientBase::ResponseAddParticipant( const bool & /* succeeded */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    CEClientBase::ResponseNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_ResponseAddParticipant) );
}

void CEDirectConnectionClientBase::ResponseRemoveParticipant( const bool & /* succeeded */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    CEClientBase::ResponseNotImplemented( "CEDirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_ResponseRemoveParticipant) );
}


//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectConnectionClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
