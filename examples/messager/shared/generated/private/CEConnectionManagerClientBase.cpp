//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEConnectionManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     25.04.2021  23:50:44 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEConnectionManagerClientBase.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/CEConnectionManagerClientBase.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEConnectionManager
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CEConnectionManagerClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &CEConnectionManagerClientBase::RequestConnetFailed
        , &CEConnectionManagerClientBase::RequestRegisterConnectionFailed
        , &CEConnectionManagerClientBase::RequestDiconnectFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CEConnectionManagerClientBase::CEConnectionManagerClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEConnectionManagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEConnectionManagerClientBase::CEConnectionManagerClientBase( const char* roleName, CEDispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEConnectionManagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CEConnectionManagerClientBase::CEConnectionManagerClientBase( const char* roleName, CEComponent & owner )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CEConnectionManagerProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), owner.GetMasterThread()) )
{
    ; // do nothing
}

CEConnectionManagerClientBase::~CEConnectionManagerClientBase( void )
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

bool CEConnectionManagerClientBase::RecreateProxy( void )
{
    bool result         = false;
    CEString roleName   = mProxy != NULL ? mProxy->GetProxyAddress().GetRoleName() : "";
    CEString threadName = mProxy != NULL ? mProxy->GetProxyAddress().GetThread()   : "";
    if ( roleName.IsEmpty() == false )
    {
        CEConnectionManagerProxy * newProxy = CEConnectionManagerProxy::CreateProxy(roleName.GetBuffer(), static_cast<IEProxyListener &>(self()), threadName.GetBuffer());
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

CEDispatcherThread * CEConnectionManagerClientBase::GetDispatcherThread( void )
{
    return ( mProxy != static_cast<CEConnectionManagerProxy *>(NULL) ? &(mProxy->GetProxyDispatcherThread()) : static_cast<CEDispatcherThread *>(NULL) );
}

void CEConnectionManagerClientBase::ClearAllNotifications( void )
{
    mProxy->ClearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_ServiceConnected);
bool CEConnectionManagerClientBase::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]", proxy.GetProxyAddress().GetServiceName().GetBuffer(), proxy.GetProxyAddress().GetRoleName().GetBuffer(), isConnected ? "CONNECTED" : "DISCONNECTED");
        mIsConnected= isConnected;
        result      = true;
    }
    return result;
}

void CEConnectionManagerClientBase::NotifyOn( NEConnectionManager::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->SetNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->ClearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_ProcessNotificationEvent);
void CEConnectionManagerClientBase::ProcessNotificationEvent( CENotificationEvent & eventElem )
{
    const CENotificationEventData & data= static_cast<const CENotificationEvent &>(eventElem).GetData();
    NEService::eResultType result       = data.GetNotifyType();
    NEConnectionManager::eMessageIDs msgId   = static_cast<NEConnectionManager::eMessageIDs>(data.GetNotifyId());
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
            case NEConnectionManager::MSG_ID_ConnectionList:
                {
                    const NEConnectionManager::MapConnection & ConnectionList = mProxy->GetConnectionList(dataState);
                    OnConnectionListUpdate(ConnectionList, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEConnectionManagerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEConnectionManager::MSG_ID_ResponseConnect:
                {
                    const CEString & nickName = mProxy->GetParamnickName();
                    const unsigned int & cookie = mProxy->GetParamcookie();
                    const CEDateTime & dateTime = mProxy->GetParamdateTime();
                    const NEConnectionManager::eConnectionResult & result = mProxy->GetParamresult();
                    ResponseConnect( nickName, cookie, dateTime, result );
                }
                break;

            case NEConnectionManager::MSG_ID_ResponseRegisterConnection:
                {
                    const NEConnectionManager::sConnection & connection = mProxy->GetParamconnection();
                    const NEConnectionManager::ListConnection & connectionList = mProxy->GetParamconnectionList();
                    const bool & success = mProxy->GetParamsuccess();
                    ResponseRegisterConnection( connection, connectionList, success );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NEConnectionManager::MSG_ID_BroadcastConnectionUpdated:
                {
                    const NEConnectionManager::MapConnection & updatedList = mProxy->GetParamupdatedList();
                    BroadcastConnectionUpdated( updatedList );
                }
                break;

            case NEConnectionManager::MSG_ID_BroadcastClientConnected:
                {
                    const NEConnectionManager::sConnection & clientConnected = mProxy->GetParamclientConnected();
                    BroadcastClientConnected( clientConnected );
                }
                break;

            case NEConnectionManager::MSG_ID_BroadcastClientDisconnected:
                {
                    const NEConnectionManager::sConnection & clientLeft = mProxy->GetParamclientLeft();
                    BroadcastClientDisconnected( clientLeft );
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CEConnectionManagerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_ProcessNotificationEvent);
            TRACE_ERR("Client object CEConnectionManagerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_InvalidResponse);
void CEConnectionManagerClientBase::InvalidResponse( NEConnectionManager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_InvalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client CEConnectionManagerClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEConnectionManager::GetString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    CEClientBase::ResponseInvalidNotImpelemnted("CEConnectionManagerClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_InvalidRequest);
void CEConnectionManagerClientBase::InvalidRequest( NEConnectionManager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_InvalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client CEConnectionManagerClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEConnectionManager::GetString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_RequestFailed);
void CEConnectionManagerClientBase::RequestFailed( NEConnectionManager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shared_generated_CEConnectionManagerClientBase_RequestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of CEConnectionManagerClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEConnectionManager::GetString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                    , NEService::GetString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEConnectionManager::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::IsResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEConnectionManager::GetRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::IsRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEConnectionManager::MSG_ID_INVALID);

    if ( index != static_cast<unsigned int>(NEConnectionManager::MSG_ID_INVALID) )
    {
        (this->*NEConnectionManager::failureFunctions[index])( FailureReason );
    }
    else
    {
        InvalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void CEConnectionManagerClientBase::OnConnectionListUpdate( const NEConnectionManager::MapConnection & /* ConnectionList */, NEService::eDataStateType /* state */ )
{
    CEClientBase::OnUpdateNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_ConnectionList) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void CEConnectionManagerClientBase::RequestConnetFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_RequestConnet) );
}

void CEConnectionManagerClientBase::RequestRegisterConnectionFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_RequestRegisterConnection) );
}

void CEConnectionManagerClientBase::RequestDiconnectFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_RequestDiconnect) );
}

void CEConnectionManagerClientBase::ResponseConnect( const CEString & /* nickName */, const unsigned int & /* cookie */, const CEDateTime & /* dateTime */, const NEConnectionManager::eConnectionResult & /* result */ )
{
    CEClientBase::ResponseNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_ResponseConnect) );
}

void CEConnectionManagerClientBase::ResponseRegisterConnection( const NEConnectionManager::sConnection & /* connection */, const NEConnectionManager::ListConnection & /* connectionList */, const bool & /* success */ )
{
    CEClientBase::ResponseNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_ResponseRegisterConnection) );
}

void CEConnectionManagerClientBase::BroadcastConnectionUpdated( const NEConnectionManager::MapConnection & /* updatedList */ )
{
    CEClientBase::BroadcastNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_BroadcastConnectionUpdated) );
}

void CEConnectionManagerClientBase::BroadcastClientConnected( const NEConnectionManager::sConnection & /* clientConnected */ )
{
    CEClientBase::BroadcastNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_BroadcastClientConnected) );
}

void CEConnectionManagerClientBase::BroadcastClientDisconnected( const NEConnectionManager::sConnection & /* clientLeft */ )
{
    CEClientBase::BroadcastNotImplemented( "CEConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_BroadcastClientDisconnected) );
}


//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEConnectionManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
