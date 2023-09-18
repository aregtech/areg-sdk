//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubMixClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/PubSubMixClientBase.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/PubSubMixClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEPubSubMix
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (PubSubMixClientBase::* FuncRequestFailure) ( NEService::eResultType );
#ifdef  _DEBUG
    static FuncRequestFailure failureFunctions = nullptr;
#endif  // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// PubSubMixClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

PubSubMixClientBase::PubSubMixClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubMixProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PubSubMixClientBase::PubSubMixClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubMixProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PubSubMixClientBase::PubSubMixClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubMixProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PubSubMixClientBase::PubSubMixClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubMixProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PubSubMixClientBase::~PubSubMixClientBase( void )
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

bool PubSubMixClientBase::recreateProxy( void )
{
    bool result { false };
    if (mProxy != nullptr)
    {
        const String & roleName   { mProxy->getProxyAddress().getRoleName() };
        const String & threadName { mProxy->getProxyAddress().getThread()   };
        if ( roleName.isEmpty() == false )
        {
            PubSubMixProxy * newProxy = PubSubMixProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * PubSubMixClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_serviceConnected);
bool PubSubMixClientBase::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_PubSubMixClientBase_serviceConnected);

    bool result { false };
    if(mProxy == &proxy)
    {
        mIsConnected= NEService::isServiceConnected(status);
        result      = true;

        TRACE_DBG("Client [ %p ]: The Service [ %s ] with Role Name [ %s ] is [ %s ]"
                 , this
                 , proxy.getProxyAddress().getServiceName().getString()
                 , proxy.getProxyAddress().getRoleName().getString()
                 , mIsConnected ? "CONNECTED" : "DISCONNECTED");
    }

    return result;
}

void PubSubMixClientBase::notifyOn( NEPubSubMix::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_processNotificationEvent);
void PubSubMixClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  { static_cast<const NotificationEvent &>(eventElem).getData() };
    NEService::eResultType result       { data.getNotifyType() };
    NEPubSubMix::eMessageIDs msgId   { static_cast<NEPubSubMix::eMessageIDs>(data.getNotifyId()) };
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
            PubSubMixClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            PubSubMixClientBase::invalidResponse(msgId);
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
            case NEPubSubMix::eMessageIDs::MsgId_StringOnChange:
                {
                    const NEPubSubMix::sString & StringOnChange = mProxy->getStringOnChange(dataState);
                    onStringOnChangeUpdate(StringOnChange, dataState);
                }
                break;

            case NEPubSubMix::eMessageIDs::MsgId_IntegerAlways:
                {
                    const NEPubSubMix::sInteger & IntegerAlways = mProxy->getIntegerAlways(dataState);
                    onIntegerAlwaysUpdate(IntegerAlways, dataState);
                }
                break;

            case NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState:
                {
                    NEPubSubMix::eServiceState ServiceProviderState = mProxy->getServiceProviderState(dataState);
                    onServiceProviderStateUpdate(ServiceProviderState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_PubSubMixClientBase_processNotificationEvent);
                    TRACE_ERR("Client object PubSubMixClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_PubSubMixClientBase_processNotificationEvent);
            TRACE_ERR("Client object PubSubMixClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_invalidResponse);
void PubSubMixClientBase::invalidResponse( NEPubSubMix::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_PubSubMixClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client PubSubMixClientBase is not implemented! Make error handling!"
                    , NEPubSubMix::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_invalidRequest);
void PubSubMixClientBase::invalidRequest( NEPubSubMix::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_PubSubMixClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PubSubMixClientBase is not implemented! Make error handling!"
                    , NEPubSubMix::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_requestFailed);
void PubSubMixClientBase::requestFailed( NEPubSubMix::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PubSubMixClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PubSubMixClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEPubSubMix::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    ASSERT(NEPubSubMix::failureFunctions == nullptr);
    invalidRequest( FailureMsgId );
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_onStringOnChangeUpdate);
void PubSubMixClientBase::onStringOnChangeUpdate( const NEPubSubMix::sString & /* StringOnChange */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PubSubMixClientBase_onStringOnChangeUpdate);
    TRACE_WARN("The attribute StringOnChange (value = %u) update method of proxy [ %s ] client PubSubMixClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPubSubMix::eMessageIDs::MsgId_StringOnChange)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_onIntegerAlwaysUpdate);
void PubSubMixClientBase::onIntegerAlwaysUpdate( const NEPubSubMix::sInteger & /* IntegerAlways */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PubSubMixClientBase_onIntegerAlwaysUpdate);
    TRACE_WARN("The attribute IntegerAlways (value = %u) update method of proxy [ %s ] client PubSubMixClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPubSubMix::eMessageIDs::MsgId_IntegerAlways)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_PubSubMixClientBase_onServiceProviderStateUpdate);
void PubSubMixClientBase::onServiceProviderStateUpdate( NEPubSubMix::eServiceState /* ServiceProviderState */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PubSubMixClientBase_onServiceProviderStateUpdate);
    TRACE_WARN("The attribute ServiceProviderState (value = %u) update method of proxy [ %s ] client PubSubMixClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubMixClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
