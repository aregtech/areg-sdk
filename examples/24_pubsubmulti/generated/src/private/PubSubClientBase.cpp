//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:54 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/PubSubClientBase.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/PubSubClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEPubSub
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (PubSubClientBase::* FuncRequestFailure) ( NEService::eResultType );
#ifdef  _DEBUG
    static FuncRequestFailure failureFunctions = nullptr;
#endif  // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// PubSubClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

PubSubClientBase::PubSubClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PubSubClientBase::PubSubClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PubSubClientBase::PubSubClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PubSubClientBase::PubSubClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PubSubProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PubSubClientBase::~PubSubClientBase( void )
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

bool PubSubClientBase::recreateProxy( void )
{
    bool result { false };
    if (mProxy != nullptr)
    {
        const String & roleName   { mProxy->getProxyAddress().getRoleName() };
        const String & threadName { mProxy->getProxyAddress().getThread()   };
        if ( roleName.isEmpty() == false )
        {
            PubSubProxy * newProxy = PubSubProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * PubSubClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_PubSubClientBase_serviceConnected);
bool PubSubClientBase::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_PubSubClientBase_serviceConnected);

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

void PubSubClientBase::notifyOn( NEPubSub::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PubSubClientBase_processNotificationEvent);
void PubSubClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  { static_cast<const NotificationEvent &>(eventElem).getData() };
    NEService::eResultType result       { data.getNotifyType() };
    NEPubSub::eMessageIDs msgId   { static_cast<NEPubSub::eMessageIDs>(data.getNotifyId()) };
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
            PubSubClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            PubSubClientBase::invalidResponse(msgId);
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
            case NEPubSub::eMessageIDs::MsgId_StringOnChange:
                {
                    const String & StringOnChange = mProxy->getStringOnChange(dataState);
                    onStringOnChangeUpdate(StringOnChange, dataState);
                }
                break;

            case NEPubSub::eMessageIDs::MsgId_IntegerAlways:
                {
                    unsigned int IntegerAlways = mProxy->getIntegerAlways(dataState);
                    onIntegerAlwaysUpdate(IntegerAlways, dataState);
                }
                break;

            case NEPubSub::eMessageIDs::MsgId_ServiceProviderState:
                {
                    NEPubSub::eServiceState ServiceProviderState = mProxy->getServiceProviderState(dataState);
                    onServiceProviderStateUpdate(ServiceProviderState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_PubSubClientBase_processNotificationEvent);
                    TRACE_ERR("Client object PubSubClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_PubSubClientBase_processNotificationEvent);
            TRACE_ERR("Client object PubSubClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_PubSubClientBase_invalidResponse);
void PubSubClientBase::invalidResponse( NEPubSub::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_PubSubClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client PubSubClientBase is not implemented! Make error handling!"
                    , NEPubSub::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PubSubClientBase_invalidRequest);
void PubSubClientBase::invalidRequest( NEPubSub::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_PubSubClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PubSubClientBase is not implemented! Make error handling!"
                    , NEPubSub::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PubSubClientBase_requestFailed);
void PubSubClientBase::requestFailed( NEPubSub::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PubSubClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PubSubClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEPubSub::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    ASSERT(NEPubSub::failureFunctions == nullptr);
    invalidRequest( FailureMsgId );
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PubSubClientBase_onStringOnChangeUpdate);
void PubSubClientBase::onStringOnChangeUpdate( const String & /* StringOnChange */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PubSubClientBase_onStringOnChangeUpdate);
    TRACE_WARN("The attribute StringOnChange (value = %u) update method of proxy [ %s ] client PubSubClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPubSub::eMessageIDs::MsgId_StringOnChange)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_PubSubClientBase_onIntegerAlwaysUpdate);
void PubSubClientBase::onIntegerAlwaysUpdate( unsigned int /* IntegerAlways */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PubSubClientBase_onIntegerAlwaysUpdate);
    TRACE_WARN("The attribute IntegerAlways (value = %u) update method of proxy [ %s ] client PubSubClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPubSub::eMessageIDs::MsgId_IntegerAlways)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_PubSubClientBase_onServiceProviderStateUpdate);
void PubSubClientBase::onServiceProviderStateUpdate( NEPubSub::eServiceState /* ServiceProviderState */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PubSubClientBase_onServiceProviderStateUpdate);
    TRACE_WARN("The attribute ServiceProviderState (value = %u) update method of proxy [ %s ] client PubSubClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPubSub::eMessageIDs::MsgId_ServiceProviderState)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
