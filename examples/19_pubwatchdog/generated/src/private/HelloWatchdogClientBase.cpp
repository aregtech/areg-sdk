//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWatchdogClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:31 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWatchdog.
 *
 * \file            generated/src/HelloWatchdogClientBase.hpp
 * \ingroup         HelloWatchdog Service Interface
 * \brief           This is an automatic generated code of HelloWatchdog
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/HelloWatchdogClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEHelloWatchdog
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (HelloWatchdogClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &HelloWatchdogClientBase::requestStartSleepFailed
        , &HelloWatchdogClientBase::requestStopServiceFailed
        , &HelloWatchdogClientBase::requestShutdownServiceFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

HelloWatchdogClientBase::HelloWatchdogClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWatchdogProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

HelloWatchdogClientBase::HelloWatchdogClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWatchdogProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

HelloWatchdogClientBase::HelloWatchdogClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWatchdogProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

HelloWatchdogClientBase::HelloWatchdogClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWatchdogProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

HelloWatchdogClientBase::~HelloWatchdogClientBase( void )
{
    if (mProxy != nullptr)
    {
        mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
        mProxy->freeProxy( static_cast<IEProxyListener &>(self()) );
        mProxy  = nullptr;
    }

    mIsConnected= false;
}

/************************************************************************
 * Operations
 ************************************************************************/

bool HelloWatchdogClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            HelloWatchdogProxy * newProxy = HelloWatchdogProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
            if (newProxy != nullptr)
            {
                mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
                mProxy->freeProxy (static_cast<IEProxyListener &>(self()) );
                mProxy = newProxy;
                result = true;
            }
        }
    }

    return result;
}

DispatcherThread * HelloWatchdogClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_serviceConnected);
bool HelloWatchdogClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_serviceConnected);

    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("Client [ %p ]: The Service [ %s ] with Role Name [ %s ] is [ %s ]"
                 , this
                 , proxy.getProxyAddress().getServiceName().getString()
                 , proxy.getProxyAddress().getRoleName().getString()
                 , isConnected ? "CONNECTED" : "DISCONNECTED");

        mIsConnected= isConnected;
        result      = true;
    }

    return result;
}

void HelloWatchdogClientBase::notifyOn( NEHelloWatchdog::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_processNotificationEvent);
void HelloWatchdogClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEHelloWatchdog::eMessageIDs msgId   = static_cast<NEHelloWatchdog::eMessageIDs>(data.getNotifyId());
    mCurrSequenceNr = data.getSequenceNr();

    switch (result)
    {
    case NEService::eResultType::RequestBusy:       // fall through
    case NEService::eResultType::RequestError:      // fall through
    case NEService::eResultType::RequestCanceled:   // fall through
        {
        /************************************************************************
         * Trigger request error handling if request failed.
         ************************************************************************/
            HelloWatchdogClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            HelloWatchdogClientBase::invalidResponse(msgId);
        }
        break;

    case NEService::eResultType::DataOK:            // fall through
    case NEService::eResultType::DataInvalid:
        {
            NEService::eDataStateType dataState;
            switch (msgId)
            {
        /************************************************************************
         * Trigger attribute update processing
         ************************************************************************/
            case NEHelloWatchdog::eMessageIDs::MsgId_ServiceState:
                {
                    NEHelloWatchdog::eState ServiceState = mProxy->getServiceState(dataState);
                    onServiceStateUpdate(ServiceState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_processNotificationEvent);
                    TRACE_ERR("Client object HelloWatchdogClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::eResultType::RequestOK:
        {
            switch (msgId)
            {
        /************************************************************************
         * Trigger response processing
         ************************************************************************/
            case NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep:
                {
                    unsigned int timeoutSleep = mProxy->getParamtimeoutSleep();
                    responseStartSleep( timeoutSleep );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_processNotificationEvent);
                    TRACE_ERR("Client object HelloWatchdogClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }        
        break;

    default:
        {
            TRACE_SCOPE(generated_src_HelloWatchdogClientBase_processNotificationEvent);
            TRACE_ERR("Client object HelloWatchdogClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_invalidResponse);
void HelloWatchdogClientBase::invalidResponse( NEHelloWatchdog::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloWatchdogClientBase is not implemented! Make error handling!"
                    , NEHelloWatchdog::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_invalidRequest);
void HelloWatchdogClientBase::invalidRequest( NEHelloWatchdog::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloWatchdogClientBase is not implemented! Make error handling!"
                    , NEHelloWatchdog::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestFailed);
void HelloWatchdogClientBase::requestFailed( NEHelloWatchdog::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloWatchdogClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEHelloWatchdog::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NEHelloWatchdog::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEHelloWatchdog::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEHelloWatchdog::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEHelloWatchdog::eMessageIDs::MsgId_Invalid) && (index < NEHelloWatchdog::getInterfaceData().idRequestCount) )
    {
        (this->*NEHelloWatchdog::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_onServiceStateUpdate);
void HelloWatchdogClientBase::onServiceStateUpdate( NEHelloWatchdog::eState /* ServiceState */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_onServiceStateUpdate);
    TRACE_WARN("The attribute ServiceState (value = %u) update method of proxy [ %s ] client HelloWatchdogClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloWatchdog::eMessageIDs::MsgId_ServiceState)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestStartSleepFailed);
void HelloWatchdogClientBase::requestStartSleepFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestStartSleepFailed);
    TRACE_WARN("The request requestStartSleep (value = %u) method of proxy [ %s ] client HelloWatchdogClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloWatchdog::eMessageIDs::MsgId_requestStartSleep)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestStopServiceFailed);
void HelloWatchdogClientBase::requestStopServiceFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestStopServiceFailed);
    TRACE_WARN("The request requestStopService (value = %u) method of proxy [ %s ] client HelloWatchdogClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloWatchdog::eMessageIDs::MsgId_requestStopService)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestShutdownServiceFailed);
void HelloWatchdogClientBase::requestShutdownServiceFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_requestShutdownServiceFailed);
    TRACE_WARN("The request requestShutdownService (value = %u) method of proxy [ %s ] client HelloWatchdogClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloWatchdog::eMessageIDs::MsgId_requestShutdownService)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogClientBase_responseStartSleep);
void HelloWatchdogClientBase::responseStartSleep( unsigned int /* timeoutSleep */ )
{
    TRACE_SCOPE(generated_src_HelloWatchdogClientBase_responseStartSleep);
    TRACE_WARN("The response responseStartSleep (value = %u) method of proxy [ %s ] client HelloWatchdogClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWatchdogClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
