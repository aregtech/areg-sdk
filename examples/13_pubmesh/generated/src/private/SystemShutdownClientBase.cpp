//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SystemShutdownClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     18.12.2022  15:17:33 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/SystemShutdownClientBase.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/SystemShutdownClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NESystemShutdown
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (SystemShutdownClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
        &SystemShutdownClientBase::requestSystemShutdownFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// SystemShutdownClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

SystemShutdownClientBase::SystemShutdownClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SystemShutdownProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

SystemShutdownClientBase::SystemShutdownClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SystemShutdownProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

SystemShutdownClientBase::SystemShutdownClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SystemShutdownProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

SystemShutdownClientBase::SystemShutdownClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SystemShutdownProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

SystemShutdownClientBase::~SystemShutdownClientBase( void )
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

bool SystemShutdownClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            SystemShutdownProxy * newProxy = SystemShutdownProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * SystemShutdownClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_SystemShutdownClientBase_serviceConnected);
bool SystemShutdownClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_SystemShutdownClientBase_serviceConnected);

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

void SystemShutdownClientBase::notifyOn( NESystemShutdown::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SystemShutdownClientBase_processNotificationEvent);
void SystemShutdownClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NESystemShutdown::eMessageIDs msgId   = static_cast<NESystemShutdown::eMessageIDs>(data.getNotifyId());
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
            SystemShutdownClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            SystemShutdownClientBase::invalidResponse(msgId);
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
            case NESystemShutdown::eMessageIDs::MsgId_ServiceState:
                {
                    NESystemShutdown::eServiceState ServiceState = mProxy->getServiceState(dataState);
                    onServiceStateUpdate(ServiceState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_SystemShutdownClientBase_processNotificationEvent);
                    TRACE_ERR("Client object SystemShutdownClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::eResultType::RequestOK:        
        break;

    default:
        {
            TRACE_SCOPE(generated_src_SystemShutdownClientBase_processNotificationEvent);
            TRACE_ERR("Client object SystemShutdownClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_SystemShutdownClientBase_invalidResponse);
void SystemShutdownClientBase::invalidResponse( NESystemShutdown::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_SystemShutdownClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client SystemShutdownClientBase is not implemented! Make error handling!"
                    , NESystemShutdown::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_SystemShutdownClientBase_invalidRequest);
void SystemShutdownClientBase::invalidRequest( NESystemShutdown::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_SystemShutdownClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client SystemShutdownClientBase is not implemented! Make error handling!"
                    , NESystemShutdown::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_SystemShutdownClientBase_requestFailed);
void SystemShutdownClientBase::requestFailed( NESystemShutdown::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_SystemShutdownClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client SystemShutdownClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NESystemShutdown::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NESystemShutdown::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NESystemShutdown::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NESystemShutdown::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NESystemShutdown::eMessageIDs::MsgId_Invalid) && (index < NESystemShutdown::getInterfaceData().idRequestCount) )
    {
        (this->*NESystemShutdown::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SystemShutdownClientBase_onServiceStateUpdate);
void SystemShutdownClientBase::onServiceStateUpdate( NESystemShutdown::eServiceState /* ServiceState */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_SystemShutdownClientBase_onServiceStateUpdate);
    TRACE_WARN("The attribute ServiceState (value = %u) update method of proxy [ %s ] client SystemShutdownClientBase is not implemented!"
                    , static_cast<unsigned int>(NESystemShutdown::eMessageIDs::MsgId_ServiceState)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SystemShutdownClientBase_requestSystemShutdownFailed);
void SystemShutdownClientBase::requestSystemShutdownFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_SystemShutdownClientBase_requestSystemShutdownFailed);
    TRACE_WARN("The request requestSystemShutdown (value = %u) method of proxy [ %s ] client SystemShutdownClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NESystemShutdown::eMessageIDs::MsgId_requestSystemShutdown)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SystemShutdownClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
