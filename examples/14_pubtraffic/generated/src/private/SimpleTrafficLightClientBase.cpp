//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficLightClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:19:32 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 *
 * \file            generated/src/SimpleTrafficLightClientBase.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/SimpleTrafficLightClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NESimpleTrafficLight
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (SimpleTrafficLightClientBase::* FuncRequestFailure) ( NEService::eResultType );
#ifdef  _DEBUG
    static FuncRequestFailure failureFunctions = nullptr;
#endif  // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

SimpleTrafficLightClientBase::SimpleTrafficLightClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SimpleTrafficLightProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

SimpleTrafficLightClientBase::SimpleTrafficLightClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SimpleTrafficLightProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

SimpleTrafficLightClientBase::SimpleTrafficLightClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SimpleTrafficLightProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

SimpleTrafficLightClientBase::SimpleTrafficLightClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SimpleTrafficLightProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

SimpleTrafficLightClientBase::~SimpleTrafficLightClientBase( void )
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

bool SimpleTrafficLightClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            SimpleTrafficLightProxy * newProxy = SimpleTrafficLightProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * SimpleTrafficLightClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_serviceConnected);
bool SimpleTrafficLightClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_serviceConnected);

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

void SimpleTrafficLightClientBase::notifyOn( NESimpleTrafficLight::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_processNotificationEvent);
void SimpleTrafficLightClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NESimpleTrafficLight::eMessageIDs msgId   = static_cast<NESimpleTrafficLight::eMessageIDs>(data.getNotifyId());
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
            SimpleTrafficLightClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            SimpleTrafficLightClientBase::invalidResponse(msgId);
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
            case NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth:
                {
                    NESimpleTrafficLight::eTrafficLight SouthNorth = mProxy->getSouthNorth(dataState);
                    onSouthNorthUpdate(SouthNorth, dataState);
                }
                break;

            case NESimpleTrafficLight::eMessageIDs::MsgId_EastWest:
                {
                    NESimpleTrafficLight::eTrafficLight EastWest = mProxy->getEastWest(dataState);
                    onEastWestUpdate(EastWest, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_processNotificationEvent);
                    TRACE_ERR("Client object SimpleTrafficLightClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NESimpleTrafficLight::eMessageIDs::MsgId_broadcastLightChanged:
                {
                    NESimpleTrafficLight::eTrafficLight SouthNorth = mProxy->getParamSouthNorth();
                    NESimpleTrafficLight::eTrafficLight EastWest = mProxy->getParamEastWest();
                    broadcastLightChanged( SouthNorth, EastWest );
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_processNotificationEvent);
                    TRACE_ERR("Client object SimpleTrafficLightClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_processNotificationEvent);
            TRACE_ERR("Client object SimpleTrafficLightClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_invalidResponse);
void SimpleTrafficLightClientBase::invalidResponse( NESimpleTrafficLight::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client SimpleTrafficLightClientBase is not implemented! Make error handling!"
                    , NESimpleTrafficLight::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_invalidRequest);
void SimpleTrafficLightClientBase::invalidRequest( NESimpleTrafficLight::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client SimpleTrafficLightClientBase is not implemented! Make error handling!"
                    , NESimpleTrafficLight::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_requestFailed);
void SimpleTrafficLightClientBase::requestFailed( NESimpleTrafficLight::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client SimpleTrafficLightClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NESimpleTrafficLight::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    ASSERT(NESimpleTrafficLight::failureFunctions == nullptr);
    invalidRequest( FailureMsgId );
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_onSouthNorthUpdate);
void SimpleTrafficLightClientBase::onSouthNorthUpdate( NESimpleTrafficLight::eTrafficLight /* SouthNorth */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_onSouthNorthUpdate);
    TRACE_WARN("The attribute SouthNorth (value = %u) update method of proxy [ %s ] client SimpleTrafficLightClientBase is not implemented!"
                    , static_cast<unsigned int>(NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_onEastWestUpdate);
void SimpleTrafficLightClientBase::onEastWestUpdate( NESimpleTrafficLight::eTrafficLight /* EastWest */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_onEastWestUpdate);
    TRACE_WARN("The attribute EastWest (value = %u) update method of proxy [ %s ] client SimpleTrafficLightClientBase is not implemented!"
                    , static_cast<unsigned int>(NESimpleTrafficLight::eMessageIDs::MsgId_EastWest)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_broadcastLightChanged);
void SimpleTrafficLightClientBase::broadcastLightChanged( NESimpleTrafficLight::eTrafficLight /* SouthNorth */, NESimpleTrafficLight::eTrafficLight /* EastWest */ )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightClientBase_broadcastLightChanged);
    TRACE_WARN("The broadcast broadcastLightChanged (value = %u) method of proxy [ %s ] client SimpleTrafficLightClientBase is not implemented!"
                    , static_cast<unsigned int>(NESimpleTrafficLight::eMessageIDs::MsgId_broadcastLightChanged)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficLightClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
