//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CESystemClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source System.
 * Generated at     21.04.2021  18:33:57 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CESystemClientBase.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "src/gen/CESystemClientBase.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NESystem
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CESystemClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &CESystemClientBase::RequestPowerOffFailed
        , &CESystemClientBase::RequestPowerOnFailed
        , &CESystemClientBase::RequestShutdownFailed
        , &CESystemClientBase::RequestStartFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CESystemClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CESystemClientBase::CESystemClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CESystemProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CESystemClientBase::CESystemClientBase( const char* roleName, CEDispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CESystemProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CESystemClientBase::CESystemClientBase( const char* roleName, CEComponent & owner )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CESystemProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), owner.GetMasterThread()) )
{
    ; // do nothing
}

CESystemClientBase::~CESystemClientBase( void )
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

bool CESystemClientBase::RecreateProxy( void )
{
    bool result         = false;
    CEString roleName   = mProxy != NULL ? mProxy->GetProxyAddress().GetRoleName() : "";
    CEString threadName = mProxy != NULL ? mProxy->GetProxyAddress().GetThread()   : "";
    if ( roleName.IsEmpty() == false )
    {
        CESystemProxy * newProxy = CESystemProxy::CreateProxy(roleName.GetBuffer(), static_cast<IEProxyListener &>(self()), threadName.GetBuffer());
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

CEDispatcherThread * CESystemClientBase::GetDispatcherThread( void )
{
    return ( mProxy != static_cast<CESystemProxy *>(NULL) ? &(mProxy->GetProxyDispatcherThread()) : static_cast<CEDispatcherThread *>(NULL) );
}

void CESystemClientBase::ClearAllNotifications( void )
{
    mProxy->ClearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(src_gen_CESystemClientBase_ServiceConnected);
bool CESystemClientBase::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE(src_gen_CESystemClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]", proxy.GetProxyAddress().GetServiceName().GetBuffer(), proxy.GetProxyAddress().GetRoleName().GetBuffer(), isConnected ? "CONNECTED" : "DISCONNECTED");
        mIsConnected= isConnected;
        result      = true;
    }
    return result;
}

void CESystemClientBase::NotifyOn( NESystem::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->SetNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->ClearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_CESystemClientBase_ProcessNotificationEvent);
void CESystemClientBase::ProcessNotificationEvent( CENotificationEvent & eventElem )
{
    const CENotificationEventData & data= static_cast<const CENotificationEvent &>(eventElem).GetData();
    NEService::eResultType result       = data.GetNotifyType();
    NESystem::eMessageIDs msgId   = static_cast<NESystem::eMessageIDs>(data.GetNotifyId());
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
            case NESystem::MSG_ID_SystemState:
                {
                    const NESystem::eSystemState & SystemState = mProxy->GetSystemState(dataState);
                    OnSystemStateUpdate(SystemState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_CESystemClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CESystemClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::RESULT_OK:        
        break;

    default:
        {
            TRACE_SCOPE(src_gen_CESystemClientBase_ProcessNotificationEvent);
            TRACE_ERR("Client object CESystemClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(src_gen_CESystemClientBase_InvalidResponse);
void CESystemClientBase::InvalidResponse( NESystem::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(src_gen_CESystemClientBase_InvalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client CESystemClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NESystem::GetString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    CEClientBase::ResponseInvalidNotImpelemnted("CESystemClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(src_gen_CESystemClientBase_InvalidRequest);
void CESystemClientBase::InvalidRequest( NESystem::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(src_gen_CESystemClientBase_InvalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client CESystemClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NESystem::GetString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(src_gen_CESystemClientBase_RequestFailed);
void CESystemClientBase::RequestFailed( NESystem::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(src_gen_CESystemClientBase_RequestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of CESystemClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NESystem::GetString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                    , NEService::GetString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NESystem::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::IsResponseId(static_cast<unsigned int>(FailureMsgId)) ? NESystem::GetRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::IsRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NESystem::MSG_ID_INVALID);

    if ( index != static_cast<unsigned int>(NESystem::MSG_ID_INVALID) )
    {
        (this->*NESystem::failureFunctions[index])( FailureReason );
    }
    else
    {
        InvalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void CESystemClientBase::OnSystemStateUpdate( const NESystem::eSystemState & /* SystemState */, NEService::eDataStateType /* state */ )
{
    CEClientBase::OnUpdateNotImplemented( "CESystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_SystemState) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void CESystemClientBase::RequestPowerOffFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CESystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_RequestPowerOff) );
}

void CESystemClientBase::RequestPowerOnFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CESystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_RequestPowerOn) );
}

void CESystemClientBase::RequestShutdownFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CESystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_RequestShutdown) );
}

void CESystemClientBase::RequestStartFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CESystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_RequestStart) );
}


//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CESystemClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
