//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/TrafficControllerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/TrafficControllerClientBase.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/TrafficControllerClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NETrafficController
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (TrafficControllerClientBase::* FuncRequestFailure) ( NEService::eResultType );
#ifdef  _DEBUG
    static FuncRequestFailure failureFunctions = nullptr;
#endif  // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// TrafficControllerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

TrafficControllerClientBase::TrafficControllerClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( TrafficControllerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

TrafficControllerClientBase::TrafficControllerClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( TrafficControllerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

TrafficControllerClientBase::TrafficControllerClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( TrafficControllerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

TrafficControllerClientBase::TrafficControllerClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( TrafficControllerProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

TrafficControllerClientBase::~TrafficControllerClientBase( void )
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

bool TrafficControllerClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            TrafficControllerProxy * newProxy = TrafficControllerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * TrafficControllerClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_serviceConnected);
bool TrafficControllerClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_serviceConnected);

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

void TrafficControllerClientBase::notifyOn( NETrafficController::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_processNotificationEvent);
void TrafficControllerClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NETrafficController::eMessageIDs msgId   = static_cast<NETrafficController::eMessageIDs>(data.getNotifyId());
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
            TrafficControllerClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            TrafficControllerClientBase::invalidResponse(msgId);
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
            case NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth:
                {
                    const NETrafficController::sTrafficLight & TrafficSouthNorth = mProxy->getTrafficSouthNorth(dataState);
                    onTrafficSouthNorthUpdate(TrafficSouthNorth, dataState);
                }
                break;

            case NETrafficController::eMessageIDs::MsgId_TrafficEastWest:
                {
                    const NETrafficController::sTrafficLight & TrafficEastWest = mProxy->getTrafficEastWest(dataState);
                    onTrafficEastWestUpdate(TrafficEastWest, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_TrafficControllerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object TrafficControllerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth:
                {
                    NETrafficController::eVehicleTrafficLight LightVehicle = mProxy->getParamLightVehicle();
                    NETrafficController::ePedestrianTrafficLight LightPedestrian = mProxy->getParamLightPedestrian();
                    broadcastSouthNorth( LightVehicle, LightPedestrian );
                }
                break;

            case NETrafficController::eMessageIDs::MsgId_broadcastEastWest:
                {
                    NETrafficController::eVehicleTrafficLight LightVehicle = mProxy->getParamLightVehicle();
                    NETrafficController::ePedestrianTrafficLight LightPedestrian = mProxy->getParamLightPedestrian();
                    broadcastEastWest( LightVehicle, LightPedestrian );
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_TrafficControllerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object TrafficControllerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_TrafficControllerClientBase_processNotificationEvent);
            TRACE_ERR("Client object TrafficControllerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_invalidResponse);
void TrafficControllerClientBase::invalidResponse( NETrafficController::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client TrafficControllerClientBase is not implemented! Make error handling!"
                    , NETrafficController::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_invalidRequest);
void TrafficControllerClientBase::invalidRequest( NETrafficController::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client TrafficControllerClientBase is not implemented! Make error handling!"
                    , NETrafficController::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_requestFailed);
void TrafficControllerClientBase::requestFailed( NETrafficController::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client TrafficControllerClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NETrafficController::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    ASSERT(NETrafficController::failureFunctions == nullptr);
    invalidRequest( FailureMsgId );
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_onTrafficSouthNorthUpdate);
void TrafficControllerClientBase::onTrafficSouthNorthUpdate( const NETrafficController::sTrafficLight & /* TrafficSouthNorth */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_onTrafficSouthNorthUpdate);
    TRACE_WARN("The attribute TrafficSouthNorth (value = %u) update method of proxy [ %s ] client TrafficControllerClientBase is not implemented!"
                    , static_cast<unsigned int>(NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_onTrafficEastWestUpdate);
void TrafficControllerClientBase::onTrafficEastWestUpdate( const NETrafficController::sTrafficLight & /* TrafficEastWest */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_onTrafficEastWestUpdate);
    TRACE_WARN("The attribute TrafficEastWest (value = %u) update method of proxy [ %s ] client TrafficControllerClientBase is not implemented!"
                    , static_cast<unsigned int>(NETrafficController::eMessageIDs::MsgId_TrafficEastWest)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_broadcastSouthNorth);
void TrafficControllerClientBase::broadcastSouthNorth( NETrafficController::eVehicleTrafficLight /* LightVehicle */, NETrafficController::ePedestrianTrafficLight /* LightPedestrian */ )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_broadcastSouthNorth);
    TRACE_WARN("The broadcast broadcastSouthNorth (value = %u) method of proxy [ %s ] client TrafficControllerClientBase is not implemented!"
                    , static_cast<unsigned int>(NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerClientBase_broadcastEastWest);
void TrafficControllerClientBase::broadcastEastWest( NETrafficController::eVehicleTrafficLight /* LightVehicle */, NETrafficController::ePedestrianTrafficLight /* LightPedestrian */ )
{
    TRACE_SCOPE(generated_src_TrafficControllerClientBase_broadcastEastWest);
    TRACE_WARN("The broadcast broadcastEastWest (value = %u) method of proxy [ %s ] client TrafficControllerClientBase is not implemented!"
                    , static_cast<unsigned int>(NETrafficController::eMessageIDs::MsgId_broadcastEastWest)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/TrafficControllerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
