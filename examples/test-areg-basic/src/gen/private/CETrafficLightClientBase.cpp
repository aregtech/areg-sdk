//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CETrafficLightClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source TrafficLight.
 * Generated at     21.04.2021  18:33:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CETrafficLightClientBase.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "src/gen/CETrafficLightClientBase.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NETrafficLight
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CETrafficLightClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
        &CETrafficLightClientBase::RequestChangeLightFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CETrafficLightClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CETrafficLightClientBase::CETrafficLightClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CETrafficLightProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CETrafficLightClientBase::CETrafficLightClientBase( const char* roleName, CEDispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CETrafficLightProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

CETrafficLightClientBase::CETrafficLightClientBase( const char* roleName, CEComponent & owner )
    : IEProxyListener   ( )
    , CEClientBase      ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CETrafficLightProxy::CreateProxy(roleName, static_cast<IEProxyListener &>(self()), owner.GetMasterThread()) )
{
    ; // do nothing
}

CETrafficLightClientBase::~CETrafficLightClientBase( void )
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

bool CETrafficLightClientBase::RecreateProxy( void )
{
    bool result         = false;
    CEString roleName   = mProxy != NULL ? mProxy->GetProxyAddress().GetRoleName() : "";
    CEString threadName = mProxy != NULL ? mProxy->GetProxyAddress().GetThread()   : "";
    if ( roleName.IsEmpty() == false )
    {
        CETrafficLightProxy * newProxy = CETrafficLightProxy::CreateProxy(roleName.GetBuffer(), static_cast<IEProxyListener &>(self()), threadName.GetBuffer());
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

CEDispatcherThread * CETrafficLightClientBase::GetDispatcherThread( void )
{
    return ( mProxy != static_cast<CETrafficLightProxy *>(NULL) ? &(mProxy->GetProxyDispatcherThread()) : static_cast<CEDispatcherThread *>(NULL) );
}

void CETrafficLightClientBase::ClearAllNotifications( void )
{
    mProxy->ClearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(src_gen_CETrafficLightClientBase_ServiceConnected);
bool CETrafficLightClientBase::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE(src_gen_CETrafficLightClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]", proxy.GetProxyAddress().GetServiceName().GetBuffer(), proxy.GetProxyAddress().GetRoleName().GetBuffer(), isConnected ? "CONNECTED" : "DISCONNECTED");
        mIsConnected= isConnected;
        result      = true;
    }
    return result;
}

void CETrafficLightClientBase::NotifyOn( NETrafficLight::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->SetNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->ClearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_CETrafficLightClientBase_ProcessNotificationEvent);
void CETrafficLightClientBase::ProcessNotificationEvent( CENotificationEvent & eventElem )
{
    const CENotificationEventData & data= static_cast<const CENotificationEvent &>(eventElem).GetData();
    NEService::eResultType result       = data.GetNotifyType();
    NETrafficLight::eMessageIDs msgId   = static_cast<NETrafficLight::eMessageIDs>(data.GetNotifyId());
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
            case NETrafficLight::MSG_ID_TrafficLight:
                {
                    const NETrafficLight::eLight & TrafficLight = mProxy->GetTrafficLight(dataState);
                    OnTrafficLightUpdate(TrafficLight, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_CETrafficLightClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CETrafficLightClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NETrafficLight::MSG_ID_ResponseChangeLight:
                {
                    const NETrafficLight::eLight & lightColor = mProxy->GetParamlightColor();
                    ResponseChangeLight( lightColor );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(src_gen_CETrafficLightClientBase_ProcessNotificationEvent);
                    TRACE_ERR("Client object CETrafficLightClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(src_gen_CETrafficLightClientBase_ProcessNotificationEvent);
            TRACE_ERR("Client object CETrafficLightClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(src_gen_CETrafficLightClientBase_InvalidResponse);
void CETrafficLightClientBase::InvalidResponse( NETrafficLight::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(src_gen_CETrafficLightClientBase_InvalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client CETrafficLightClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NETrafficLight::GetString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    CEClientBase::ResponseInvalidNotImpelemnted("CETrafficLightClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(src_gen_CETrafficLightClientBase_InvalidRequest);
void CETrafficLightClientBase::InvalidRequest( NETrafficLight::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(src_gen_CETrafficLightClientBase_InvalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client CETrafficLightClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NETrafficLight::GetString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(src_gen_CETrafficLightClientBase_RequestFailed);
void CETrafficLightClientBase::RequestFailed( NETrafficLight::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(src_gen_CETrafficLightClientBase_RequestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of CETrafficLightClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NETrafficLight::GetString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , CEProxyAddress::ConvertAddressToPath(mProxy->GetProxyAddress()).GetBuffer()
                    , NEService::GetString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NETrafficLight::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::IsResponseId(static_cast<unsigned int>(FailureMsgId)) ? NETrafficLight::GetRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::IsRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NETrafficLight::MSG_ID_INVALID);

    if ( index != static_cast<unsigned int>(NETrafficLight::MSG_ID_INVALID) )
    {
        (this->*NETrafficLight::failureFunctions[index])( FailureReason );
    }
    else
    {
        InvalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void CETrafficLightClientBase::OnTrafficLightUpdate( const NETrafficLight::eLight & /* TrafficLight */, NEService::eDataStateType /* state */ )
{
    CEClientBase::OnUpdateNotImplemented( "CETrafficLightClientBase", static_cast<unsigned int>(NETrafficLight::MSG_ID_TrafficLight) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void CETrafficLightClientBase::RequestChangeLightFailed( NEService::eResultType /* FailureReason */ )
{
    CEClientBase::RequestFailedNotImplemented( "CETrafficLightClientBase", static_cast<unsigned int>(NETrafficLight::MSG_ID_RequestChangeLight) );
}

void CETrafficLightClientBase::ResponseChangeLight( const NETrafficLight::eLight & /* lightColor */ )
{
    CEClientBase::ResponseNotImplemented( "CETrafficLightClientBase", static_cast<unsigned int>(NETrafficLight::MSG_ID_ResponseChangeLight) );
}


//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CETrafficLightClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
