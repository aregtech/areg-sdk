//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/SystemClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     12.05.2021  16:41:13 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/SystemClientBase.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "src/gen/SystemClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NESystem
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (SystemClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &SystemClientBase::requestPowerOffFailed
        , &SystemClientBase::requestPowerOnFailed
        , &SystemClientBase::requestShutdownFailed
        , &SystemClientBase::requestStartFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// SystemClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

SystemClientBase::SystemClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SystemProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

SystemClientBase::SystemClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SystemProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

SystemClientBase::SystemClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SystemProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

SystemClientBase::~SystemClientBase( void )
{
    if (mProxy != NULL)
    {
        mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
        mProxy->freeProxy( static_cast<IEProxyListener &>(self()) );
        mProxy  = NULL;
    }
    
    mIsConnected= false;
}

/************************************************************************
 * Operations
 ************************************************************************/

bool SystemClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        SystemProxy * newProxy = SystemProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
        if (newProxy != NULL)
        {
            mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
            mProxy->freeProxy (static_cast<IEProxyListener &>(self()) );
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

DispatcherThread * SystemClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<SystemProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

void SystemClientBase::clearAllNotifications( void )
{
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(src_gen_SystemClientBase_ServiceConnected);
bool SystemClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(src_gen_SystemClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]"
                 , proxy.getProxyAddress().getServiceName().getString()
                 , proxy.getProxyAddress().getRoleName().getString()
                 , isConnected ? "CONNECTED" : "DISCONNECTED");
        
        mIsConnected= isConnected;
        result      = true;
    }
    
    return result;
}

void SystemClientBase::notifyOn( NESystem::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_SystemClientBase_processNotificationEvent);
void SystemClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NESystem::eMessageIDs msgId   = static_cast<NESystem::eMessageIDs>(data.getNotifyId());
    mCurrSequenceNr = data.getSequenceNr();

    switch (result)
    {
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_CANCELED:
        {
        /************************************************************************
         * Trigger request error handling if request failed.
         ************************************************************************/
            requestFailed(msgId, result);
        }
        break;

    case NEService::RESULT_INVALID:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification 
         ************************************************************************/
            invalidResponse(msgId);
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
                    NESystem::eSystemState SystemState = mProxy->getSystemState(dataState);
                    onSystemStateUpdate(SystemState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_SystemClientBase_processNotificationEvent);
                    TRACE_ERR("Client object SystemClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
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
            TRACE_SCOPE(src_gen_SystemClientBase_processNotificationEvent);
            TRACE_ERR("Client object SystemClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
                        , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                        , static_cast<int>(result));
            ASSERT(false);
        }
        break;
    }
}

/************************************************************************
 * Error handling calls
 ************************************************************************/
DEF_TRACE_SCOPE(src_gen_SystemClientBase_invalidResponse);
void SystemClientBase::invalidResponse( NESystem::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(src_gen_SystemClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client SystemClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NESystem::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("SystemClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(src_gen_SystemClientBase_invalidRequest);
void SystemClientBase::invalidRequest( NESystem::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(src_gen_SystemClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client SystemClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NESystem::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(src_gen_SystemClientBase_requestFailed);
void SystemClientBase::requestFailed( NESystem::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(src_gen_SystemClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of SystemClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NESystem::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NESystem::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NESystem::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NESystem::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NESystem::MSG_ID_INVALID) )
    {
        (this->*NESystem::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void SystemClientBase::onSystemStateUpdate( NESystem::eSystemState /* SystemState */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "SystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_SystemState) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void SystemClientBase::requestPowerOffFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "SystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_requestPowerOff) );
}

void SystemClientBase::requestPowerOnFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "SystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_requestPowerOn) );
}

void SystemClientBase::requestShutdownFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "SystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_requestShutdown) );
}

void SystemClientBase::requestStartFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "SystemClientBase", static_cast<unsigned int>(NESystem::MSG_ID_requestStart) );
}


//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/SystemClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
